# This file is part of the blueprint package.
# Copyright (c) 2024 Arduino SA
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
import opta
import logging


if __name__ == "__main__":
    logging.basicConfig(
        datefmt="%H:%M:%S",
        format="%(asctime)s.%(msecs)03d %(message)s",
        level=logging.INFO  # Switch to DEBUG to see raw commands
    )

    opta = opta.Opta(bus_id=3)

    # enum_devices initializes the bus, resets all expansions, and returns a list of
    # detected expansions. NOTE: keep a reference to the list of expansion for later
    # use, as every time this function is called it restarts the enumeration process.
    for e in opta.enum_devices():
        print("")
        logging.info(f"Expansion type: {e.type} address: 0x{e.addr:02X} name: {e.name}")

        # Read firmware version.
        major, minor, patch = e.firmware_version()
        logging.info(f"Firmware Version Major: {major} Minor: {minor} Patch: {patch}")

        # Read product ID.
        pid = e.product_id()
        logging.info("Product ID bytes: " + ", ".join(["0x%02X" % a for a in pid[0:8]]))

        if e.type == "digital":
            # Write digital pins. If the default state and timeout (in milliseconds) are
            # also specified, the pins will revert to the specified default state after
            # the timeout expires.
            e.digital(pins=0b10101010, default=0b01010101, timeout=3000)

            # If no args are specified, digital() returns all digital pins.
            pins = e.digital()
            logging.info(f"Digital pins: 0b{pins:08b}")

            # Read analog pins.
            logging.info("Analog pin  [0   ]: %d" % e.analog(channel=0))
            logging.info("Analog pins [0..7]: " + str(e.analog()))

        if e.type == "analog":
            # Configure LEDs on Opta Analog
            e.digital(pins=0b10011001)

            # Configure analog channels. Note almost all of the possible args are used here
            # for demonstration purposes, however only a few are actually required (such as
            # the channel type and mode). Most of the other args have reasonable defaults.

            # Configure channel (0) as PWM.
            e.analog(channel=0, channel_type="pwm", period=1000, duty=50, default_period=500)

            # Configure channel (2) as DAC.
            e.analog(channel=2, channel_type="dac", channel_mode="voltage", value=7540)

            # Configure channel (3) as ADC.
            e.analog(channel=3, channel_type="adc", channel_mode="voltage", pulldown=True,
                     rejection=False, diagnostic=False, average=0, secondary=False)

            # Configure channel (4) as RTD.
            e.analog(channel=4, channel_type="rtd", use_3_wire=False, current_ma=1.2, update_time=0)

            # Configure channel (5) as digital input.
            e.analog(channel=5, channel_type="din", comparator=True, inverted=False, filtered=True,
                     scale=False, sink=1, threshold=9, debounce_mode="simple", debounce_time=24)

            # Read ADC channel (3).
            # This should print 65535 if channels 2 and 3 are connected.
            logging.info("ADC channel  [3   ]: %d" % e.analog(channel=3))

            # Read all analog channels.
            logging.info("Analog channels [0..7]: " + str(e.analog()))

            # Read RTD channel (4).
            logging.info("RTD channel  [4   ]: %.1f ohm" % e.analog(channel=4))

            # Read DIN channel (5).
            logging.info("DIN channel  [5   ]: %d" % e.analog(channel=5))

            # Read all analog channels.
            logging.info("Analog channels [0..7]: " + str(e.analog()))

            # Re-configure channel (2) as DAC.
            e.analog(channel=2, channel_type="dac", channel_mode="voltage", value=3770)
            logging.info("ADC channel  [3   ]: %d" % e.analog(channel=3))
