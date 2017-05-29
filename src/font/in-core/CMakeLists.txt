
#add to parent source list
#mg_add_all_source_files ()
list (APPEND incore_sources 
    rbf_fixedsys_8x15.c
    rbf_terminal_8x12.c
    rbf_vgaoem_8x8.c
    vbf_courier_8x13.c
    vbf_sansserif_11x13.c
    vbf_system_14x16.c
    upf_times_12x10.c
    upf_times_17x14.c
    )

mg_add_source_files (${incore_sources})

#commit to parent
mg_commit_source_files ()

