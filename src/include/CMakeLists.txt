
#append all files to _source_list_.
#mg_add_all_source_files ()
list (APPEND src_include_sources
    accelkey.h bidi.h bitmap.h cliprect.h ctrlclass.h
    cursor.h dc.h event.h list.h element.h
    ial.h inline.h internals.h zorder.h menu.h misc.h
    msgstr.h sysfont.h timer.h devfont.h fontname.h
    readbmp.h icon.h blockheap.h
    ourhdr.h client.h server.h sharedres.h sockio.h drawsemop.h
    gal.h newgal.h memops.h incoreres.h sysres.h clipboard.h
    md5.h glyph.h
    )

mg_add_source_files (${src_include_sources})

#commit to parent
mg_commit_source_files ()

