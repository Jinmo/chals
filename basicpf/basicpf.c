#include <sys/prctl.h>
#include <linux/filter.h>
#include <linux/seccomp.h>
#include <signal.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define LOAD_SYSCALL_NR \
	BPF_STMT(BPF_LD+BPF_W+BPF_ABS, offsetof(struct seccomp_data, nr))

#define LO_ARG(idx) offsetof(struct seccomp_data, args[(idx)])

#define ARG(i) \
	BPF_STMT(BPF_LD+BPF_W+BPF_ABS, LO_ARG(i))

#define ARGX(i) \
	BPF_STMT(BPF_LDX+BPF_W+BPF_ABS, LO_ARG(i))

#define EQ(addr, value) \
	ARG(addr), \
	BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, value, 1, 0), \
	BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ERRNO)

#define JEQ(nr, jmp) \
	BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, nr, jmp, 0)

#define RETURN_IDX(i) \
	BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, i, 0, 1), \
	BPF_STMT(BPF_LDX+BPF_MEM, i)

#define SYSCALL_BOX 100001
#define SYSCALL_CHECK 100000

#define C1 0x67141053
#define C2 0x1f252d4f
#define C3 0x29674066
#define C4 0xc39330a

struct sock_filter filter[] = {
// entry
	LOAD_SYSCALL_NR,
	JEQ(SYSCALL_CHECK, 2),
	JEQ(SYSCALL_BOX, 14),
	BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ALLOW),
// entry for check
	EQ(0, C1),
	EQ(1, C2),
	EQ(2, C3),
	EQ(3, C4),
	BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ERRNO | 1),
// entry for box

#define K0 'rtlu'
#define K1 'es_a'
#define K2 'eruc'
#define K3 'yek_'

#define XOR(i) \
	BPF_STMT(BPF_MISCOP(BPF_TAX), 0), \
	ARG(i), \
	BPF_STMT(BPF_ALU+BPF_XOR+BPF_X, 0)

#define SBOX(x, y) \
	BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, x, 0, 1), \
	BPF_STMT(BPF_LD+BPF_IMM, 256+y)

#define BOX(i) \
	BPF_STMT(BPF_LD+BPF_IMM, 0), \
	ARG(i), \
	BPF_STMT(BPF_ALU+BPF_AND+BPF_K, 0xffffffff), \
	BPF_STMT(BPF_ALU+BPF_XOR+BPF_K, K##i), \
	STORE(i, 0), STORE(i, 1), STORE(i, 2), STORE(i, 3)

#define STORE(i, j) \
	BPF_STMT(BPF_ST, i * 4 + j), \
	BPF_STMT(BPF_ALU+BPF_RSH+BPF_K, 8)

	BOX(0),
	BOX(1),
	BOX(2),
	BOX(3),
	ARG(4),
	RETURN_IDX(0), RETURN_IDX(1), RETURN_IDX(2), RETURN_IDX(3),
	RETURN_IDX(4), RETURN_IDX(5), RETURN_IDX(6), RETURN_IDX(7),
	RETURN_IDX(8), RETURN_IDX(9), RETURN_IDX(10), RETURN_IDX(11),
	RETURN_IDX(12), RETURN_IDX(13), RETURN_IDX(14), RETURN_IDX(15),
	// sbox
	BPF_STMT(BPF_MISC|BPF_TXA, 0),
	BPF_STMT(BPF_ALU+BPF_AND+BPF_K, 0xff),
	SBOX(0, 108), SBOX(1, 96), SBOX(2, 52), SBOX(3, 32), SBOX(4, 63), SBOX(5, 49), SBOX(6, 95), SBOX(7, 36), SBOX(8, 57), SBOX(9, 69), SBOX(10, 107), SBOX(11, 59), SBOX(12, 124), SBOX(13, 86), SBOX(14, 70), SBOX(15, 28),
	SBOX(16, 101), SBOX(17, 109), SBOX(18, 89), SBOX(19, 98), SBOX(20, 33), SBOX(21, 78), SBOX(22, 121), SBOX(23, 71), SBOX(24, 122), SBOX(25, 10), SBOX(26, 44), SBOX(27, 61), SBOX(28, 91), SBOX(29, 105), SBOX(30, 46), SBOX(31, 83),
	SBOX(32, 25), SBOX(33, 76), SBOX(34, 51), SBOX(35, 1), SBOX(36, 66), SBOX(37, 120), SBOX(38, 74), SBOX(39, 116), SBOX(40, 0), SBOX(41, 42), SBOX(42, 110), SBOX(43, 20), SBOX(44, 27), SBOX(45, 72), SBOX(46, 15), SBOX(47, 45),
	SBOX(48, 19), SBOX(49, 94), SBOX(50, 62), SBOX(51, 34), SBOX(52, 6), SBOX(53, 24), SBOX(54, 37), SBOX(55, 68), SBOX(56, 7), SBOX(57, 39), SBOX(58, 103), SBOX(59, 73), SBOX(60, 55), SBOX(61, 90), SBOX(62, 123), SBOX(63, 113),
	SBOX(64, 82), SBOX(65, 112), SBOX(66, 99), SBOX(67, 23), SBOX(68, 106), SBOX(69, 17), SBOX(70, 102), SBOX(71, 119), SBOX(72, 88), SBOX(73, 100), SBOX(74, 127), SBOX(75, 4), SBOX(76, 64), SBOX(77, 111), SBOX(78, 97), SBOX(79, 41),
	SBOX(80, 43), SBOX(81, 47), SBOX(82, 85), SBOX(83, 58), SBOX(84, 31), SBOX(85, 11), SBOX(86, 67), SBOX(87, 48), SBOX(88, 35), SBOX(89, 60), SBOX(90, 54), SBOX(91, 21), SBOX(92, 16), SBOX(93, 38), SBOX(94, 117), SBOX(95, 30),
	SBOX(96, 53), SBOX(97, 2), SBOX(98, 18), SBOX(99, 14), SBOX(100, 50), SBOX(101, 22), SBOX(102, 75), SBOX(103, 29), SBOX(104, 115), SBOX(105, 5), SBOX(106, 126), SBOX(107, 118), SBOX(108, 93), SBOX(109, 92), SBOX(110, 40), SBOX(111, 114),
	SBOX(112, 87), SBOX(113, 8), SBOX(114, 12), SBOX(115, 84), SBOX(116, 77), SBOX(117, 81), SBOX(118, 13), SBOX(119, 26), SBOX(120, 125), SBOX(121, 65), SBOX(122, 9), SBOX(123, 104), SBOX(124, 56), SBOX(125, 3), SBOX(126, 80), SBOX(127, 79),
	SBOX(128, 128), SBOX(129, 129), SBOX(130, 130), SBOX(131, 131), SBOX(132, 132), SBOX(133, 133), SBOX(134, 134), SBOX(135, 135), SBOX(136, 136), SBOX(137, 137), SBOX(138, 138), SBOX(139, 139), SBOX(140, 140), SBOX(141, 141), SBOX(142, 142), SBOX(143, 143),
	SBOX(144, 144), SBOX(145, 145), SBOX(146, 146), SBOX(147, 147), SBOX(148, 148), SBOX(149, 149), SBOX(150, 150), SBOX(151, 151), SBOX(152, 152), SBOX(153, 153), SBOX(154, 154), SBOX(155, 155), SBOX(156, 156), SBOX(157, 157), SBOX(158, 158), SBOX(159, 159),
	SBOX(160, 160), SBOX(161, 161), SBOX(162, 162), SBOX(163, 163), SBOX(164, 164), SBOX(165, 165), SBOX(166, 166), SBOX(167, 167), SBOX(168, 168), SBOX(169, 169), SBOX(170, 170), SBOX(171, 171), SBOX(172, 172), SBOX(173, 173), SBOX(174, 174), SBOX(175, 175),
	SBOX(176, 176), SBOX(177, 177), SBOX(178, 178), SBOX(179, 179), SBOX(180, 180), SBOX(181, 181), SBOX(182, 182), SBOX(183, 183), SBOX(184, 184), SBOX(185, 185), SBOX(186, 186), SBOX(187, 187), SBOX(188, 188), SBOX(189, 189), SBOX(190, 190), SBOX(191, 191),
	SBOX(192, 192), SBOX(193, 193), SBOX(194, 194), SBOX(195, 195), SBOX(196, 196), SBOX(197, 197), SBOX(198, 198), SBOX(199, 199), SBOX(200, 200), SBOX(201, 201), SBOX(202, 202), SBOX(203, 203), SBOX(204, 204), SBOX(205, 205), SBOX(206, 206), SBOX(207, 207),
	SBOX(208, 208), SBOX(209, 209), SBOX(210, 210), SBOX(211, 211), SBOX(212, 212), SBOX(213, 213), SBOX(214, 214), SBOX(215, 215), SBOX(216, 216), SBOX(217, 217), SBOX(218, 218), SBOX(219, 219), SBOX(220, 220), SBOX(221, 221), SBOX(222, 222), SBOX(223, 223),
	SBOX(224, 224), SBOX(225, 225), SBOX(226, 226), SBOX(227, 227), SBOX(228, 228), SBOX(229, 229), SBOX(230, 230), SBOX(231, 231), SBOX(232, 232), SBOX(233, 233), SBOX(234, 234), SBOX(235, 235), SBOX(236, 236), SBOX(237, 237), SBOX(238, 238), SBOX(239, 239),
	SBOX(240, 240), SBOX(241, 241), SBOX(242, 242), SBOX(243, 243), SBOX(244, 244), SBOX(245, 245), SBOX(246, 246), SBOX(247, 247), SBOX(248, 248), SBOX(249, 249), SBOX(250, 250), SBOX(251, 251), SBOX(252, 252), SBOX(253, 253), SBOX(254, 254), SBOX(255, 255),
	BPF_STMT(BPF_ALU+BPF_AND+BPF_K, 0xff),
	BPF_STMT(BPF_ALU+BPF_OR+BPF_K, SECCOMP_RET_ERRNO),
	BPF_STMT(BPF_RET+BPF_A, 0)
};

struct sock_fprog prog = {
	.filter = filter,
	.len = sizeof(filter) / sizeof(filter[0])
};

__asm__(".text; .globl syscall\n syscall:\npush %ebp; mov %esp, %ebp; mov 8(%ebp), %eax; mov 12(%ebp), %ebx; mov 16(%ebp), %ecx; mov 20(%ebp), %edx; mov 24(%ebp), %esi; mov 28(%ebp), %edi; int $0x80; leave; ret");

int syscall(int nr, ...);

int _error(char *s) {
	puts(s);
	exit(1);
}

int _strlen(char *s) {
	int len = 0;
	while(*s++) len++;
	return len;
}

void _strcpy(char *dst, char *src) {
	while(*src) *dst++ = *src++;
}

int _read(int fd, char *buf, int len) {
	syscall(3, fd, buf, len);
}

void box(int *z) {
	int result[4] = {0, };
	for(int i = 0; i < 16; i++) {
		result[i >> 2] |= ((-syscall(SYSCALL_BOX, z[0], z[1], z[2], z[3], i)) & 0xff) << (i * 8);
	}
	memcpy(z, result, 16);
}

int check(int a, int b, int c, int d) {
	return -syscall(SYSCALL_CHECK, a, b, c, d) == 1;
}

void init() {
	prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) && _error("prctl failed");
	prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog) && _error("prctl failed");
}

int main() {
	int z[100] = {0, };
	char input[400] = {0, };
	_read(0, input, 400);
	input[400] = 0;
	if(input[_strlen(input) - 1] == '\n')
		input[_strlen(input) - 1] = 0;
	if(_strlen(input) != 16) _error("length must be 16");
	_strcpy((char *)z, input);
	init();
	for(int i = 0; i < 16; i++)
		box(z);

	if(!check(z[0], z[1], z[2], z[3]))
		_error("wrong.");
	printf("great job! flag is \"CYKOR{%s}\" without the quotes.\n", input);
}
