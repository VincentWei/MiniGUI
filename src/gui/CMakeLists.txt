
#append all files to _source_list_.
#mg_add_all_source_files ()
list (APPEND gui_sources 
    accelkey.c caret.c ctrlclass.c dialog.c menu.c window.c
    keyboard.c
    defkeymap.c fr-pc-kmap.c fr-kmap.c de-kmap.c de-latin1-kmap.c
	hebrewkeymap.c arabickeymap.c
    it-kmap.c es-kmap.c es-cp850-kmap.c
	lf_manager.c lf_classic.c lf_flat.c lf_skin.c lf_common.c
    )

list (APPEND gui_sources
    keyboard.h linux_kd.h  linux_keyboard.h
    linux_types.h lf_common.h
    )
mg_add_source_files (${gui_sources})

#commit to parent
mg_commit_source_files ()

