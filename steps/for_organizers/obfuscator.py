import lief
import mmap
import random
import struct
from capstone import *

path = 'steps/x64/Release/steps.exe'
mask = 2 ** 64 - 1

pe = lief.parse(path)
main = next(func for func in pe.exported_functions if func.name == 'main')
offset = pe.rva_to_offset(main.address)

f = open(path, 'rb+')
f.seek(offset)
code = f.read()

cs = Cs(CS_ARCH_X86, CS_MODE_64)
mapping = mmap.mmap(f.fileno(), f.tell())

disass = list(cs.disasm(code, 0))
disass = disass[:next(index + 1 for index, disas in enumerate(disass) if disas.mnemonic == 'int3')]

k1, k2 = [random.getrandbits(63) * 2 + 1 for i in range(2)]
k1, k2 = 17985425403911846645, 211525760460895571

for disas in disass[::-1]:
	start = disas.address + offset
	end = start + disas.size
	payload = mapping[start:start + 16]
	assert len(payload) == 16
	print(disas.address, payload.hex())

	a, b = struct.unpack("<QQ", payload)
	payload = struct.pack("<QQ", (a * k1) & mask, (b * k2) & mask)

	mapping[start:end] = payload[:disas.size]

print(k1, k2)
print(pow(k1, -1, mask + 1), pow(k2, -1, mask + 1))

f.flush()
f.close()