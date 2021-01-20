////////////////////////
// Mysterion-128      //
// Auteur : JME       //
// Date : 7/12/2020   //
////////////////////////


#include <x86intrin.h> // Pour estimer le nb de cycles.
#include <stdio.h>
#include <time.h>
#include "mysterion_ext.h"
#define NB_128_WORDS_FOR_BENCHMARKING 10000

#define init_cpucycles()
#define cpucycles(cycles) cycles = __rdtsc()

void main() {

	m_state cipher, plain, key ;
	plain.state[0] = rand()&0xFFFFFFFF; key.state[0] = rand()&0xFFFFFFFF ;
	plain.state[1] = rand()&0xFFFFFFFF; key.state[1] = rand()&0xFFFFFFFF ;
	plain.state[2] = rand()&0xFFFFFFFF; key.state[2] = rand()&0xFFFFFFFF ;
	plain.state[3] = rand()&0xFFFFFFFF; key.state[3] = rand()&0xFFFFFFFF ;
	//
 /* plain.state[0] = 0xFF000000; key.state[0] = 0x00000000 ;
	plain.state[1] = 0x00000000; key.state[1] = 0x00000000 ;
	plain.state[2] = 0x00000000; key.state[2] = 0x00000000 ;
	plain.state[3] = 0x00000001; key.state[3] = 0x00000000 ;*/


	int i;
	cipher = encrypt_state(plain, key);
	printf("~~~ MYSTERION-128 ~~~\nRunning...\n");
	unsigned long long before, after;
  cpucycles(before);
  clock_t begin = clock();
	for(i=0;i<NB_128_WORDS_FOR_BENCHMARKING;i++) {
		cipher = encrypt_state(cipher, key);
	}
  cpucycles(after);
  clock_t end = clock();
  float time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

  unsigned long long nb_cycles = after-before;
  float nb_cycles_per_byte = (1.0*nb_cycles)/(NB_128_WORDS_FOR_BENCHMARKING*16);
  float kilobytes_per_sec = ((NB_128_WORDS_FOR_BENCHMARKING*16)/time_spent)/1024;
  printf("- Nb Cycles: %llu\n", nb_cycles);
  printf("- Nb Cycle per Byte: %f\n", nb_cycles_per_byte);
  printf("- KB/s: %f\n", kilobytes_per_sec);
  printf("Chiffrement final:\n");
  print_state(cipher);
}
