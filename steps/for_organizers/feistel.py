import struct

mask = 2 ** 64 - 1

def F(X, K):
    return (((X * 7 & mask) ^ K ^ 0x00001337) >> 1) + ((X - K & mask) ^ 0x13370000) & mask

plaintext = b'step_by_step_obfuscation_fun'

payload = plaintext + b'\x00' * ((16 - len(plaintext) % 16) % 16)
payload = bytearray(payload)
size = len(payload)
for offset in range(0, size, 16):
    K = 0x13371337
    block = struct.unpack_from("<QQ", payload, offset)
    for i in range(16):
        L, R = block
        Ln = R;
        Rn = L ^ F(R, K);
        block = Ln, Rn
        K += 1
    block = block[1], block[0]
    struct.pack_into("<QQ", payload, offset, *block)

print(payload, hex(K))

for offset in range(size - 16, -16, -16):
    K = 0x13371337 + 16
    block = struct.unpack_from("<QQ", payload, offset)
    block = block[1], block[0]
    for i in range(16):
        K -= 1
        L, R = block
        Rn = L;
        Ln = R ^ F(L, K);
        block = Ln, Rn
    struct.pack_into("<QQ", payload, offset, *block)

print(payload)