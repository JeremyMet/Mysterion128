
#include "mysterion.h" 

void main(int argc, char* argv[]) {
	m_state plain, key;
	error_t error;
	bool encrypt;
	if (argc != 5) {
		printf("ERROR:\n\t1st: mode (-e/-d), 2sd Key, 3rd input file, 4th output file.\n");
		exit(1);
	}
	// Vérification du mode (encrypt / decrypt)
	if (strcmp(argv[1], "-e")==0) {
		encrypt = true;
	}
	else if (strcmp(argv[1], "-d")==0) {
		encrypt = false;
	}
	else {
		printf("ERROR: Specify the mode -e: encrypt, -d: decrypt.\n");
		exit(1);
	}
	// Vérification de la clef.
	error = char_to_state(argv[2], &key);
	if (error == __KEYLEN__) {
		printf("ERROR: Key is not 128-bit long (16 chars).\n");
		exit(1);
	}
	// Vérification de la disponibilité du fichier à chiffrer.
	FILE* current_file_IN = fopen(argv[3], "rb");
	if (current_file_IN == NULL) {
		printf("ERROR: Can not open %s.\n", argv[3]);
		exit(1);
	}
	// Vérification de la possibilité d'écriture sur le fichier de sortie.
	FILE* current_file_OUT = fopen(argv[4], "wb");
	if (current_file_IN == NULL) {
		printf("ERROR: Can not open %s.\n", argv[4]);
		exit(1);
	}
	// Encrypt or Decrypt file.
	clock_t begin = clock();
	int nb_bytes = encrypt_decrypt(current_file_IN, current_file_OUT, encrypt, key);
	clock_t end = clock();
	double time_spent = (double) (end-begin)/CLOCKS_PER_SEC;
	printf("Done:\n\tEncrypted/Decrypted %d bytes in %fs.\n", nb_bytes, time_spent);
	fclose(current_file_IN);
	fclose(current_file_OUT);
}
