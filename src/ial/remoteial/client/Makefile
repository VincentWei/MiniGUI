CFLAGS=-I../../../include/
LDFLAGS=-L./ -lminigui_ths -ldl -lm -lpng -ljpeg -lz -lpthread

CC=gcc 
CUSTOM_OBJS=netial.o netial_ctrlwin.o
BINS=netial

all:$(BINS)

$(BINS) : $(CUSTOM_OBJS)
	$(CC) -o $@ $^  $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm $(CUSTOM_OBJS) $(BINS)

