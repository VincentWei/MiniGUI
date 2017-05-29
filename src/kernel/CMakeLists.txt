
#append source to _source_list_, depend on variable of 'MG_MINIMALGDI'
if (MG_MINIMALGDI)
    list (APPEND kernel_sources
        init-lite.c fixstr.c blockheap.c
        )
else ()
    list (APPEND kernel_sources
        timer.c
        init.c init-lite.c
        desktop-ths.c desktop-sa.c desktop-procs.c
        cursor.c cursor-procs.c cursor-sa.c
        sharedres.c
        message.c event.c
        blockheap.c fixstr.c slotset.c zorder.c
        )
endif (MG_MINIMALGDI)

mg_add_source_files (${kernel_sources})

#commit _source_list_ to parent
mg_commit_source_files ()

