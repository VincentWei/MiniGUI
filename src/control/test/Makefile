all:bidiedit_arabic scrollbar_test bidiedit_hebrew
	
CC = gcc
CFLAGS = -g -Wall

LD = gcc
LDFLAGS = 

MINIGUI_COMMON = /usr/local/include/minigui
LIBS = -lminigui_procs -ljpeg -lz -lpng -lpthread -lfreetype -lmgplus
#LIBS = -lminigui_ths -ljpeg -lz -lpng -lpthread -lfreetype


PROG = bidiedit_arabic scrollbar_test bidiedit_hebrew
OBJS = bidiedit_arabic.o scrollbar_test.o bidiedit_hebrew.o

.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) $(CPPFLAGS) -c -o $a $<

scrollbar_test:scrollbar_test.c scrollbar_test_res.c  $(MINIGUI_COMMON)
	gcc scrollbar_test.c scrollbar_test_res.c -g -o scrollbar_test $(LIBS)

bidiedit_arabic:bidiedit_arabic.c $(MINIGUI_COMMON)
	gcc bidiedit_arabic.c -g -o bidiedit_arabic $(LIBS)

bidiedit_hebrew:bidiedit_hebrew.c  $(MINIGUI_COMMON)
	gcc bidiedit_hebrew.c -g -o bidiedit_hebrew $(LIBS)

clean:
	rm -f scrollbar_test
	rm -f bidiedit_hebrew
	rm -f bidiedit_arabic
   


