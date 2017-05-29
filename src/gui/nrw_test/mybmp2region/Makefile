mybmp2region : mybmp2region.c 
	gcc mybmp2region.c -g -o mybmp2region -lminigui_ths -lfreetype -ljpeg -lpng -lz -lpthread
mginit : mginit.c mybmp2region.c 
	gcc mginit.c -o mginit -lminigui_procs -lfreetype -ljpeg -lpng -lz -lpthread
	gcc mybmp2region.c -g -o mybmp2region -lminigui_procs -lfreetype -ljpeg -lpng -lz -lpthread

clean:
	rm -f mginit mybmp2region
