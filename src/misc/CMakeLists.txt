
if(MG_MINIMALGDI)
    set(misc_sources 
        misc.c math.c rwops.c nposix.c)
else()
    set(misc_sources 
        misc.c systext.c rwops.c endianrw.c nposix.c clipboard.c
        math.c about.c license.c
        error.c sockio.c listenfd.c)
    
if (WIN32)
    list (APPEND misc_sources misc-win.c)
endif()
endif(MG_MINIMALGDI)

mg_add_source_files(${misc_sources})

#commit to parent
mg_commit_source_files ()


#message(STATUS "misc add source file ok")
