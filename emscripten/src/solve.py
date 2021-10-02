from gmpy2 import invert
import struct

s = 0xf5d2d6af, 0xa045134c, 0x9b1493d4, 0x8184e4e0, 0x46e97329, 0x178c17e0, 0x32106ce0, 0x44ce7e4b, 0x8732105d
key = 0xcafebeef

invkey = invert(key, 2**32)

print ''.join(struct.pack("<L", (c * invkey) & 0xffffffff) for c in s)