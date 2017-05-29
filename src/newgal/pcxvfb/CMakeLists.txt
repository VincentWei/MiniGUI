
#mg_add_all_source_files()

if (WIN32)
    set (src_list_for_win32
        pcxvfb-winfb.c pcxvfb-winfb.h
        )
endif (WIN32)

list (APPEND pcxvfb_sources
    pcxvfb.h pcxvfb.c
    ${src_list_for_win32}
    )
mg_add_source_files (${pcxvfb_sources})
mg_commit_source_files()
