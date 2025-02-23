import logging
import se05x
from micropython import const

TC_R = "\033[91m"
TC_G = "\033[92m"
TC_B = "\033[94m"
TC_RST = "\033[0m"
KEY_ID = const(0x10)

if __name__ == "__main__":
    logging.basicConfig(
        datefmt="%H:%M:%S",
        format="%(asctime)s.%(msecs)03d %(message)s",
        level=logging.INFO
    )

    # Create and initialize SE05x device.
    se = se05x.SE05X()

    # Print applet version.
    major, minor, patch = se.version()
    logging.info(f"{TC_G}Applet Version: {major}.{minor}.{patch}{TC_RST}")

    # Delete key object if it exists.
    if se.exists(KEY_ID):
        se.delete(KEY_ID)

    # Generate EC key pair.
    se.write(KEY_ID, se05x.EC_KEY, curve=se05x.EC_CURVE_NIST_P256)
    ec_pub_key = se.read(KEY_ID)
    logging.info(f"{TC_B}Public Key: " + "".join("%02X" % b for b in ec_pub_key) + TC_RST)

    # Sign and verify hash.
    data = bytes(range(32))
    sign = se.sign(KEY_ID, data)
    logging.info(f"{TC_B}EC signature: " + "".join("%02X" % b for b in sign) + TC_RST)
    if not se.verify(KEY_ID, data, sign):
        logging.info(f"{TC_R}EC signature verified failed!{TC_RST}")
    logging.info(f"{TC_G}EC signature verified successfully!{TC_RST}")
