AM_CPPFLAGS = -I$(abs_top_srcdir)/include -I$(abs_top_srcdir)/src/control/ -I$(abs_top_srcdir)/src/include

noinst_LTLIBRARIES = libexctrl.la

SRC_FILES = \
	treeview.c \
	treeview_rdr.c \
	monthcalendar.c \
    spinbox.c \
	coolbar.c \
	listview.c \
    gif89a.c \
	animation.c \
	iconview.c \
	gridview.c \
	gridviewcelltype.c

libexctrl_la_SOURCES=$(SRC_FILES)

EXTRA_DIST=makefile.ng makefile.msvc \
		treeview_impl.h \
		monthcalendar_impl.h \
		spinbox_impl.h \
		coolbar_impl.h \
		listview_impl.h \
		animation_impl.h \
		iconview_impl.h \
		gridview_impl.h

LIB_NAME=libexctrl
