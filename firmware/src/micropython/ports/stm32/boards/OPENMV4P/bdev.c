#include "qspi.h"
#include "storage.h"

extern char _ffs_cache;

STATIC const mp_soft_qspi_obj_t qspi_bus = {
    .cs  = MICROPY_HW_QSPIFLASH_CS,
    .clk = MICROPY_HW_QSPIFLASH_SCK,
    .io0 = MICROPY_HW_QSPIFLASH_IO0,
    .io1 = MICROPY_HW_QSPIFLASH_IO1,
    .io2 = MICROPY_HW_QSPIFLASH_IO2,
    .io3 = MICROPY_HW_QSPIFLASH_IO3,
};

const mp_spiflash_config_t spiflash_config = {
    .bus_kind = MP_SPIFLASH_BUS_QSPI,
    .bus.u_qspi.data = (void*)&qspi_bus,
    .bus.u_qspi.proto = &qspi_proto,
    //NOTE: The FFS cache is not used when QSPI is enabled.
    .cache = (mp_spiflash_cache_t *) &_ffs_cache,
};

spi_bdev_t spi_bdev;
