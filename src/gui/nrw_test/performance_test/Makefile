mginit : mginit.c normalwin.c nr_win.c trans_nr_win.c roundcornerwin.c nr_ctrl.c nr_ctrl2.c nr_edit.c
	gcc roundcornerwin.c -o roundcornerwin -g 		-lmgplus -lminigui_procs -lfreetype -ljpeg -lpng -lz -lpthread
	gcc normalwin.c -o normalwin -g 				-lmgplus -lminigui_procs -lfreetype -ljpeg -lpng -lz -lpthread
	gcc nr_win.c -o nr_win -g -lminigui_procs 		-lmgplus -lfreetype -ljpeg -lpng -lz -lpthread
	gcc nr_ctrl.c -o nr_ctrl -g -lminigui_procs 	-lmgplus -lfreetype -ljpeg -lpng -lz -lpthread
	gcc nr_ctrl2.c -o nr_ctrl2 -g -lminigui_procs 	-lmgplus -lfreetype -ljpeg -lpng -lz -lpthread
	gcc nr_btn.c -o nr_btn -g -lminigui_procs 		-lmgplus -lfreetype -ljpeg -lpng -lz -lpthread
	gcc nr_edit.c -o nr_edit -g -lminigui_procs 	-lmgplus -lfreetype -ljpeg -lpng -lz -lpthread
	gcc nr_prog.c -o nr_prog -g -lminigui_procs 	-lmgplus -lfreetype -ljpeg -lpng -lz -lpthread
	gcc trans_nr_win.c -o trans_nr_win -g 			-lmgplus -lminigui_procs -lfreetype -ljpeg -lpng -lz -lpthread
	gcc mginit.c -o mginit -g -lminigui_procs 		-lmgplus -lfreetype -ljpeg -lpng -lz -lpthread

roundcornerwin : roundcornerwin.c
	gcc roundcornerwin.c -o roundcornerwin -g -lminigui_ths -lfreetype -ljpeg -lpng -lz -lpthread
thread_test : thread_test.c
	gcc thread_test.c -o thread_test -g -lminigui_ths -lfreetype -ljpeg -lpng -lz -lpthread

clean:
	rm -f mginit normalwin nr_win trans_nr_win roundcornerwin \
		thread_test nr_ctrl nr_ctrl2 nr_btn nr_edit nr_prog
