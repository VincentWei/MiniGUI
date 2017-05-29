#all:arabic_test shape_test combin_test
all:arabic_test arabic_test_release arabic_test_debug arabic_test_unicode arabic_test_hex 
r:arabic_test_release
d:arabic_test_debug 
u:arabic_test_unicode 
hex:arabic_test_hex 

MINIGUI_COMMON = /usr/local/include/minigui
LIBS = -lminigui_ths -lm -ljpeg -lz -lpng -lpthread -lttf
#LIBS = -lminigui_ths -lm -ljpeg -lz -lpng -lpthread -lfreetype

arabic_test:arabic_test.c $(MINIGUI_COMMON)
	gcc arabic_test.c -Wall -g -o arabic_test $(LIBS) 

arabic_test_release:arabic_test.c $(MINIGUI_COMMON)
	gcc arabic_test.c -g -o arabic_test  $(LIBS) 

arabic_test_debug:arabic_test.c $(MINIGUI_COMMON)
	gcc arabic_test.c -DARABIC_DEBUG -g -o arabic_test $(LIBS)

arabic_test_unicode:arabic_test.c $(MINIGUI_COMMON)
	gcc arabic_test.c -DARABIC_UNICODE -DARABIC_DEBUG -g -o arabic_test_u $(LIBS)

arabic_test_hex:arabic_test.c $(MINIGUI_COMMON)
	gcc arabic_test.c -DHEXSTRING -DARABIC_DEBUG -g -o arabic_test_hex $(LIBS)

shape_test:shape_test.c $(MINIGUI_COMMON)
	gcc shape_test.c  -Wall -g -o shape_test $(LIBS)

combin_test:combin_test.c $(MINIGUI_COMMON)
	gcc combin_test.c -Wall -g -o combin_test $(LIBS) 

clean:
	rm -f arabic_test
	rm -f arabic_test_u
	rm -f arabic_test_hex
	rm -f shape_test
	rm -f combin_test


