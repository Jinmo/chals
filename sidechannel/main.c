#include <stdint.h>
#include "object.h"

#define MAX_REFERENCES 65536

int reference_exists();

void safe_free(void *p) {
	if(!reference_exists(p))
		free(p);
}

uint64_t getint() {
	char buf[8];
	int offset = 0;
	while(offset < 8) {
		buf[offset++] = getchar();
		if(buf[offset - 1] == -1)
			exit(-1);
	}
	return *(uint64_t *)buf;
}

#define alloca_probe(length) \
	(length > kMaxAllocationLength \
	? exit(1) \
	: ( \
		(allocated += length) > kMaxAllocationLength) \
		? exit(2) \
		: alloca(length) \
	)

void getobj() {
	int type = getint();
	size_t length;
	switch(type) {
		case 0: {
			char *buf; 
			length = getint();
			if(length > kMaxStringLength)
				exit(-2);
			buf = alloc(STRING, length);
			for(int i = 0; i < length; i++) {
				buf[i] = getchar();
				if(buf[i] == -1) exit(-3);
			}
			break;
		}
		case 1: {
			uint64_t *buf;
			length = getint();
			if(length > kMaxListLength)
				exit(-4);
			buf = alloc(LIST, length * sizeof(*buf));
			for(int i = 0; i < length; i++) {
				buf[i] = getref();
			}
			break;
		} \
		case 2: {
			dst = getint() * 2 + 1;
		}
	}
	return dst;
}

#define getref() (tmp = getint(), tmp > r ? exit(3) : tmp)

int main() {
	uint64_t a, b, c, r = 0, tmp;
	int allocated = 0;
	void *refs[MAX_REFERENCES] = {0, };
	while(1) {
		c = getchar();
		if(c == -1) break;
		switch(c) {
			case '+': // a += b
			a = getint();
			b = getint();
			case 'd': // define
			if(r == MAX_REFERENCES - 1) return -1;
			refs[r++] = getobj();
			case 'x': // delete
			a = getref();
			safe_free(refs[a]);
			case 'p': // print
			a = getref();
			print(refs[a]);
		}
	}
	return 0;
}