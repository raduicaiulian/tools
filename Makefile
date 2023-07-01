all: program.c
	gcc program.c -g -o program -lm
decrypt: decrypt.c
	gcc decrypt.c -g -o decrypt -lmagic
