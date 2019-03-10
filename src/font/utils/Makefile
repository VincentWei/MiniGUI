CC=gcc
CFLAGS=-Wall -Werror -g -D__MINIGUI_LIB__ -I../../../include/

all:mkmap-ucs-gb18030 mkmap-ucs-gb2312 \
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

clean:
	rm -f *.o mkmap-ucs-gb2312 mkmap-gb18030-ucs.c mkmap-ucs-gb18030 \
    gen-unicode-version \
    gen-bidi-type-tab \
    gen-joining-type-tab \
    gen-arabic-shaping-tab \
    gen-mirroring-tab \
    gen-brackets-tab \
    gen-brackets-type-tab

