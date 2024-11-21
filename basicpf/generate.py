import struct
import random

sbox = None
random.seed(0)
if sbox is None:
	while True:
		sbox = random.sample(range(128), 128)
		q = sum(int(x == y) for x, y in zip(range(128), sbox))
		if q > 0:
			continue
		break
	print sbox
	sbox = sbox[:128] + range(128, 256)

if False:
	for i in range(256):
		print 'SBOX(%d, %d),' % (i, sbox[i]),
		if i % 16 == 15:
			print

key = 'ultra_secure_key'
key = struct.unpack("<4L", key)
mask = 2 ** 32 - 1

print '\n'.join(map(hex, key))

table = [
[3], [2], [1], [0]
]

def box3(x):
	a, b, c, d = bytearray(struct.pack("<L", x))
	a, b, c, d = map(lambda y: sbox[y], [a, b, c, d])
	return struct.unpack("<L", str(bytearray([a, b, c, d])))[0]

def box2(args):
	r, z = args
	x = 0
	for index in [r]:
		x ^= z[index]
	x = (x ^ key[r]) & mask
	if False:
		print hex(x),
	x = box3(x)
	return x

def box(a, b, c, d):
	z = a, b, c, d
	a, b, c, d = map(box2, zip(range(4), [z, z, z, z]))
	return a, b, c, d

def mix(text):
	return text
	mixed = []
	for i in range(4):
		result = 0
		for j in table[i]:
			result ^= text[j]
		mixed.append(result)
	return mixed

text = 'youarebpfmaster!'
for i in range(16):
	text = struct.pack("<4L", *box(*mix(struct.unpack("<4L", text))))

	print ', '.join(map(hex, struct.unpack("<4L", text)))

for i in range(4):
	print '#define C%d 0x%x' % (i + 1, struct.unpack("<4L", text)[i])