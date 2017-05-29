CFLAGS=-I../../../../include/ -fPIC -shared 

CC=gcc 
DLCUSTOM_OBJS=iale_custom.o
TARGET=libiale_custom.so

all:$(TARGET)

$(TARGET) : $(DLCUSTOM_OBJS)
	$(CC) -o $@ $^  $(CFLAGS)

.PHONY: clean
clean:
	-rm -rf $(TARGET) $(DLCUSTOM_OBJS)

