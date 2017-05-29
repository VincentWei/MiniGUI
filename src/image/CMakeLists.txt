
#mg_add_all_source_files()
list (APPEND image_sources
    cdjpeg.h gif.c imgapi.c imgapi.h jcomapi.c jdapimin.c
    jdapistd.c jdatasrc.c jdcoefct.c jdcolor.c jdct.h jddctmgr.c
    jdhuff.c jdhuff.h jdinput.c jdmainct.c jdmarker.c jdmaster.c
    jdmerge.c jdphuff.c jdpostct.c jdsample.c jdtrans.c jidct.c
    jmorecfg.h jpegint.h jpeglib.h jpg.c jutils.c
    )
mg_add_source_files (${image_sources})

mg_commit_source_files()
