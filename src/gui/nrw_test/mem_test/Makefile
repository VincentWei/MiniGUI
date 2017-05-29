mginit : mginit.c mem_test.c
	gcc mtrace_test.c -o mtrace_test -lminigui_procs -lfreetype -ljpeg -lpng -lz -lpthread
	gcc mem_test.c -o mem_test -lminigui_procs -lfreetype -ljpeg -lpng -lz -lpthread
	gcc mginit.c -o mginit -lminigui_procs -lfreetype -ljpeg -lpng -lz -lpthread
mem_test : mem_test.c
	gcc mem_test.c -o mem_test -lminigui_ths -lfreetype -ljpeg -lpng -lz -lpthread
mtrace_test : mtrace_test.c
	gcc mtrace_test.c -o mtrace_test -lminigui_ths -lfreetype -ljpeg -lpng -lz -lpthread

clean:
	rm -f mtrace_test mem_test mginit
