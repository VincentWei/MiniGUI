if (MGIAL_DLCUSTOM)
list (APPEND subdir_list dlcustom)
endif (MGIAL_DLCUSTOM)

# add sub source directories
# should we add them in automatic?
if (MG_MINIMALGDI)
    mg_add_sub_source_dirs (
        misc
        kernel
        sysres
        include
        newgal
        newgdi
        )
else ()
    mg_add_sub_source_dirs (
        libc
        misc
        main
        kernel
        sysres
        ial
        control
        ex_ctrl
        font
        gui
        mybmp
        include
        textedit
        newgal
        newgdi
        )
endif (MG_MINIMALGDI)

if (MGRM_STANDALONE)
    mg_add_sub_source_dirs(standalone)
elseif (MGRM_PROCESSES)
    mg_add_sub_source_dirs(client server)
endif (MGRM_STANDALONE)

#
# debug
#
#message("=======================================================")

# sources
#message(STATUS "Source list:")
#foreach (_file ${_source_list_})
#        message(STATUS "   " ${_file})
#endforeach(_file)

# cflags
#message("CLFAGS:")
#foreach (_flag ${CMAKE_C_FLAGS})
#    message("   ${_flag}")
#endforeach(_flag)

#
# end of debug
#
#message("=======================================================")

include_directories ("${PROJECT_BINARY_DIR}" 
    "${PROJECT_SOURCE_DIR}/include" 
    "${PROJECT_SOURCE_DIR}/src/include"
    "${PROJECT_SOURCE_DIR}/src/newgal"
    "${PROJECT_SOURCE_DIR}/src/font"
    "${PROJECT_SOURCE_DIR}/src/control"
    )

if (PTHREAD_FOUND)
    include_directories ("${PTHREAD_INCLUDE_DIR}")
endif ()
if (ZLIB_FOUND)
    include_directories ("${ZLIB_INCLUDE_DIR}")
endif ()
if (JPEG_FOUND)
    include_directories ("${JPEG_INCLUDE_DIR}")
endif ()

# license picture
IF (license_splash OR license_screensaver OR license_productid)
    SET (DUMMY_FOR_LICENSE_PICTURE_C ${PROJECT_BINARY_DIR}/_dummy_for_license_picture.c)
    IF (NOT WIN32)
        ADD_CUSTOM_COMMAND (OUTPUT ${DUMMY_FOR_LICENSE_PICTURE_C}
            COMMAND ./runme.sh ${LICENSE_PICTURE} > /dev/null && echo "/* ${LICENSE_PICTURE} */" > ${DUMMY_FOR_LICENSE_PICTURE_C}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src/sysres/license/
            COMMENT "Going to generate license pictures ...")
    ELSE ()
        IF (NOT EXISTS ${DUMMY_FOR_LICENSE_PICTURE_C})
            MESSAGE(FATAL_ERROR "${DUMMY_FOR_LICENSE_PICTURE_C} not found. Please generate this file on a Linux machine, then CMake on Windows again")
        ENDIF (NOT EXISTS ${DUMMY_FOR_LICENSE_PICTURE_C})
    ENDIF (NOT WIN32)
    list (APPEND _source_list_ ${DUMMY_FOR_LICENSE_PICTURE_C})
ENDIF (license_splash OR license_screensaver OR license_productid)

# add minigui library
set (MINIGUI_LIBNAME ${MINIGUI_NAME}_${MGLIB_SUFFIX})

################################# target ###################################
# !!! to install dynamic library (.DLL) and import library (.lib)  !!!
# !!! in WIN32 we should use                                       !!!
# !!! *RUNTIME* and *ARCHIVE* to set the DESTINATION of the lib    !!!
if (_SHAREDLIB_OUTPUT)
    add_library (${MINIGUI_LIBNAME} SHARED ${_source_list_})
    set_target_properties (${MINIGUI_LIBNAME} PROPERTIES VERSION ${MINIGUI_VERSION} SOVERSION ${MINIGUI_SOVERSION})
    set_target_properties (${MINIGUI_LIBNAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
    if (WIN32)
        if (PTHREAD_FOUND)
            target_link_libraries(${MINIGUI_LIBNAME} "${PTHREAD_LIBRARY}")
        endif ()
        if (ZLIB_FOUND)
            target_link_libraries(${MINIGUI_LIBNAME} "${ZLIB_LIBRARY}")
        endif ()
        if (PNG_FOUND)
            target_link_libraries(${MINIGUI_LIBNAME} ${PNG_LIBRARIES})
        endif ()
        if (JPEG_FOUND)
            target_link_libraries(${MINIGUI_LIBNAME} "${JPEG_LIBRARY}")
        endif ()
        if (FREETYPE_FOUND)
            target_link_libraries(${MINIGUI_LIBNAME} ${FREETYPE_LIBRARIES})
        endif ()
    endif (WIN32)

    install (TARGETS ${MINIGUI_LIBNAME}
        LIBRARY DESTINATION lib     # for .so  (dynamic library)
        ARCHIVE DESTINATION lib     # for .lib (import library)
        RUNTIME DESTINATION bin     # for .dll (dynamic library)
        )
endif (_SHAREDLIB_OUTPUT)

if (_STATICLIB_OUTPUT)
    add_library (${MINIGUI_LIBNAME}_static ${_source_list_})
    if (NOT WIN32)
        set_target_properties (${MINIGUI_LIBNAME}_static PROPERTIES OUTPUT_NAME ${MINIGUI_LIBNAME})
    endif(NOT WIN32)
    set_target_properties (${MINIGUI_LIBNAME}_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

    install (TARGETS ${MINIGUI_LIBNAME}_static
        ARCHIVE DESTINATION lib
        )
endif(_STATICLIB_OUTPUT)
