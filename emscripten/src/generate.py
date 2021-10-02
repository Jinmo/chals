import struct
from gmpy2 import invert

ciphertext = 'Adaptability is good for a reverser!'
key = 0xcafebeef

if len(ciphertext) % 4 != 0:
	print 'pad plz'
	exit()

print ', '.join([hex(struct.unpack("<L", ciphertext[i:i+4])[0]*key & 0xffffffff).rstrip('L') for i in range(0, len(ciphertext), 4)])