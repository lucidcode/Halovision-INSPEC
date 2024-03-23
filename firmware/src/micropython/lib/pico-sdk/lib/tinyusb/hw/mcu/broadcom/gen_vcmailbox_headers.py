from jinja2 import Environment, FileSystemLoader
import pathlib
import textwrap

env = Environment(loader=FileSystemLoader(searchpath=["."]), trim_blocks=True)

p = pathlib.Path("firmware.wiki/Mailbox-property-interface.md")

started = False
name = None
properties = []
current_property = None
for line in p.read_text().split("\n"):
    if line.startswith("####"):
        name = line.split(" ", maxsplit=1)[1]
        description = []
        current_property = {"name": name, "description": description}
        properties.append(current_property)
        started = True
        continue
    if not started:
        continue

    if line.strip().startswith("*"):
        s = line.split(maxsplit=2)
        s[1] = s[1].strip(":")
        if line[1] == "*":
            if s[1] == "Tag":
                current_property["tag"] = int(s[2], 16)
            elif s[1] == "Request":
                current_struct = {}
                current_property["request"] = current_struct
            elif s[1] == "Response":
                current_struct = {}
                current_property["response"] = current_struct
        elif line[3] == "*":
            if s[1] == "Length":
                current_struct["length"] = s[2]
            elif s[1] == "Value":
                current_struct["fields"] = []
            elif s[1] == "State":
                current_struct = None
        elif line[5] == "*":
            if s[1] == "(repeated)" or current_struct is None:
                description.append(line)
                continue
            current_struct["fields"].append((s[1], s[2]))
        else:
            description.append(line)
    elif line.strip() and not line.startswith("#"):
        description.append(line)

to_c_types = {
    "u8": "uint8_t",
    "u32": "uint32_t",
    "u64": "uint64_t",
    # Oddball ones
    "Bits": "uint16_t",
    "128": "uint8_t",
}
OVERRIDE_NAME = {
    "parent clock id (0 for a root clock)": "parent_clock_id",
    "0-15: DMA channels 0-15 (0=do not use, 1=usable)": "usable_dma_channels",
    "16-31: reserved for future use": "reserved",
    "rate (in Hz)": "rate_in_hz",
    "value (offset from 1.2V in units of 0.025V)": "value",
    "0 is successful": "error_code",
    "bytes: EDID block": "edid_block",
    "state (as above)": "state",
    "RGBA palette values (index 0 to 255)": "rgba_palette_values",
    "offset: first palette index to set (0-255)": "offset",
    "length: number of palette entries to set (1-256)": "length",
    "RGBA palette values (offset to offset+length-1)": "rgba_palette_values",
    "0=valid, 1=invalid": "valid",
    "(unused)": "unused",
    "enable (1=visible, 0=invisible)": "enable",
}
ARRAY_SIZES = {
    "mac_address": 6,
    "ascii_command_line_string": 128,
    "edid_block": 128,
    "rgba_palette_values": 256
}
for p in properties:
    lower = (
        p["name"]
        .lower()
        .replace(" ", "_")
        .replace("(", "")
        .replace(")", "")
        .replace("/", "_")
    )
    p["lower"] = lower
    wrapped_description = []
    for line in p["description"]:
        wrapped_description.extend(textwrap.wrap(line, width=77))
        wrapped_description.append("")
    p["description"] = wrapped_description
    # print(p)
    for struct in ("request", "response"):
        if "fields" not in p[struct]:
            continue
        for i, field in enumerate(p[struct]["fields"]):
            # print(field)
            array_size = ""
            t = field[0]
            array = t.endswith("...")
            t = t.strip(".")
            if t == "128":
                array = True
            t = to_c_types[t]
            name = field[1]
            comment = field[1]
            if name in OVERRIDE_NAME:
                name = OVERRIDE_NAME[name]
            elif "in" in name.split():
                name = name.split("in")[0].strip()
            name = name.lower().replace(" ", "_")
            if name in ARRAY_SIZES:
                array_size = ARRAY_SIZES[name]
            p[struct]["fields"][i] = (t, name, array, array_size, comment)

power_device_ids = [
    "SD_CARD",
    "UART0",
    "UART1",
    "USB_HCD",
    "I2C0",
    "I2C1",
    "I2C2",
    "SPI",
    "CCP2TX",
    "UNKNOWN_RPI4_1",
    "UNKNOWN_RPI4_2",
]

clock_ids = [
    "RESERVED",
    "EMMC",
    "UART",
    "ARM",
    "CORE",
    "V3D",
    "H264",
    "ISP",
    "SDRAM",
    "PIXEL",
    "PWM",
    "HEVC",
    "EMMC2",
    "M2MC",
    "PIXEL_BVB",
]

mem_flags = [
    (1 << 0, "DISCARDABLE", "Can be resized to 0 at any time. Use for cached data"),
    (0 << 2, "NORMAL", "Normal allocating alias. Don't use from ARM"),
    (1 << 2, "DIRECT", "0xC alias uncached"),
    (2 << 2, "COHERENT", "0x8 alias. Non-callocating in L2 but coherent"),
    (3 << 2, "L1_NONALLOCATING", "Allocating in L2"),
    (1 << 4, "ZERO", "initialise buffer to all zeros"),
    (1 << 5, "NO_INIT", "Don't initialise (default is to initialise to all ones)"),
    (1 << 6, "HINT_PERMALOCK", "Likely to be locked for long periods of time."),
]

voltage_ids = ["RESERVED", "CORE", "SDRAM_C", "SDRAM_P", "SDRAM_I"]

channels = [
    "POWER",
    "FRAMEBUFFER",
    "UART",
    "VCHIQ",
    "LEDS",
    "BUTTONS",
    "TOUCH_SCREEN",
    "COUNT",
    "PROPERTY_ARM_TO_VC",
    "PROPERTY_VC_TO_ARM",
]

header = env.get_template("broadcom/gen/vcmailbox.h.jinja")

pathlib.Path("broadcom/gen/vcmailbox.h").write_text(
    header.render(
        properties=properties,
        power_device_ids=power_device_ids,
        clock_ids=clock_ids,
        voltage_ids=voltage_ids,
        mem_flags=mem_flags,
        channels=channels,
    )
)
