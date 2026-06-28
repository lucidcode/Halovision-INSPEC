import gc
import ubinascii
import utime
import mimxrt


class ota_updater:
    BLOCK_SIZE = 4096
    # Firmware is staged 4MB into the 8MB ROMFS region.
    STAGING_ADDR = 0xC00000

    def __init__(self, comms):
        self.comms = comms
        self.expected_size = 0
        self.expected_crc = 0
        self.received_bytes = 0
        self.target_addr = 0
        self.write_addr = 0
        self.active = False
        self.last_erased_sector = -1

    def begin(self, addr_hex, size_str, crc_hex):
        self.target_addr = int(addr_hex, 16)
        self.expected_size = int(size_str)
        self.expected_crc = int(crc_hex, 16)
        self.received_bytes = 0
        self.active = True
        self.last_erased_sector = -1

        # ROMFS region: write directly to target address.
        # App region: stage in unused ROMFS space, copy later via C function.
        if self.target_addr >= 0x800000:
            self.write_addr = self.target_addr
        else:
            self.write_addr = self.STAGING_ADDR

        gc.collect()
        self.comms.send_data('ota:ready')

    def receive_chunk(self, data):
        if not self.active:
            return

        write_pos = self.write_addr + self.received_bytes

        # Erase the 4KB sector if this is the first write into it.
        sector = write_pos // self.BLOCK_SIZE
        if sector != self.last_erased_sector:
            mimxrt.flash_erase(sector * self.BLOCK_SIZE)
            self.last_erased_sector = sector

        mimxrt.flash_write(write_pos, data)
        self.received_bytes += len(data)

        # Report progress every 4KB.
        if self.received_bytes % self.BLOCK_SIZE < len(data):
            self.comms.send_data(f'ota:{self.received_bytes}')

        if self.received_bytes >= self.expected_size:
            self.active = False
            self.comms.ota_mode = False
            self.comms.send_data(f'ota:complete:{self.received_bytes}')

    def verify(self):
        if self.received_bytes != self.expected_size:
            self.comms.send_data(f'ota:error:size_mismatch:{self.received_bytes}')
            return False

        crc = 0
        remaining = self.expected_size
        addr = self.write_addr
        while remaining > 0:
            chunk_size = min(self.BLOCK_SIZE, remaining)
            chunk = mimxrt.flash_read(addr, chunk_size)
            crc = ubinascii.crc32(chunk, crc)
            addr += chunk_size
            remaining -= chunk_size
        crc = crc & 0xFFFFFFFF

        if crc == self.expected_crc:
            self.comms.send_data('ota:verified')
            return True
        else:
            self.comms.send_data(f'ota:crc_mismatch:{crc:08X}')
            return False

    def apply(self):
        if self.target_addr >= 0x800000:
            # ROMFS was written directly in place.
            self.comms.send_data('ota:applied')
        else:
            # Firmware staged in ROMFS. Copy to app region from RAM, then reset.
            self.comms.send_data('ota:applying')
            utime.sleep_ms(500)
            mimxrt.flash_copy_and_reset(self.STAGING_ADDR, self.target_addr, self.expected_size)
            # Does not return.

    def abort(self):
        self.active = False
        self.received_bytes = 0
        self.comms.ota_mode = False
        self.comms.send_data('ota:aborted')
