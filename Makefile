CC1= -g -o bin/shascan  -Wall
CC2= -g -o bin/shainspect  -Wall


all: hello

hello:  
	gcc $(CC1) src/shascan.c
	gcc $(CC2) src/shainspect.c

clear:
	rm -rf bin/*
	rm 0x01