AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include

noinst_LTLIBRARIES = libcontrol.la

SRC_FILES = ctrlmisc.c \
              static.c button.c edit.c listbox.c progressbar.c \
              menubutton.c trackbar.c combobox.c propsheet.c \
              newtoolbar.c scrolled.c scrollwnd.c scrollview.c \
			  textedit.c listmodel.c scrollbar.c bidiedit.c

libcontrol_la_SOURCES = $(SRC_FILES) 

EXTRA_DIST=makefile.ng makefile.msvc \
        button_impl.h ctrlmisc.h simedit_impl.h edit_impl.h listbox_impl.h \
		medit_impl.h bidiedit_impl.h \
        progressbar_impl.h static_impl.h menubutton_impl.h trackbar_impl.h \
        combobox_impl.h propsheet_impl.h newtoolbar_impl.h scrolled.h \
        scrollwnd.h scrollview_impl.h textedit_impl.h text.h listmodel.h 

LIB_NAME=libcontrol
