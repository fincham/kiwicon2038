#!/usr/bin/python

import sys

"""
Takes a .com file as input then adds the necessary headers and padding to make it in to an ISA option ROM.
The last byte will be modified so that the ROM's checksum is correct.
"""

# May need to be adjusted to build a larger ROM. 16 blocks = 8k ROM.
blocks = int(sys.argv[1])

if __name__ == "__main__":
    sys.stderr.write("%s: ROM will be %i blocks (%i bytes).\n" % (sys.argv[0], blocks, blocks * 512))
    header = chr(0x55) + chr(0xaa) + chr(blocks)
    com_bytes = sys.stdin.read()
    checksum = 0
    for byte in header + com_bytes:
        checksum = checksum + ord(byte)
    last_byte = chr((256 - checksum) % 256)
    written = sys.stdin.tell()
    sys.stdout.write(header)
    sys.stdout.write(com_bytes)
    padding = blocks * 512 - written - 3
    sys.stderr.write("%s: Wrote %i ROM bytes, padding with %i to make %i.\n" % (sys.argv[0], written + 3, padding, blocks * 512))
    sys.stdout.write(chr(0x00) * (padding - 1))
    sys.stdout.write(last_byte)
