
# add sub dirs
mg_add_sub_source_dirs (in-core)

#append all files to _source_list_.
#mg_add_all_source_files ()

list (APPEND font_sources 
    charset.h rawbitmap.h varbitmap.h freetype.h freetype2.h qpf.h
    upf.h bitmapfont.h gunicode.h gunichartables.h se_minigui.h
    )

list (APPEND font_sources 
    charset.c charset-arabic.c charset-bidi.c
    sysfont.c logfont.c devfont.c fontname.c
    rawbitmap.c varbitmap.c qpf.c upf.c
    fontcache.c freetype.c freetype2.c font-engines.c
    gbunimap.c gbkunimap.c gb18030unimap.c big5unimap.c
    ujisunimap.c sjisunimap.c euckrunimap.c
    textops.c mapunitogb.c mapunitogbk.c mapunitobig5.c mapunitogb18030.c
    bitmapfont.c scripteasy.c
    )
mg_add_source_files (${font_sources})

#commit to parent
mg_commit_source_files ()

