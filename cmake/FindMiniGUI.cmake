# - Find MiniGUI
# Find the MiniGUI includes and library
# This module defines
#   MINIGUI_FOUND, whether is being found.
#   MINIGUI_INCLUDE_DIRS, where to find <minigui/minigui.h> header files, etc.
#   MINIGUI_LIBRARY_DIRS, where to find libminigui_* library.
#   MINIGUI_LIBRARIES, the libraries needed to use MiniGUI.

if (NOT WIN32)
    include(FindPkgConfig)
    pkg_check_modules (MINIGUI minigui)
endif ()

if (NOT MINIGUI_FOUND)

    # find the headfile of library
    set (MINIGUI_HEADS minigui/common.h)
    find_path (MINIGUI_INCLUDE_DIRS ${MINIGUI_HEADS})

    set (MINIGUI_NAMES 
        minigui minigui_ths minigui_procs minigui_sa minigui_msd
        libminigui_ths libminigui_procs libminigui_sa libminigui_msd )
    find_library (MINIGUI_LIBRARY NAMES ${MINIGUI_NAMES})

    # just find one of dependency, guess other one.
    if (NOT MINIGUI_LIBRARY AND MINIGUI_INCLUDE_DIRS)
        message ("We just find the headfile, try to guess the library location.")
        set (MINIGUI_LIBRARY_DIRS "${MINIGUI_INCLUDE_DIRS}/../lib")
        find_library (MINIGUI_LIBRARY NAMES ${MINIGUI_NAMES} PATHS ${MINIGUI_LIBRARY_DIRS})
    elseif (NOT MINIGUI_INCLUDE_DIRS AND MINIGUI_LIBRARY)
        message ("We just find the lib file, try to guess the include location.")
        string (REGEX REPLACE "[a-z|A-Z|0-9]+[.].*$" "" MINIGUI_LIBRARY_DIRS ${MINIGUI_LIBRARY})
        find_path (MINIGUI_INCLUDE_DIRS ${MINIGUI_HEADS} "${MINIGUI_LIBRARY_DIRS}../included")
    endif()

    # find the library.
    if (MINIGUI_INCLUDE_DIRS AND MINIGUI_LIBRARY)
        set (MINIGUI_FOUND TRUE)
        if (NOT MINIGUI_LIBRARY_DIRS)
            string (REGEX REPLACE "[a-z|A-Z|0-9|_|-]+[.].*$" "" MINIGUI_LIBRARY_DIRS ${MINIGUI_LIBRARY})
        endif ()
        string (REGEX MATCH "[a-z|A-Z|0-9|_|-]+[.](a|so|lib|dll|LIB|DLL)[^/]*$" MINIGUI_LIBRARY_WHOLE_NAME "${MINIGUI_LIBRARY}")
        string (REGEX MATCH "[^(lib)][a-z|A-Z|0-9|_|-]+" MINIGUI_LIBRARIES ${MINIGUI_LIBRARY_WHOLE_NAME})
        message (STATUS "Find MINIGUI include in ${MINIGUI_INCLUDE_DIRS}")
        message (STATUS "Find MINIGUI library in ${MINIGUI_LIBRARY}")
    else ()
        message (STATUS "Could NOT find MINIGUI, (missing: MINIGUI_INCLUDE_DIRS MINIGUI_LIBRARY)")
    endif ()

else ()
        message(STATUS "MiniGUI is being found:")
        message(STATUS "  -I: ${MINIGUI_INCLUDE_DIRS}")
        message(STATUS "  -L: ${MINIGUI_LIBRARY_DIRS}")
        message(STATUS "  -l: ${MINIGUI_LIBRARIES}")
endif ()

