////////////////////////
// Mysterion-128      //
// Auteur : JME       //
// Date : 7/12/2020   //
////////////////////////

#include "mysterion_cst.h"
#include "misc.c"

#include <stdio.h>

#define ROL32(X, s) (((X) << s) ^ ((X) >>(32-s)))

void print_state(m_state current_state) {
	printf("------------------------------------\n");
	print_bin(current_state.state[0]);
	print_bin(current_state.state[1]);
	print_bin(current_state.state[2]);
	print_bin(current_state.state[3]);
}

/*
	SBox,
	- 4 XOR,
	- 3 AND,
	- 1 OR
	Soit 8 instructions.
*/

void SBox(m_state* current_state) {
	uint32_t A = current_state->state[0] ;
	uint32_t B = current_state->state[1] ;
	uint32_t C = current_state->state[2] ;
	uint32_t D = current_state->state[3] ;
	current_state->state[0] = (A&B)^C;
	current_state->state[2] = (B|C)^D;
	current_state->state[3] = (current_state->state[0]&D)^A;
	current_state->state[1] = (current_state->state[2]&A)^B;
}

void SBox_Inv(m_state* current_state) {
	uint32_t A = current_state->state[0] ;
	uint32_t B = current_state->state[1] ;
	uint32_t C = current_state->state[2] ;
	uint32_t D = current_state->state[3] ;
	current_state->state[1] = (C&D)^B;
	current_state->state[3] = (A | current_state->state[1])^C;
	current_state->state[0] = (current_state->state[3] & A)^D;
	current_state->state[2] = ((current_state->state[0] & current_state->state[1]))^A;
}

/*
	ShiftColumns
	(sans compter la gestion de la boucle -- on peut éventuellement la dérouler si besoin de perf -- ni les accès mémoires).
		- 16 AND (&),
		- 24 XOR (^),
		- 12 SHIFT (<<),
		- 12 Substractions (-),
   Si on considère que chacune de ces instructions prenne un cycle
	 - 64 Instructions pour un ShiftColumns
*/

void ShiftColumns(m_state* current_state) {
	int i;
	for(i=0;i<4;i++) {
		uint32_t X = current_state->state[i];
		X = (X&0xC0C0C0C0) ^ ROL32(X&0x03030303, 8) ^ ROL32(X&0x0C0C0C0C, 16) ^ ROL32(X&0x30303030, 24);
		current_state->state[i] = X ;
	} // fin de la boucle sur i.
}

void ShiftColumns_Inv(m_state* current_state) {
	int i;
	for(i=0;i<4;i++) {
		uint32_t X = current_state->state[i];
		X = (X&0xC0C0C0C0) ^ ROL32(X&0x03030303, 24) ^ ROL32(X&0x0C0C0C0C, 16) ^ ROL32(X&0x30303030, 8);
		current_state->state[i] = X ;
	} // fin de la boucle sur i.
}

/*
	MultGF16
	La complexité de cette opération peut être probablement allégée (factorisation de certaines expressions, etc)
	En l'état, on dénote
	 	- 18 XOR (^)
		- 12 AND (&)
		Si on considère que chacune de ces instructions prennent un cycle
 	 - 30 Instructions pour un MultGF16
*/

m_state MultGF16(m_state A, m_state B) {
	m_state ret;
	ret.state[3] = (A.state[3]&B.state[3]) ^ (A.state[2]&B.state[0]) ^ (A.state[1]&B.state[1]) ^ (A.state[0]&B.state[2]);
	ret.state[2] = (A.state[2]&B.state[3]) ^ ((A.state[3] ^ A.state[0])&B.state[2]) ^ ((A.state[1]^A.state[0])&B.state[1]) ^ ((A.state[1] ^ A.state[2])&B.state[0]);
	ret.state[1] = (A.state[1]&B.state[3]) ^ (A.state[2]&B.state[2]) ^ ((A.state[3]^A.state[0])&B.state[1]) ^ ((A.state[1] ^ A.state[0])&B.state[0]);
	ret.state[0] = (A.state[0]&B.state[3]) ^ (A.state[1]&B.state[2]) ^ (A.state[2]&B.state[1]) ^ ((A.state[3] ^ A.state[0])&B.state[0]);
	return ret;
}

/*
	L = tableau de 8 m_state, soit 8 colonnes de 32 mots de 4 bits (32*8*4) = 1024 bits
	La fonction la plus lourde ...
	- 8xMultGF16 = 240 Instructions,
	- 32 XOR,
	- 32 OR,
	- 64 AND,
	- 64 Shifts,
	Soit environ 432 instructions.
*/
m_state LBox(const m_state* L, m_state current_state) {
	int i,j;
	m_state ret ;
	ret.state[0] = 0x00000000;
	ret.state[1] = 0x00000000;
	ret.state[2] = 0x00000000;
	ret.state[3] = 0x00000000;
	for(i=0;i<8;i++) {
		m_state tmp = MultGF16(L[i], current_state);
		for(j=0;j<4;j++) {
			ret.state[j] ^= tmp.state[j];
			current_state.state[j] = (((current_state.state[j] << 1) & 0xfefefefe) | ((current_state.state[j] >> 7) & 0x01010101));
		} // fin de boucle sur j.
		// printf("~~~ State ~~~\n");
		// print_state(ret);
		// printf("~~~ Multiplication ~~~\n");
		// print_state(L[i]);
	} // fin de boucle sur i.
	return ret;
}

/*
	NB_ROUND == 12
	- 4 XOR + NB_ROUND*8 XOR = 4+12*8 = 100 XOR,
	- NB_ROUND*SBOX = 12*8 Instructions = 96 Instructions,
	- NB_ROUND*LBOX = 12*432 = 5184 Instructions
	- NB_ROUND*ShiftColumns = 12*64 = 768 Instructions
	Soit environ
		6148 instructions.
*/

m_state encrypt_state(m_state current_state, m_state key) {
	current_state.state[0] ^= key.state[0];
	current_state.state[1] ^= key.state[1];
	current_state.state[2] ^= key.state[2];
	current_state.state[3] ^= key.state[3];
	uint8_t round;
	for(round=0;round<NB_ROUND;round++) {
		SBox(&current_state);
		current_state = LBox(L, current_state);
		ShiftColumns(&current_state);
		uint8_t i = round*4;
		current_state.state[0] ^= (key.state[0]^C[i+0]);
		current_state.state[1] ^= (key.state[1]^C[i+1]);
		current_state.state[2] ^= (key.state[2]^C[i+2]);
		current_state.state[3] ^= (key.state[3]^C[i+3]);
	} // fin de boucle sur round.
	return current_state;
}


m_state decrypt_state(m_state current_state, m_state key) {
	uint8_t round;
	for(round=0;round<NB_ROUND;round++) {
		uint8_t i = round*4;
		current_state.state[0] ^= (key.state[0]^C_Inv[i+3]);
		current_state.state[1] ^= (key.state[1]^C_Inv[i+2]);
		current_state.state[2] ^= (key.state[2]^C_Inv[i+1]);
		current_state.state[3] ^= (key.state[3]^C_Inv[i+0]);
		ShiftColumns_Inv(&current_state);
		current_state = LBox(L_Inv, current_state);
		SBox_Inv(&current_state);
	}
	current_state.state[0] ^= key.state[0];
	current_state.state[1] ^= key.state[1];
	current_state.state[2] ^= key.state[2];
	current_state.state[3] ^= key.state[3];
	return current_state;
}

m_state xor_states(m_state A, m_state B) {
	m_state ret;
	ret.state[0] = (A.state[0]^B.state[0]);
	ret.state[1] = (A.state[1]^B.state[1]);
	ret.state[2] = (A.state[2]^B.state[2]);
	ret.state[3] = (A.state[3]^B.state[3]);
	return ret;
}

/* A's expected length: 8*16 */
error_t char_to_state(char* A, m_state* current_state) {
	error_t error = __OK__;
	if (strlen(A) != 16) {
		error = __KEYLEN__;
		goto end_of_function;
	}
	int i, j;
	for(i=0;i<4;i++) {
		uint32_t tmp = 0;
		for(j=0;j<4;j++) {
			tmp += (uint32_t) A[4*i+j];
			tmp <<= 8;
		}
		current_state->state[i] = tmp;
	}
	end_of_function:
		return error;
}

void state_to_buffer(m_state* current_state, uint8_t* buffer) {
	int i, j;
	for(i=0;i<4;i++) {
		uint32_t tmp = current_state->state[i];
		for(j=0;j<4;j++) {
			buffer[4*i+j] = (tmp >> (8*j))&0xFF;
		} // fin de boucle sur j.
	} // fin de boucle sur i.
}

int encrypt_decrypt(FILE* current_file_IN, FILE* current_file_OUT, bool encrypt, m_state key)
{
	m_state tmp_state;
	bool encrypting = true;
	uint8_t padding_value;
	int current_input_file_length = 0;
	m_state previous_state;
	uint8_t buffer[16];
	while(encrypting) {
		size_t size;
		int i;
		for(i=0;i<16;i++) {
			size = fread(&buffer[i], sizeof(char), 1, current_file_IN);
			current_input_file_length += size;
			if (size == 0)
			{
				encrypting = false;
				break;
			}
		} // fin de boucle sur i.
		if (encrypt) {
			// Padding PKCS7.
			padding_value = 16-i;
			for(i;i<16;i++) {buffer[i] = padding_value;}
			// Create state
			tmp_state.state[0] = buffer[0]+(buffer[1]<<8)+(buffer[2]<<16)+(buffer[3]<<24);
			tmp_state.state[1] = buffer[4]+(buffer[5]<<8)+(buffer[6]<<16)+(buffer[7]<<24);
			tmp_state.state[2] = buffer[8]+(buffer[9]<<8)+(buffer[10]<<16)+(buffer[11]<<24);
			tmp_state.state[3] = buffer[12]+(buffer[13]<<8)+(buffer[14]<<16)+(buffer[15]<<24);
			tmp_state = encrypt_state(tmp_state, key);
			fwrite(&tmp_state, sizeof(m_state), 1, current_file_OUT);
		}
		else {
			if (encrypting) {
				tmp_state.state[0] = buffer[0]+(buffer[1]<<8)+(buffer[2]<<16)+(buffer[3]<<24);
				tmp_state.state[1] = buffer[4]+(buffer[5]<<8)+(buffer[6]<<16)+(buffer[7]<<24);
				tmp_state.state[2] = buffer[8]+(buffer[9]<<8)+(buffer[10]<<16)+(buffer[11]<<24);
				tmp_state.state[3] = buffer[12]+(buffer[13]<<8)+(buffer[14]<<16)+(buffer[15]<<24);
				tmp_state = decrypt_state(tmp_state, key);
				if (current_input_file_length > 16) {
					fwrite(&previous_state, sizeof(m_state), 1, current_file_OUT);
				}
				previous_state = tmp_state;
			}
		}
	}
	// Dans le cas d'un déchiffrement, on traite le padding
	if (encrypt==false) {
		padding_value = ((previous_state.state[3] >> 24) & 0xFF);
		if (padding_value < 16) {
			state_to_buffer(&previous_state, buffer);
			fwrite(buffer, sizeof(uint8_t), 16-padding_value, current_file_OUT);
		}
	}
	return current_input_file_length;
}
