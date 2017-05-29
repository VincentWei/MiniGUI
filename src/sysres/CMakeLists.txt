
mg_add_sub_source_dirs (bmp icon font cursor) 

if (MG_MINIMALGDI)
    list (APPEND sysres_sources 
            mgetc.c)
else()
    list (APPEND sysres_sources
            resource.c incoreres.c mgetc.c resmgr.c resmgr.h) 
endif(MG_MINIMALGDI)

mg_add_source_files(${sysres_sources})
mg_commit_source_files()
