
#add subdirs

mg_add_sub_dir_with_condition (nexusial MGIAL_NET)

if (NOT WIN32)
    mg_add_sub_dir_with_condition (dlcustom    MGIAL_DLCUSTOM)
    mg_add_sub_dir_with_condition (dlcustom    MGIAL_NEXUS)
    mg_add_sub_dir_with_condition (native      MGIAL_CONSOLE)
    set (src_list_for_nowin32
        lide.c lide.h
        jz4740.c jz4740.h
        )
endif (NOT WIN32)

list(APPEND ial_sources 
    ial.c 
    dummy.c dummy.h
    auto.c auto.h auto-protocol.h
    2440.c 2440.h
    davinci6446.c davinci6446.h
    random.c random.h
    pcxvfbial.c pcxvfb.h mgsock.c
    rtos_xvfb.c rtos_xvfb.h
    comminput.c comminput.h
    ipaq-h3600.c ipaq-h3600.h
    ipaq-h5400.c ipaq-h5400.h
    qemu.h qemu.c
    tslibial.h tslibial.c
    qvfb.c qvfb.h
    wvfbial.c wvfb.h
    cisco_touchpad.h cisco_touchpad.c
    mstarial.h mstarial.c
    dfb.h dfb.c
    ${src_list_for_nowin32}
    )


#append all files at current folder
#mg_add_all_source_files ()
mg_add_source_files (${ial_sources})

#commit _source_list_ to parent
mg_commit_source_files ()

