#define _WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <utility>
#include <Windows.h>

uint64_t F(uint64_t X, uint64_t K) {
	return (((X * 7) ^ K ^ 0x00001337) >> 1) + (X - K ^ 0x13370000);
}

// Export the symbol for a better visibility
__declspec(dllexport) int main() {
	char buf[0x100] = { 0 };
	char ciphertext[] = "\xba\x91\x12\xe5\x01\x87\xf9\x51\xfa\x4b\xf8\x7d\xc5\xf1\x8d\x39\x5e\xbb\x23\xe3\x2e\x4f\xe9\xe3\x3c\x3c\x5e\x30\xdc\xc4\xf3\xd6";
	printf("Password: ");

	fgets(buf, sizeof(buf), stdin);
	char* delim = strchr(buf, '\n');
	if (delim)
		*delim = '\0';

	size_t size = strlen(buf);
	size = size + 0xf & ~0xf;

	// Feistel cipher with key K ~ K + 15
	for (size_t offset = 0; offset < size; offset += 16) {
		size_t K = 0x13371337;
		uint64_t* block = (uint64_t*)&buf[offset];
		for (int i = 0; i < 16; i++) {
			uint64_t& L = block[0];
			uint64_t& R = block[1];

			uint64_t Ln = R;
			uint64_t Rn = L ^ F(R, K);
			L = Ln;
			R = Rn;
			K += 1;
		}
		std::swap(block[0], block[1]);
	}

#define CIPHERTEXT_SIZE sizeof(ciphertext) - 1

	if (size == CIPHERTEXT_SIZE && !memcmp(buf, ciphertext, CIPHERTEXT_SIZE)) {
		puts("Correct!");
	}
	else {
		puts("Wrong!");
	}
	getchar();

	return 0;
}

uint8_t backup, *original;

inline bool is_single_step(_EXCEPTION_POINTERS* info) {
	return info->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT || info->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP;
}

inline void backup_and_add_breakpoint(uint8_t* code) {
	original = code;
	backup = *code;

	// Replace the instruction with int3 (0xcc)
	*code = 0xcc;
}

LONG filter(_EXCEPTION_POINTERS* info) {
	if (!is_single_step(info)) {
		return EXCEPTION_CONTINUE_SEARCH;
	}

	uint64_t* code = (uint64_t*)info->ContextRecord->Rip;
	uint64_t decrypt[] = { 16414774308783366493uLL, 466790644520867547uLL };
	uint64_t encrypt[] = { 17985425403911846645uLL, 211525760460895571uLL };

	// Remove int3 breakpoint
	if (original) {
		*original = backup;
		original = nullptr;
	}

	// Encrypt the previously decrypted instruction
	static uint64_t* prev;
	if (prev) {
		prev[0] *= encrypt[0];
		prev[1] *= encrypt[1];
	}
	prev = code;

	// Decrypt one instruction that we will execute
	code[0] *= decrypt[0];
	code[1] *= decrypt[1];

	int offset = 0;
	switch (code[0] & 0xFF) {
	case 0xE8:
		offset = 5;
		break;
	case 0xFF:
		offset = 6;
		break;
	case 0xC3:
		offset = 1;
		break;
	}

	if (!offset) {
		// Set single step bit if the instruction is not call near/call indirect/return near
		info->ContextRecord->EFlags |= (1 << 8);
	}
	else {
		// Set a breakpoint right after the instruction
		backup_and_add_breakpoint((uint8_t*)code + offset);
	}

	return EXCEPTION_CONTINUE_EXECUTION;
}

class A {
public:
	A() {
		// Set a breakpoint on main, and add VEH handler for deobfuscation
		size_t startAddress;
		DWORD flOldProtect;

		startAddress = (size_t)main & ~0xfffuLL;
		VirtualProtect((LPVOID)startAddress, 0x1000, 0x40, &flOldProtect);

		backup_and_add_breakpoint((uint8_t*)main);

		AddVectoredExceptionHandler(0, filter);
	}
};

A a;