#include <stdio.h>
#include <string.h>

unsigned int key = 0xcafebeef;
unsigned int ciphertext[] = {0xf5d2d6af, 0xa045134c, 0x9b1493d4, 0x8184e4e0, 0x46e97329, 0x178c17e0, 0x32106ce0, 0x44ce7e4b, 0x8732105d};
unsigned int buf[64];

int main() {
	setvbuf(stdin, 0, 2, 0);
	puts("Do you know kimchi, psy? (y/n)");
	if(getchar() != 'y') {
		puts("Then learn about it. Thanks, bye.");
		return 0;
	}
	puts("Then do you know Emscripten?");
	puts("You can learn it in http://kripken.github.io/emscripten-site/.");
	puts("Anyway, you should know about my SUPER-SECRET PASSWORD if you know about Emscripten!");
	puts("What is it?");
	while(getchar() != 10);
	fgets((char *)buf, sizeof(buf), stdin);
	for(int i = 0; i < sizeof(ciphertext) / sizeof(ciphertext[0]); i++) {
		buf[i] *= key;
	}
	if(memcmp(buf, ciphertext, sizeof(ciphertext)) == 0) {
		puts("Congratulations! The password is the flag.");
	} else {
		puts("Incorrect password!");
	}
	return 0;
}