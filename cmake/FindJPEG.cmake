# - This module determines the jpeg library of the system
# The following variables are set if the library found:
# JPEG_FOUND - If false do nnt try to use jpeg.
# JPEG_INCLUDE_DIR - where to find the headfile of library.
# JPEG_LIBRARY_DIR - where to find the jpeg library.
# JPEG_LIBRARY - the library needed to use jpeg.

# find the headfile of library

set (JPEG_HEADS jpeglib.h)
find_path (JPEG_INCLUDE_DIR ${JPEG_HEADS})

set (JPEG_NAMES jpeg libjpeg)
find_library (JPEG_LIBRARY NAMES ${JPEG_NAMES})

# just find one of dependency, guess other one.
if (NOT JPEG_LIBRARY AND JPEG_INCLUDE_DIR)
	message ("We just find the headfile, try to guess the library location.")
	set (JPEG_LIBRARY_DIR "${JPEG_INCLUDE_DIR}/../lib")
	find_library (JPEG_LIBRARY NAMES ${JPEG_NAMES} PATHS ${JPEG_LIBRARY_DIR})
elseif (NOT JPEG_INCLUDE_DIR AND JPEG_LIBRARY)
	message ("We just find the lib file, try to guess the include location.")
	string (REGEX REPLACE "[a-z|A-Z|0-9|_|-]+[.].*$" "" JPEG_LIBRARY_DIR ${JPEG_LIBRARY})
	find_path (JPEG_INCLUDE_DIR ${JPEG_HEADS} "${JPEG_LIBRARY_DIR}../included")
endif()

# find the library.
if (JPEG_INCLUDE_DIR AND JPEG_LIBRARY)
	set (JPEG_FOUND TRUE)
	if (NOT JPEG_LIBRARY_DIR)
		string (REGEX REPLACE "[a-z|A-Z|0-9|_|-]+[.].*$" "" JPEG_LIBRARY_DIR ${JPEG_LIBRARY})
	endif ()
    string (REGEX MATCH "[a-z|A-Z|0-9|_|-]+[.](a|so|lib|dll|LIB|DLL)[^/]*$" JPEG_LIBRARY_WHOLE_NAME "${JPEG_LIBRARY}")
    string (REGEX MATCH "[^(lib)][a-z|A-Z|0-9|_|-]+" JPEG_LIBRARY_NAME ${JPEG_LIBRARY_WHOLE_NAME})
	message (STATUS "Find JPEG include in ${JPEG_INCLUDE_DIR}")
	message (STATUS "Find JPEG library in ${JPEG_LIBRARY}")
else ()
	message (STATUS "Could NOT find JPEG, (missing: JPEG_INCLUDE_DIR JPEG_LIBRARY)")
endif ()

