
#mg_add_all_source_files()

list (APPEND mybmp_sources
    mybmp.c winbmp.c gif.c jpeg.c png.c lbm.c pcx.c tga.c 
    )
mg_add_source_files (${mybmp_sources})
mg_commit_source_files()

