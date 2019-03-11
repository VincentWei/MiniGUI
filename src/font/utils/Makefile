CC=gcc
CFLAGS=-Wall -Werror -g -D__MINIGUI_LIB__ -I../../../include/
COMPRESSION=2

all:mkmap-ucs-gb18030 mkmap-ucs-gb2312 \
    unicode-version.h \
    gen-unicode-version \
    gen-bidi-type-tab \
    gen-joining-type-tab \
    gen-arabic-shaping-tab \
    gen-mirroring-tab \
    gen-brackets-tab \
    gen-brackets-type-tab

mkmap-ucs-gb2312:mkmap-ucs-gb2312.o

mkmap-ucs-gb18030:mkmap-ucs-gb18030.o

gen-unicode-version:gen-unicode-version.o
gen-bidi-type-tab:gen-bidi-type-tab.o packtab.o
gen-joining-type-tab:gen-joining-type-tab.o packtab.o
gen-arabic-shaping-tab:gen-arabic-shaping-tab.o packtab.o
gen-mirroring-tab:gen-mirroring-tab.o packtab.o
gen-brackets-tab:gen-brackets-tab.o packtab.o
gen-brackets-type-tab:gen-brackets-type-tab.o packtab.o

biditables:unicode-version.h \
    unicode-bidi-type-table.inc \
    unicode-joining-type-table.inc \
    unicode-arabic-shaping-table.inc \
    unicode-bidi-mirroring-table.inc \
    unicode-bidi-brackets-table.inc \
    unicode-bidi-brackets-type-table.inc

unicode-version.h:ucd/ReadMe.txt gen-unicode-version
	./gen-unicode-version ucd/ReadMe.txt > unicode-version.h

clean:
	rm -f *.o unicode-*.h unicode-*.inc \
    mkmap-ucs-gb2312 mkmap-gb18030-ucs.c mkmap-ucs-gb18030 \
    gen-unicode-version \
    gen-bidi-type-tab \
    gen-joining-type-tab \
    gen-arabic-shaping-tab \
    gen-mirroring-tab \
    gen-brackets-tab \
    gen-brackets-type-tab

unicode-bidi-type-table.inc: \
		ucd/UnicodeData.txt \
		unicode-version.h \
		gen-bidi-type-tab
	./gen-bidi-type-tab $(COMPRESSION) $^ > $@

unicode-joining-type-table.inc: \
		ucd/UnicodeData.txt \
		ucd/ArabicShaping.txt \
		unicode-version.h \
		gen-joining-type-tab
	./gen-joining-type-tab $(COMPRESSION) $^ > $@

unicode-arabic-shaping-table.inc: \
		ucd/UnicodeData.txt \
		unicode-version.h \
		gen-arabic-shaping-tab
	./gen-arabic-shaping-tab $(COMPRESSION) $^ > $@

unicode-bidi-mirroring-table.inc: \
		ucd/BidiMirroring.txt \
		unicode-version.h \
		gen-mirroring-tab
	./gen-mirroring-tab $(COMPRESSION) $^ > $@

unicode-bidi-brackets-table.inc: \
		ucd/BidiBrackets.txt \
		ucd/UnicodeData.txt \
		unicode-version.h \
		gen-brackets-tab
	./gen-brackets-tab $(COMPRESSION) $^ > $@

unicode-bidi-brackets-type-table.inc: \
		ucd/BidiBrackets.txt \
		unicode-version.h \
		gen-brackets-type-tab
	./gen-brackets-type-tab $(COMPRESSION) $^ > $@

