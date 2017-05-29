noinst_PROGRAMS = test_window test_draw test_calccolor ctrldemo test_draw_menu

SUBDIRS = res

test_window_SOURCES = test_window.c font_dialog.c
test_window_LDADD = -lmgutils

test_draw_SOURCES = test_draw.c

test_draw_menu_SOURCES = test_draw_menu.c

test_calccolor_SOURCES = test_calccolor.c

ctrldemo_SOURCES = ctrls.h resource.h ctrldemo.c subclass.c \
                static.c button_my.c edit.c listbox.c combobox.c menubutton.c \
				toolbar.c trackbar.c progbar.c \
                monthcalendar.c treeview.c listview.c coolbar.c spinbox.c \
                timeedit.c newtoolbar.c 
ctrldemo_LDADD = -lmgutils

EXTRA_DIST=my_debug.h

