all: xxvfb.o xxvfb_common.o xxvfb_input.o main.c
	gcc -g -o xxvfb main.c xxvfb.o xxvfb_input.o xxvfb_common.o -lX11
xxvfb.o:define.h xxvfbhdr.h xxvfb.h xxvfb.c
	gcc -g -c xxvfb.c
xxvfb_common.o:define.h xxvfbhdr.h xxvfb_common.c
	gcc -g -c xxvfb_common.c
xxvfb_input.o: xxvfb_input.c xxvfb_input.h
	gcc -g -c xxvfb_input.c
install:
	cp ./xxvfb /usr/local/bin/
clean:
	rm -rf *.o
	rm xxvfb
