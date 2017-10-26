
mg_add_sub_source_dirs (bmp icon font cursor) 

if (MG_MINIMALGDI)
    list (APPEND sysres_sources 
            mgetc.c)
else()
    list (APPEND sysres_sources
            resource.c incoreres.c resmgr.c resmgr.h) 
    IF (WIN32)
        LIST (APPEND sysres_sources mgetc_win32.c)
    ELSE ()
        LIST (APPEND sysres_sources mgetc.c)
    ENDIF (WIN32)
endif(MG_MINIMALGDI)

mg_add_source_files(${sysres_sources})
mg_commit_source_files()
