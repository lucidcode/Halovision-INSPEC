import gc
import machine
import ubinascii
import utime

try:
    import mimxrt
except ImportError:
    mimxrt = None

PROTOCOL = 1

# ROMFS record format, see tools/mkromfs.py and extmod/vfs_rom.c.
ROMFS_MAGIC = b'\xd2\xcd\x31'
KIND_PADDING = 1
KIND_DATA = 2
KIND_FILE = 5


def read_vint(buf, pos):
    value = 0
    while True:
        byte = buf[pos]
        pos += 1
        value = (value << 7) | (byte & 0x7F)
        if not byte & 0x80:
            return value, pos


def encode_vint(value):
    encoded = [value & 0x7F]
    value >>= 7
    while value:
        encoded.insert(0, 0x80 | (value & 0x7F))
        value >>= 7
    return bytes(encoded)


class ota_updater:
    BLOCK_SIZE = 4096
    APP_ADDR = 0x40000
    APP_MAX_SIZE = 0x3C0000
    ROMFS_ADDR = 0x800000
    ROMFS_SIZE = 0x800000
    FLASH_END = ROMFS_ADDR + ROMFS_SIZE
    RECORD_PEEK = 256
    IDLE_TIMEOUT_MS = 15000
    COPY_DELAY_MS = 500

    def __init__(self, comms):
        self.comms = comms
        # The main loop is paused while a session is open.
        self.session = False
        self.active = False
        self.verified = False
        self.append = False
        self.romfs_dirty = False
        self.expected_size = 0
        self.expected_crc = 0
        self.received_bytes = 0
        self.target_addr = 0
        self.write_addr = 0
        self.erased_until = 0
        self.last_activity = 0
        self.copy_at = None

    def info(self):
        backend = 'mimxrt' if mimxrt else 'none'
        self.comms.send_data(f'otainfo:{backend}:{PROTOCOL}')

    def romfs_info(self):
        # Lists the ROMFS files so the app only sends files that are missing or changed.
        if not mimxrt:
            self.comms.send_data('ota:error:unsupported')
            return

        bounds = self.romfs_bounds()
        if bounds is None:
            self.comms.send_data(f'ota:romfs:0:{self.ROMFS_SIZE}')
            return

        try:
            files = self.romfs_files(bounds[0], bounds[1])
        except ValueError:
            self.comms.send_data('ota:error:romfs_corrupt')
            return

        seen = set()
        for offset, name, data_offset, size in files:
            # Lookups return the first match, so later copies are shadowed.
            if name in seen:
                continue
            seen.add(name)
            crc = self.crc(self.ROMFS_ADDR + data_offset, size)
            self.comms.send_data(f'ota:file:{offset}:{size}:{crc:08X}:{name.decode()}')
            utime.sleep_ms(10)
        self.comms.send_data(f'ota:romfs:{bounds[1]}:{self.ROMFS_SIZE}')

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

        append = False
        bounds = self.romfs_bounds()
        romfs_end = self.ROMFS_ADDR + (bounds[1] if bounds else 0)
        if target_addr == self.APP_ADDR:
            max_size = self.APP_MAX_SIZE
            # Stage after the ROMFS image, so files added with OpenMV IDE survive.
            write_addr = self.align(romfs_end)
        elif target_addr == self.ROMFS_ADDR:
            max_size = self.ROMFS_SIZE
            write_addr = self.ROMFS_ADDR
        elif bounds and target_addr == romfs_end:
            # New file records, appended after the last file.
            max_size = self.ROMFS_SIZE
            write_addr = romfs_end
            append = True
        else:
            self.comms.send_data('ota:error:bad_address')
            return

        if expected_size <= 0 or expected_size > max_size:
            self.comms.send_data('ota:error:bad_size')
            return
        if write_addr + expected_size > self.FLASH_END:
            self.comms.send_data('ota:error:no_space')
            return

        self.comms.end_ota()
        self.session = True
        self.active = False
        self.verified = False
        self.append = append
        self.target_addr = target_addr
        self.write_addr = write_addr
        self.expected_size = expected_size
        self.expected_crc = expected_crc
        self.received_bytes = 0
        self.erased_until = self.align(write_addr)
        self.touch()
        gc.collect()

        # Skip regions that already hold the requested image.
        if not append and self.crc(target_addr, expected_size) == expected_crc:
            self.touch()
            self.comms.send_data('ota:unchanged')
            return

        if target_addr != self.APP_ADDR:
            self.romfs_dirty = True

        try:
            if append:
                self.clear_tail(write_addr)
            # Erase everything up front. Erasing during the transfer stalls the BLE link.
            erase_end = self.align(write_addr + expected_size)
            mimxrt.flash_erase(self.erased_until, erase_end - self.erased_until)
            self.erased_until = erase_end
        except Exception as e:
            print("OTA erase error", e)
            self.abort('write_failed')
            return

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
        if self.append:
            try:
                self.commit_romfs()
            except ValueError:
                self.comms.send_data('ota:error:bad_records')
                return
            except Exception as e:
                print("OTA commit error", e)
                self.comms.send_data('ota:error:write_failed')
                return
            self.touch()
            self.comms.send_data('ota:applied')
        elif self.target_addr == self.ROMFS_ADDR:
            # ROMFS was written in place.
            self.comms.send_data('ota:applied')
        else:
            # Commands run inside the BLE write callback, and the write is only acknowledged
            # once it returns. The copy never returns, so poll() starts it from the main loop.
            self.copy_at = utime.ticks_add(utime.ticks_ms(), self.COPY_DELAY_MS)
            self.comms.send_data('ota:applying')

    def copy_firmware(self):
        self.copy_at = None
        try:
            mimxrt.flash_copy_and_reset(self.write_addr, self.target_addr, self.expected_size)
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
        self.copy_at = None
        self.session = False

    def touch(self):
        self.last_activity = utime.ticks_ms()

    def poll(self):
        if not self.session:
            return
        if self.copy_at is not None:
            # Give the acknowledgement and notification time to leave before IRQs are disabled.
            if utime.ticks_diff(utime.ticks_ms(), self.copy_at) >= 0:
                self.copy_firmware()
            return
        if not self.comms.connected:
            self.abort('disconnected')
        elif utime.ticks_diff(utime.ticks_ms(), self.last_activity) > self.IDLE_TIMEOUT_MS:
            self.abort('timeout')

    def align(self, addr):
        return (addr + self.BLOCK_SIZE - 1) & ~(self.BLOCK_SIZE - 1)

    def romfs_bounds(self):
        # Returns where the ROMFS payload starts and ends, relative to ROMFS_ADDR, or None.
        head = mimxrt.flash_read(self.ROMFS_ADDR, 32)
        if head[:3] != ROMFS_MAGIC:
            return None
        try:
            _, start = read_vint(head, 0)
            length, start = read_vint(head, start)
        except IndexError:
            return None
        if start + length > self.ROMFS_SIZE:
            return None
        return start, start + length

    def romfs_files(self, start, end):
        # Lists the top-level files between start and end as (record offset, name, data offset,
        # data size). Raises ValueError if a record is malformed.
        files = []
        pos = start
        while pos < end:
            buf = mimxrt.flash_read(self.ROMFS_ADDR + pos, min(self.RECORD_PEEK, end - pos))
            try:
                kind, i = read_vint(buf, 0)
                length, i = read_vint(buf, i)
                next_pos = pos + i + length
                if kind == 0 or next_pos > end:
                    raise ValueError
                if kind == KIND_FILE:
                    name_len, i = read_vint(buf, i)
                    name = buf[i:i + name_len]
                    i += name_len
                    data_kind, i = read_vint(buf, i)
                    data_len, i = read_vint(buf, i)
                    if data_kind != KIND_DATA or len(name) != name_len or pos + i + data_len > next_pos:
                        raise ValueError
                    files.append((pos, name, pos + i, data_len))
            except IndexError:
                raise ValueError
            pos = next_pos
        return files

    def clear_tail(self, addr):
        # Appended records share a sector with the end of the image. The rest of that sector
        # is normally still erased. If not, rewrite the sector without it.
        sector = addr - addr % self.BLOCK_SIZE
        if sector == addr:
            return
        tail = mimxrt.flash_read(addr, sector + self.BLOCK_SIZE - addr)
        if tail == b'\xff' * len(tail):
            return
        head = mimxrt.flash_read(sector, addr - sector)
        mimxrt.flash_erase(sector)
        mimxrt.flash_write(sector, head)

    def commit_romfs(self):
        # Makes the appended records visible, then hides the files they replace. A power cut
        # before the header is rewritten leaves the old image. One after it leaves both copies,
        # and lookups still find the old one first.
        bounds = self.romfs_bounds()
        if bounds is None or self.ROMFS_ADDR + bounds[1] != self.write_addr:
            raise ValueError
        start, end = bounds
        new_end = end + self.expected_size
        added = self.romfs_files(end, new_end)
        if not added:
            raise ValueError
        names = [file[1] for file in added]
        stale = [file[0] for file in self.romfs_files(start, end) if file[1] in names]

        self.write_romfs_header(start, new_end - start)
        for offset in stale:
            self.retire(offset)

    def write_romfs_header(self, start, length):
        length_bytes = encode_vint(length)
        padding = start - len(ROMFS_MAGIC) - len(length_bytes)
        if padding < 0:
            raise ValueError
        sector = mimxrt.flash_read(self.ROMFS_ADDR, self.BLOCK_SIZE)
        # Leading 0x80 bytes are zeros in the length, so the payload stays at the same offset.
        sector = ROMFS_MAGIC + b'\x80' * padding + length_bytes + sector[start:]
        mimxrt.flash_erase(self.ROMFS_ADDR)
        mimxrt.flash_write(self.ROMFS_ADDR, sector)
        if mimxrt.flash_read(self.ROMFS_ADDR, self.BLOCK_SIZE) != sector:
            raise OSError('header')

    def retire(self, offset):
        # Turns a file record into padding, which lookups skip. Changing the kind from 5 to 1
        # only clears a bit, so no erase is needed. 0xFF bytes leave their neighbours unchanged.
        addr = self.ROMFS_ADDR + offset
        word = addr - addr % 4
        data = bytearray(b'\xff\xff\xff\xff')
        data[addr - word] = KIND_PADDING
        mimxrt.flash_write(word, data)
        if mimxrt.flash_read(addr, 1)[0] != KIND_PADDING:
            raise OSError('retire')
