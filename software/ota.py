import gc
import machine
import ubinascii
import utime

try:
    import mimxrt
except ImportError:
    mimxrt = None

PROTOCOL = 1


class ota_updater:
    BLOCK_SIZE = 4096
    APP_ADDR = 0x40000
    APP_MAX_SIZE = 0x3C0000
    ROMFS_ADDR = 0x800000
    ROMFS_MAX_SIZE = 0x400000
    STAGING_ADDR = 0xC00000
    IDLE_TIMEOUT_MS = 15000

    def __init__(self, comms):
        self.comms = comms
        # The main loop is paused while a session is open.
        self.session = False
        self.active = False
        self.verified = False
        self.romfs_dirty = False
        self.expected_size = 0
        self.expected_crc = 0
        self.received_bytes = 0
        self.target_addr = 0
        self.write_addr = 0
        self.erased_until = 0
        self.last_activity = 0

    def info(self):
        backend = 'mimxrt' if mimxrt else 'none'
        self.comms.send_data(f'otainfo:{backend}:{PROTOCOL}')

    def begin(self, addr_hex, size_str, crc_hex):
        if not mimxrt:
            self.comms.send_data('ota:error:unsupported')
            return

        try:
            target_addr = int(addr_hex, 16)
            expected_size = int(size_str)
            expected_crc = int(crc_hex, 16)
        except ValueError:
            self.comms.send_data('ota:error:bad_arguments')
            return

        if target_addr == self.APP_ADDR:
            max_size = self.APP_MAX_SIZE
            write_addr = self.STAGING_ADDR
        elif target_addr == self.ROMFS_ADDR:
            max_size = self.ROMFS_MAX_SIZE
            write_addr = self.ROMFS_ADDR
        else:
            self.comms.send_data('ota:error:bad_address')
            return

        if expected_size <= 0 or expected_size > max_size:
            self.comms.send_data('ota:error:bad_size')
            return

        self.comms.end_ota()
        self.session = True
        self.active = False
        self.verified = False
        self.target_addr = target_addr
        self.write_addr = write_addr
        self.expected_size = expected_size
        self.expected_crc = expected_crc
        self.received_bytes = 0
        self.erased_until = write_addr
        self.touch()
        gc.collect()

        # Skip regions that already hold the requested image.
        if self.crc(target_addr, expected_size) == expected_crc:
            self.touch()
            self.comms.send_data('ota:unchanged')
            return

        if target_addr == self.ROMFS_ADDR:
            self.romfs_dirty = True

        self.active = True
        self.comms.start_ota(self.receive_chunk)
        self.touch()
        self.comms.send_data('ota:ready')

    def receive_chunk(self, data):
        if not self.active or not data:
            return

        if data == b'ota.abort':
            self.abort()
            return

        self.touch()
        length = len(data)
        if self.received_bytes + length > self.expected_size:
            self.abort('overflow')
            return

        start = self.write_addr + self.received_bytes
        end = start + length
        try:
            # Erase every sector the chunk reaches. Chunks often straddle a sector boundary.
            while self.erased_until < end:
                mimxrt.flash_erase(self.erased_until)
                self.erased_until += self.BLOCK_SIZE
            mimxrt.flash_write(start, data)
        except Exception as e:
            print("OTA write error", e)
            self.abort('write_failed')
            return

        previous = self.received_bytes
        self.received_bytes += length

        if self.received_bytes == self.expected_size:
            self.active = False
            self.comms.end_ota()
            self.comms.send_data(f'ota:complete:{self.received_bytes}')
        elif self.received_bytes // self.BLOCK_SIZE != previous // self.BLOCK_SIZE:
            # Acknowledge every 4KB so the app can pace the transfer.
            self.comms.send_data(f'ota:{self.received_bytes}')

    def crc(self, addr, size):
        crc = 0
        while size > 0:
            chunk_size = min(self.BLOCK_SIZE, size)
            crc = ubinascii.crc32(mimxrt.flash_read(addr, chunk_size), crc)
            addr += chunk_size
            size -= chunk_size
        return crc & 0xFFFFFFFF

    def verify(self):
        if not self.session or self.active or self.received_bytes != self.expected_size:
            self.comms.send_data(f'ota:error:size_mismatch:{self.received_bytes}')
            return

        self.touch()
        crc = self.crc(self.write_addr, self.expected_size)
        self.touch()
        if crc == self.expected_crc:
            self.verified = True
            self.comms.send_data('ota:verified')
        else:
            self.comms.send_data(f'ota:crc_mismatch:{crc:08X}')

    def apply(self):
        if not self.verified:
            self.comms.send_data('ota:error:not_verified')
            return

        self.verified = False
        self.touch()
        if self.target_addr == self.ROMFS_ADDR:
            # ROMFS was written in place.
            self.comms.send_data('ota:applied')
        else:
            self.comms.send_data('ota:applying')
            # Give the notification time to leave before IRQs are disabled.
            utime.sleep_ms(500)
            try:
                mimxrt.flash_copy_and_reset(self.STAGING_ADDR, self.target_addr, self.expected_size)
            except Exception as e:
                print("OTA apply error", e)
                self.comms.send_data('ota:error:apply_failed')

    def end(self):
        self.comms.send_data('ota:ended')
        self.close()

    def abort(self, reason=None):
        self.active = False
        self.received_bytes = 0
        self.comms.end_ota()
        self.comms.send_data(f'ota:aborted:{reason}' if reason else 'ota:aborted')
        self.close()

    def close(self):
        if self.romfs_dirty:
            utime.sleep_ms(500)
            machine.reset()
        self.active = False
        self.verified = False
        self.session = False

    def touch(self):
        self.last_activity = utime.ticks_ms()

    def poll(self):
        if not self.session:
            return
        if not self.comms.connected:
            self.abort('disconnected')
        elif utime.ticks_diff(utime.ticks_ms(), self.last_activity) > self.IDLE_TIMEOUT_MS:
            self.abort('timeout')
