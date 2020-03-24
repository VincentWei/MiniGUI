AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include

noinst_LTLIBRARIES = libime.la

EXTRA_DIST = hzinput.c  hzinput.h  pinyin.c  pinyin.h

if IME_GB2312_SUPPORT
GB2312_SRCS = hzinput.c hzinput.h
else
GB2312_SRCS =
endif

if IME_GB2312_PINYIN_SUPPORT
GB2312_PINYIN_SRCS = pinyin.c pinyin.h
else
GB2312_PINYIN_SRCS =
endif

libime_la_SOURCES = $(GB2312_SRCS) $(GB2312_PINYIN_SRCS)

