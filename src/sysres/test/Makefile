all: drawicon skin_test drawcursor drawimage

drawicon: drawicon.c	
	gcc -Wall -g drawicon.c -o drawicon -lminigui_ths -lpng -ljpeg -lpthread -lfreetype

skin_test: skin_test.c
	gcc -Wall -g skin_test.c -o skin_test -lminigui_ths -lpng -ljpeg -lpthread -lfreetype

drawcursor: drawcursor.c  	
	gcc -Wall -g drawcursor.c -o drawcursor -lminigui_ths -lpng -ljpeg -lpthread -lfreetype

drawimage: drawimage.c  	
	gcc -Wall -g drawimage.c -o drawimage -lminigui_ths -lpng -ljpeg -lpthread -lfreetype




