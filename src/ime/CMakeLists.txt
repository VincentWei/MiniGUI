
#append files to _source_list_. depend on variable IME_GB2312_SUPPORT
if (IME_GB2312_SUPPORT)
    list (APPEND ime_sources
        hzinput.c hzinput.h
        )
endif (IME_GB2312_SUPPORT)

#append files to _source_list_. depend on variable IME_GB2312_PINYIN_SUPPORT 
if (IME_GB2312_PINYIN_SUPPORT)
    list (APPEND ime_sources
        pinyin.c pinyin.h
        )
endif (IME_GB2312_PINYIN_SUPPORT)

# add source file to _source_list_
mg_add_source_files (${ime_sources})

#commit to parent
mg_commit_source_files ()

