# This file is part of the msgpack-rpc module.
# Any copyright is dedicated to the Public Domain.
# https://creativecommons.org/publicdomain/zero/1.0/

import logging
import msgpackrpc
import hashlib
import random
import binascii
from time import ticks_ms
from time import ticks_diff


hash_in = 0
hash_out = 0
DATA_BUF_SIZE = 256


def md5hash(buf):
    global hash_out
    hash_out += 1
    return hashlib.md5(buf).digest()


def randbytes(size):
    return bytes(random.getrandbits(8) for x in range(size))


if __name__ == "__main__":
    # Configure the logger.
    # All message equal to or higher than the logger level are printed.
    logging.basicConfig(
        datefmt="%H:%M:%S",
        format="%(asctime)s.%(msecs)03d %(message)s",
        level=logging.INFO,
    )

    # Create an RPC object
    rpc = msgpackrpc.MsgPackRPC()
    # Register objects or functions to be called by the remote processor.
    rpc.bind(md5hash)
    # Start the remote processor and wait for it to be ready to communicate.
    rpc.start(firmware=0x08180000, timeout=3000, num_channels=2)

    # Set data buffer size
    rpc.call("set_buffer_size", DATA_BUF_SIZE)

    data = randbytes(DATA_BUF_SIZE)
    ticks_start = ticks_ms()
    while True:
        f = rpc.call_async("md5hash", data)
        # While waiting for the remote processor we can generate the checksum
        # of the data that was just sent, and generate new random data block.
        digest = hashlib.md5(data).digest()
        data = randbytes(DATA_BUF_SIZE)

        # Read back the checksum of the first block
        digest_ret = f.join()
        if digest != digest_ret:
            raise Exception(f"MD5 checksum doesn't match {binascii.hexlify(digest)} {binascii.hexlify(digest_ret)}")
        khs = (hash_in + hash_out) / ticks_diff(ticks_ms(), ticks_start)
        logging.info(f"hashes generated: {hash_out} hashes received: {hash_in} size: {DATA_BUF_SIZE} bytes KH/S: {khs}")
        hash_in += 1
