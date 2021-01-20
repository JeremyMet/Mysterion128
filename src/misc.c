#include <stdint.h>
#include <stdio.h>

void print_bin(uint32_t i_int) {
	int i, mask;
	for(i=0;i<32;i++) {
		mask = 1 << (31-i) ;
		if (!(i&0b111)) {printf(" ");}
		printf("%i" ,(i_int&mask)?1:0);
	}
	printf("\n");
}
