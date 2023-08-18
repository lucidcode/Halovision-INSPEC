from jinja2 import Environment, FileSystemLoader
import pathlib
import copy

svd = Environment(loader=FileSystemLoader(searchpath=["svd"]))

bcm2711 = svd.get_template("chips/bcm2711.svd.jinja")
bcm2835 = svd.get_template("chips/bcm2835.svd.jinja")
bcm2837 = svd.get_template("chips/bcm2837.svd.jinja")

broadcom = Environment(loader=FileSystemLoader(searchpath=["broadcom"]))
pins = broadcom.get_template("gen/pins.c.jinja")

chips = pathlib.Path("svd/chips")
gen = pathlib.Path("svd/gen")
bcm_gen = pathlib.Path("broadcom/gen")

bcm2711_altfunc = [[None] * 6 for i in range(58)]
bcm2837_altfunc = [[None] * 6 for i in range(54)]

def i2c(start_pin, altfunc, instance_number, bcm2837=False):
    bcm2711_altfunc[start_pin][altfunc] = f"SDA{instance_number}"
    bcm2711_altfunc[start_pin + 1][altfunc] = f"SCL{instance_number}"
    if bcm2837:
        bcm2837_altfunc[start_pin][altfunc] = f"SDA{instance_number}"
        bcm2837_altfunc[start_pin + 1][altfunc] = f"SCL{instance_number}"


def in_order(start_pin, altfunc, prefix, suffixes, bcm2837=False):
    for i, suffix in enumerate(suffixes):
        if suffix is None:
            continue
        bcm2711_altfunc[start_pin + i][altfunc] = prefix + str(suffix)
        if bcm2837:
            bcm2837_altfunc[start_pin + i][altfunc] = prefix + str(suffix)

# These are unique JTAG pins on the BCM2837. We do them first so the 2711 pin
# versions are overwritten.
in_order(4, 5, "ARM_", ("TDI", "TDO", "RTCK"), bcm2837=True)
in_order(12, 5, "ARM_", ("TMS", "TCK"), bcm2837=True)

i2c(0, 0, 0, bcm2837=True)
i2c(2, 0, 1, bcm2837=True)
i2c(0, 5, 6)
i2c(2, 5, 3)
i2c(4, 5, 3)
i2c(6, 5, 4)
i2c(8, 5, 4)
i2c(10, 5, 5)
i2c(12, 5, 5)
i2c(22, 5, 6)
i2c(28, 0, 0, bcm2837=True)
i2c(44, 1, 0, bcm2837=True)
i2c(44, 2, 1, bcm2837=True)

SPI_SUFFIXES = ("_CE0_N", "_MISO", "_MOSI", "_SCLK")
in_order(0, 3, "SPI3", SPI_SUFFIXES)
in_order(4, 3, "SPI4", SPI_SUFFIXES)
in_order(7, 0, "SPI0", ("_CE1_N",) + SPI_SUFFIXES, bcm2837=True)
in_order(12, 3, "SPI5", SPI_SUFFIXES)
in_order(18, 3, "SPI6", SPI_SUFFIXES)
in_order(16, 4, "SPI1", ("_CE2_N", "_CE1_N") + SPI_SUFFIXES, bcm2837=True)
for i, instance in enumerate(range(3, 7)):
    bcm2711_altfunc[24 + i][5] = f"SPI{instance}_CE1_N"
in_order(35, 0, "SPI0", ("_CE1_N",) + SPI_SUFFIXES, bcm2837=True)

# This SPI has a unique pin order. It is also a different instance on the 2711.
# So we set it twice, once to set both and once to correct 2711.
in_order(40, 0, "SPI2", ("_MISO", "_MOSI", "_SCLK", "_CE0_N", "_CE1_N", "_CE2_N"), bcm2837=True)
in_order(40, 0, "SPI0", ("_MISO", "_MOSI", "_SCLK", "_CE0_N", "_CE1_N", "_CE2_N"))

in_order(4, 2, "DPI_D", range(0, 24))
in_order(8, 1, "SD", range(18), bcm2837=True)
in_order(0, 1, "SA", range(5, -1, -1), bcm2837=True)
in_order(6, 1, "", ("SOE_N", "SWE_N"), bcm2837=True)
in_order(0, 2, "", ("PCLK", "DE", "LCD_VSYNC", "LCD_HSYNC"))
in_order(4, 0, "GPCLK", range(3), bcm2837=True)
in_order(8, 3, "BSCSL_", ("CE_N", "MISO", "MOSI", "SCLK"))
in_order(12, 0, "PWM0_", range(2), bcm2837=True)
in_order(18, 5, "PWM0_", range(2), bcm2837=True)
in_order(18, 0, "PCM_", ("CLK", "FS", "DIN", "DOUT"), bcm2837=True)
in_order(20, 5, "GPCLK", range(2), bcm2837=True)
in_order(22, 0, "SD0_", ("CLK", "CMD", "DAT0", "DAT1", "DAT2", "DAT3"))
in_order(22, 3, "SD1_", ("CLK", "CMD", "DAT0", "DAT1", "DAT2", "DAT3"), bcm2837=True)
in_order(22, 4, "ARM_", ("TRST", "RTCK", "TDO", "TCK", "TDI", "TMS"), bcm2837=True)
in_order(28, 1, "SA", range(5, -1, -1), bcm2837=True)
in_order(28, 4, "MII_A_", ("RX_ERR", "TX_ERR", "CRS", "COL"))
in_order(28, 5, "RGMII_", ("MDIO", "MDC"))
in_order(32, 4, "SD_CARD_", ("PRES", "WRPROT", "LED"))
in_order(34, 3, "SD1_", ("CLK", "CMD"))
in_order(36, 3, "SD1_DAT", range(8))
in_order(36, 1, "SD", range(8), bcm2837=True)
in_order(34, 1, "", ("SOE_N", "SWE_N"), bcm2837=True)
in_order(28, 2, "PCM_", ("CLK", "FS", "DIN", "DOUT"), bcm2837=True)
bcm2711_altfunc[32][0] = "GPCLK0"
bcm2711_altfunc[34][0] = "GPCLK0"
bcm2837_altfunc[32][0] = "GPCLK0"
bcm2837_altfunc[34][0] = "GPCLK0"
bcm2711_altfunc[34][5] = "RGMII_IRQ"
in_order(35, 4, "RGMII_", ("START_STOP", "RX_OK", "MDIO", "MDC", "IRQ"))
in_order(36, 5, "MII_A_", ("RX_ERR", "TX_ERR", "CRS", "COL"))
in_order(40, 0, "PWM0_", range(2), bcm2837=True)
# BCM2711 has a second PWM1 that takes the place of PWM0 in these two spots.
in_order(40, 0, "PWM1_", range(2), bcm2837=False)
in_order(42, 0, "GPCLK", range(1, 3), bcm2837=True)
bcm2711_altfunc[44][0] = "GPCLK1"
bcm2711_altfunc[45][0] = "PWM0_1"
bcm2837_altfunc[44][0] = "GPCLK1"
bcm2837_altfunc[45][0] = "PWM0_1"
in_order(44, 5, "SD_CARD_", ("VOLT", "PWR0"))
in_order(48, 3, "SD1_", ("CLK", "CMD"), bcm2837=True)
in_order(50, 3, "SD1_DAT", range(4), bcm2837=True)

def uart(start_pin, altfunc, instance_number, bcm2837=False):
    bcm2711_altfunc[start_pin][altfunc] = f"TXD{instance_number}"
    bcm2711_altfunc[start_pin + 1][altfunc] = f"RXD{instance_number}"
    bcm2711_altfunc[start_pin + 2][altfunc] = f"CTS{instance_number}"
    bcm2711_altfunc[start_pin + 3][altfunc] = f"RTS{instance_number}"
    if bcm2837:
        bcm2837_altfunc[start_pin][altfunc] = f"TXD{instance_number}"
        bcm2837_altfunc[start_pin + 1][altfunc] = f"RXD{instance_number}"
        bcm2837_altfunc[start_pin + 2][altfunc] = f"CTS{instance_number}"
        bcm2837_altfunc[start_pin + 3][altfunc] = f"RTS{instance_number}"


uart(0, 4, 2)
uart(4, 4, 3)
uart(8, 4, 4)
uart(12, 4, 5)
in_order(14, 0, "", ("TXD0", "RXD0"), bcm2837=True)
in_order(16, 3, "", ("CTS0", "RTS0"), bcm2837=True)
uart(14, 5, 1, bcm2837=True)
in_order(30, 3, "", ("CTS0", "RTS0", "TXD0", "RXD0"), bcm2837=True)
in_order(30, 5, "", ("CTS1", "RTS1", "TXD1", "RXD1"), bcm2837=True)

uart(36, 2, 0, bcm2837=True)
uart(40, 5, 1, bcm2837=True)

functions = {
    "I2C": ["SCL", "SDA"],
    "UART": ["TXD", "RXD", "CTS", "RTS"]
}

def convert_to_pin_info(altfunc):
    pin_info = []
    for pin_number, row in enumerate(altfunc):
        function_info = []
        for alt in range(6):
            altfun = row[alt]
            if not altfun:
                function_info.append(("NONE", 0, 0))
                continue
            done = False
            for f in functions:
                for pin in functions[f]:
                    if altfun.startswith(pin):
                        index = int(altfun[len(pin):])
                        function_info.append((f, index, pin))
                        done = True
                        break
                if done:
                    break
            if done:
                continue
            if altfun.startswith("SPI"):
                f = "SPI"
                index = int(altfun[3:4])
                pin = altfun.split("_")[1]
                function_info.append((f, index, pin))
                continue
            elif altfun.startswith("PWM"):
                f = "PWM"
                index = int(altfun[3:4])
                pin = int(altfun[-1:]) + 1
                function_info.append((f, index, "CHANNEL" + str(pin)))
                continue

            function_info.append(("NONE", 0, ""))
        pin_info.append(function_info)
    return pin_info

videocore_interrupts = [
    "Timer 0",
    "Timer 1",
    "Timer 2",
    "Timer 3",
    "H264 0",
    "H264 1",
    "H264 2",
    "JPEG",
    "ISP",
    "USB",
    "V3D",
    "Transposer",
    "Multicore Sync 0",
    "Multicore Sync 1",
    "Multicore Sync 2",
    "Multicore Sync 3",
    "DMA 0",
    "DMA 1",
    "DMA 2",
    "DMA 3",
    "DMA 4",
    "DMA 5",
    "DMA 6",
    ("DMA_7_8", "OR of DMA 7 and 8"),
    ("DMA_9_10", "OR of DMA 9 and 10"),
    "DMA 11",
    "DMA 12",
    "DMA 13",
    "DMA 14",
    ("AUX", "OR of UART1, SPI1 and SPI2"),
    "ARM",
    "DMA 15",
    "HDMI CEC",
    "HVS",
    "RPIVID",
    "SDC",
    "DSI 0",
    "Pixel Valve 2",
    "Camera 0",
    "Camera 1",
    "HDMI 0",
    "HDMI 1",
    "Pixel Valve 3",
    "SPI/BSC Slave",
    "DSI 1",
    "Pixel Valve 0",
    ("PIXEL_VALVE_1_2", "OR of Pixel Valve 1 and 2"),
    "CPR",
    "SMI",
    "GPIO 0",
    "GPIO 1",
    "GPIO 2",
    "GPIO 3",
    ("I2C", "OR of all I2C"),
    ("SPI", "OR of all SPI"),
    "PCM/I2S",
    "SDHOST",
    ("UART", "OR of all PL011 UARTs"),
    ("ETH_PCIE", "OR of all ETH_PCIe L2"),
    "VEC",
    "CPG",
    "RNG",
    ("EMMC", "OR of EMMC and EMMC2"),
    "ETH_PCIe secure"
]

armc_interrupts = [
    ("TIMER", "ARMC Timer"),
    ("MAILBOX", "Mailbox"),
    ("DOORBELL0", "Doorbell 0"),
    ("DOORBELL1", "Doorbell 1"),
    ("VPU0_HALTED", "VPU0 halted"),
    ("VPU1_HALTED", "VPU1 halted"),
    ("ARM_ADDRESS_ERROR", "ARM address error"),
    ("ARM_AXI_ERROR", "ARM AXI error"),
]

for i in range(8):
    armc_interrupts.append((f"SWI{i}", f"Software interrupt {i}"))

bcm2711_interrupt_names = {}
bcm2837_interrupt_names = {}
for i, irq in enumerate(armc_interrupts):
    bcm2711_interrupt_names[64 + i] = irq
    bcm2837_interrupt_names[64 + i] = irq
for i, irq in enumerate(videocore_interrupts):
    if isinstance(irq, str):
        name = irq.replace(" ", "_").replace("/", "_").upper()
        irq = (name, irq)
    bcm2711_interrupt_names[96 + i] = irq
    bcm2837_interrupt_names[i] = irq

# Map the bits in the "basic" registers to their interrupt number
legacy_basic_irqs = [(i, 64 + i) for i in range(8)] + list(enumerate((7, 9, 10, 18, 19, 53, 54, 55, 56, 57, 62), start=10))

(gen / "bcm2711_lpa.svd").write_text(
    bcm2711.render(
        peripheral_base=0xFE000000,
        arm_local_base=0xFF800000,
        altfunc=bcm2711_altfunc,
        name="bcm2711_lpa",
        interrupt_names=bcm2711_interrupt_names,
        basic_irq=legacy_basic_irqs
    )
)

(gen / "bcm2837_lpa.svd").write_text(
    bcm2837.render(
        peripheral_base=0x3F000000,
        arm_local_base=0x3F000000,
        altfunc=bcm2837_altfunc,
        name="bcm2837_lpa",
        interrupt_names=bcm2837_interrupt_names,
        basic_irq=legacy_basic_irqs
    )
)

(gen / "bcm2835_lpa.svd").write_text(
    bcm2835.render(
        peripheral_base=0x20000000,
        arm_local_base=0x20000000,
        altfunc=bcm2837_altfunc,
        name="bcm2835_lpa",
        interrupt_names=bcm2837_interrupt_names,
        basic_irq=legacy_basic_irqs
    )
)

(bcm_gen / "pins.c").write_text(
    pins.render(
        bcm2711_altfunc=convert_to_pin_info(bcm2711_altfunc),
        bcm2837_altfunc=convert_to_pin_info(bcm2837_altfunc)
    )
)
