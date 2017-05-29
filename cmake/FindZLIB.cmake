# - This module determines the zlib library of the system
# The following variables are set if the library found:
# ZLIB_FOUND - If false do nnt try to use zlib.
# ZLIB_INCLUDE_DIR - where to find the headfile of library.
# ZLIB_LIBRARY_DIR - where to find the zlib library.
# ZLIB_LIBRARY - the library needed to use zlib.

# find the headfile of library
set (ZLIB_HEADS zlib.h)
find_path (ZLIB_INCLUDE_DIR ${ZLIB_HEADS})

set (ZLIB_NAMES z zlib libz zdll)
find_library (ZLIB_LIBRARY NAMES ${ZLIB_NAMES})

# just find one of dependency, guess other one.
if (NOT ZLIB_LIBRARY AND ZLIB_INCLUDE_DIR)
	message ("We just find the headfile, try to guess the library location.")
	set (ZLIB_LIBRARY_DIR "${ZLIB_INCLUDE_DIR}/../lib")
	find_library (ZLIB_LIBRARY NAMES ${ZLIB_NAMES} PATHS ${ZLIB_LIBRARY_DIR})
elseif (NOT ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)
	message ("We just find the lib file, try to guess the include location.")
	string (REGEX REPLACE "[a-z|A-Z|0-9|_|-]+[.].*$" "" ZLIB_LIBRARY_DIR ${ZLIB_LIBRARY})
	find_path (ZLIB_INCLUDE_DIR ${ZLIB_HEADS} "${ZLIB_LIBRARY_DIR}../included")
endif()

# find the library.
if (ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)
	set (ZLIB_FOUND TRUE)
	if (NOT ZLIB_LIBRARY_DIR)
		string (REGEX REPLACE "[a-z|A-Z|0-9|_|-]+[.].*$" "" ZLIB_LIBRARY_DIR ${ZLIB_LIBRARY})
	endif ()
    string (REGEX MATCH "[a-z|A-Z|0-9|_|-]+[.](a|so|lib|dll|LIB|DLL)[^/]*$" ZLIB_LIBRARY_WHOLE_NAME "${ZLIB_LIBRARY}")
    string (REGEX MATCH "[^(lib)][a-z|A-Z|0-9|_|-]*" ZLIB_LIBRARY_NAME ${ZLIB_LIBRARY_WHOLE_NAME})
	message (STATUS "Find ZLIB include in ${ZLIB_INCLUDE_DIR}")
	message (STATUS "Find ZLIB library in ${ZLIB_LIBRARY}")
else ()
	#set (ZLIB_FOUND FALSE)
	message (STATUS "Could NOT find ZLIB, (missing: ZLIB_INCLUDE_DIR ZLIB_LIBRARY)")
endif ()
