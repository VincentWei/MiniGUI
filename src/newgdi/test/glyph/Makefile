all:glyph-test glyph-draw glyph-fillmybitmap

glyph-test:glyph-test.c
	gcc glyph-test.c -Wall -g -o glyph-test -lminigui_ths -lrt -lm -ljpeg -lz -lfreetype -lpng -lpthread -I ../../../../include/ -I ../../../include/ -I ../../../newgdi/

glyph-draw:glyph-draw.c
	gcc glyph-draw.c -Wall -g -o glyph-draw -lminigui_ths -lrt -lm -ljpeg -lz -lfreetype -lpng -lpthread -I ../../../../include/ -I ../../../include/ -I ../../../newgdi/

glyph-fillmybitmap:glyph-bitmap.c
	gcc glyph-bitmap.c -Wall -g -o glyph-fillmybitmap -lminigui_ths -lrt -lm -ljpeg -lz -lpng -lpthread -I ../../../../include/ -I ../../../include/ -I ../../../newgdi/

clean:
	rm glyph-test glyph-draw glyph-fillmybitmap


