CC=gcc 
CFLAGS=-Wall -g

all:bin2c qpf2c vbf2c upgradevbf bdf2vbf dumpupf qpf2upf upf2c

bin2c:bin2c.o

qpf2c:qpf2c.o

qpf2upf:qpf2upf.o

upf2c:upf2c.c

vbf2c:vbf2c.o

makegbrbf:makegbrbf.o

makeucs2gbtable:makeucs2gbtable.o

upgradevbf:upgradevbf.o

bdf2vbf:bdf2vbf.c

dumpupf:
	gcc -lminigui_ths -lttf  -ljpeg -lpng -lpthread -lm -lz -o dumpupf dumpupf.c

clean:
	rm -f qpf2c qpf2upf upf2c vbf2c bin2c dumpqpf dumpupf dumpqpf2 upgradevbf bdf2vbf *.o
