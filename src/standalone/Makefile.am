AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include

noinst_LTLIBRARIES = libstandalone.la

if MG_MINIMALGDI
libstandalone_la_SOURCES = 
else
libstandalone_la_SOURCES = standalone.c
endif

