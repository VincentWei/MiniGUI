# - Locate FreeType library
# This module defines
#  FREETYPE_LIBRARIES, the library to link against
#  FREETYPE_FOUND, if false, do not try to link to FREETYPE
#  FREETYPE_INCLUDE_DIRS, where to find headers.
#  This is the concatenation of the paths:
#  FREETYPE_INCLUDE_DIR_ft2build
#  FREETYPE_INCLUDE_DIR_freetype2
#  FREETYPE_INCLUDE_DIR - where to find the headfile of library.
#  FREETYPE_LIBRARY_DIR - where to find the freetype library.
#  FREETYPE_LIBRARY_NAME - name of the freetype library.

# find the headfile of library
set (FT2BUILD_HEADS ft2build.h)
find_path(FREETYPE_INCLUDE_DIR_ft2build ${FT2BUILD_HEADS}
	HINTS
	$ENV{FREETYPE_DIR}
	)

set (FREETYPE2_HEADS freetype/config/ftheader.h)
find_path(FREETYPE_INCLUDE_DIR_freetype2 ${FREETYPE2_HEADS}
	HINTS
	$ENV{FREETYPE_DIR}/include/freetype2
    PATHS
    /usr/include/freetype2
	)

set (FREETYPE_NAMES freetype libfreetype)
find_library(FREETYPE_LIBRARY
	NAMES ${FREETYPE_NAMES}
	HINTS
	$ENV{FREETYPE_DIR}/lib
	)

# set the user variables
if(FREETYPE_INCLUDE_DIR_ft2build AND FREETYPE_INCLUDE_DIR_freetype2)
	set (FREETYPE_INCLUDE_DIRS "${FREETYPE_INCLUDE_DIR_ft2build};${FREETYPE_INCLUDE_DIR_freetype2}")
	set (FREETYPE_INCLUDE_DIR ${FREETYPE_INCLUDE_DIRS})
endif(FREETYPE_INCLUDE_DIR_ft2build AND FREETYPE_INCLUDE_DIR_freetype2)
set(FREETYPE_LIBRARIES "${FREETYPE_LIBRARY}")

# find the library.
if (FREETYPE_INCLUDE_DIR AND FREETYPE_LIBRARY)
	set (FREETYPE_FOUND TRUE)
	if (NOT FREETYPE_LIBRARY_DIR)
		string (REGEX REPLACE "/[a-z|A-Z|0-9|_|-]+[.](a|so|lib|dll|LIB|DLL)[^/]*$" "" FREETYPE_LIBRARY_DIR "${FREETYPE_LIBRARY}")
	endif ()
    string (REGEX MATCH "[a-z|A-Z|0-9|_|-]+[.](a|so|lib|dll|LIB|DLL)[^/]*$" FREETYPE_LIBRARY_WHOLE_NAME "${FREETYPE_LIBRARY}")
    string (REGEX MATCH "[^(lib)][a-z|A-Z|0-9|_|-]+" FREETYPE_LIBRARY_NAME ${FREETYPE_LIBRARY_WHOLE_NAME})
    message (STATUS "Find FREETYPE include in ${FREETYPE_INCLUDE_DIR}")
	message (STATUS "Find FREETYPE library in ${FREETYPE_LIBRARY}")
else ()
	message (STATUS "Could NOT find FREETYPE, (missing: FREETYPE_INCLUDE_DIR FREETYPE_LIBRARY)")
endif ()

