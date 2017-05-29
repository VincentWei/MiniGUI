all:hebrew_test
r:hebrew_test_release
d:hebrew_test_debug 

helloworld:helloworld.c
	gcc helloworld.c -Wall -g -o helloworld -lminigui_ths -lm -ljpeg -lz -lfreetype -lpng -lpthread

hebrew_test:hebrew_test.c
	gcc hebrew_test.c -g -o hebrew_test -lminigui_ths -lm -ljpeg -lz -lttf -lpng -lpthread

hebrew_test_release:hebrew_test.c
	gcc hebrew_test.c -g -o hebrew_test -lminigui_ths -lm -ljpeg -lz -lttf -lpng -lpthread

hebrew_test_debug:hebrew_test.c
	gcc hebrew_test.c -DARABIC_DEBUG -g -o hebrew_test -lminigui_ths -lm -ljpeg -lz -lttf -lpng -lpthread

hebrew_test_unicode:hebrew_test.c
	gcc hebrew_test.c -DARABIC_UNICODE -DARABIC_DEBUG -g -o hebrew_test_u -lminigui_ths -lm -lfreetype -ljpeg -lz -lpng -lpthread

shape_test:shape_test.c
	gcc shape_test.c  -Wall -g -o shape_test -lminigui_ths -lm -ljpeg -lz -lpng -lpthread

combin_test:combin_test.c
	gcc combin_test.c -Wall -g -o combin_test -lminigui_ths -lm -ljpeg -lz -lpng -lpthread

clean:
	rm hebrew_test
	rm hebrew_test_release
	rm hebrew_test_debug 


