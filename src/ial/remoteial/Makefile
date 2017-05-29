CFLAGS=-I../../../include/ -fPIC -shared  -DUSEDLCUSTOM

CC=gcc 
CUSTOM_OBJS=netial_server.o
BINS=libnetial.so

all:$(BINS)

libnetial.so : $(CUSTOM_OBJS)
	$(CC) -o $@ $^  $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf *.so $(CUSTOM_OBJS)

