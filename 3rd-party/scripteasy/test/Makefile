all:sample
	
C = gcc
#CC = arm-linux-gcc

LD = gcc
#LD = arm-linux-gcc

MINIGUI_COMMON = /home/houhh/smb/Target/pc-rel-3
LIBS = -lminigui_ths -lscripteasy -ljpeg -lz -lpng -lpthread 

LDFLAGS = -L$(MINIGUI_COMMON)/lib -ldl
CFLAGS = -g -Wall -I$(MINIGUI_COMMON)/include

.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) $(CPPFLAGS) -c -o $a $<

sample:sample.c $(MINIGUI_COMMON)
	    $(CC)   sample.c font_table.c $(CFLAGS) -Wall -g -o sample $(LDFLAGS) $(LIBS)

clean:
	rm -f sample 
   


