all:mlshadowtest_left mlshadowtest_right window_mini_max

mlshadowtest_left:mlshadowtest_left.c
	gcc mlshadowtest_left.c -o mlshadowtest_left -lminigui -lm -ljpeg -lz -lmgext -lfreetype -lpng -lpthread
mlshadowtest_right:mlshadowtest_right.c
	gcc mlshadowtest_right.c -o mlshadowtest_right -lminigui -lm -ljpeg -lz -lmgext -lfreetype -lpng -lpthread
window_mini_max:window_mini_max.c
	gcc window_mini_max.c -o window_mini_max -lminigui -lm -ljpeg -lz -lmgext -lfreetype -lpng -lpthread

clean:
	rm mlshadowtest_left
	rm mlshadowtest_right
	rm window_mini_max


