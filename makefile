all: src/main.c src/mysterion.c
		cd src; \
		gcc main.c mysterion.c -o ../mysterion128;
