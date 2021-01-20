#ifndef __MYSTERION_EXT__
#define __MYSTERION_EXT__


#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct {
	uint32_t state[4];
} m_state;

// Defines 
#define NB_ROUND 12
#define true 0x01
#define false 0x00
#define __OK__ 0x01
#define __KEYLEN__ 0x02
#define __NOK__ 0xFF

// Typedef
typedef uint32_t error_t;
typedef uint8_t bool;

// Internal Functions
m_state LBox(const m_state* L, m_state current_state);
m_state MultGF16(m_state A, m_state B);
void ShiftColumns(m_state* current_state);
void SBox(m_state* current_state);
void state_to_buffer(m_state* current_state, uint8_t* buffer);
error_t char_to_state(char* A, m_state* current_state);

// External API
m_state encrypt_state(m_state current_state, m_state key);
void print_state(m_state current_state);
int encrypt_decrypt(FILE* current_file_IN, FILE* current_file_OUT, bool encrypt, m_state key);

#endif
