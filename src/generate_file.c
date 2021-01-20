#include<stdio.h>
#include<stdlib.h>

void main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("ERROR:\n\t-Expecting a size (in bytes) as a first argument,\n\t-Name of the generated file as a second argument.\n");
    exit(1);
  }
  int size = atoi(argv[1]);
  FILE* current_file = fopen(argv[2], "wb");
  if (current_file==NULL) {
    printf("ERROR: Can not create the %s file.\n", argv[2]);
    exit(1);
  }
  int i;
  for(i=0;i<size;i++) {
    char rd = rand()&0xFF;
    fwrite(&rd, sizeof(char), 1, current_file);
  }
  fclose(current_file);
}
