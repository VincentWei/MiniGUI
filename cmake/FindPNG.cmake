# - This module determines the png library of the system
# The following variables are set if the library found:
# PNG_FOUND - If false do nnt try to use png.
# PNG_INCLUDE_DIR - where to find the headfile of library.
# PNG_LIBRARY_DIR - where to find the png library.
# PNG_LIBRARY - the library needed to use png.

# find the headfile of library
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ./)
include (FindZLIB)

if (ZLIB_FOUND)
set (PNG_HEADS png.h)
find_path (PNG_INCLUDE_DIR ${PNG_HEADS})

set (PNG_NAMES png libpng png12 libpng12)
find_library (PNG_LIBRARY NAMES ${PNG_NAMES})

# just find one of dependency, guess other one.
if (NOT PNG_LIBRARY AND PNG_INCLUDE_DIR)
	message ("We just find the headfile, try to guess the library location.")
	set (PNG_LIBRARY_DIR "${PNG_INCLUDE_DIR}/../lib")
	find_library (PNG_LIBRARY NAMES ${PNG_NAMES} PATHS ${PNG_LIBRARY_DIR})
elseif (NOT PNG_INCLUDE_DIR AND PNG_LIBRARY)
	message ("We just find the lib file, try to guess the include location.")
	string (REGEX REPLACE "[a-z|A-Z|0-9|_|-]+[.].*$" "" PNG_LIBRARY_DIR ${PNG_LIBRARY})
	find_path (PNG_INCLUDE_DIR ${PNG_HEADS} "${PNG_LIBRARY_DIR}../included")
endif()

# find the library.
if (PNG_INCLUDE_DIR AND PNG_LIBRARY)
	set (PNG_FOUND TRUE)
	if (NOT PNG_LIBRARY_DIR)
		string (REGEX REPLACE "[a-z|A-Z|0-9|_|-]+[.].*$" "" PNG_LIBRARY_DIR ${PNG_LIBRARY})
	endif ()
    string (REGEX MATCH "[a-z|A-Z|0-9|_|-]+[.](a|so|lib|dll|LIB|DLL)[^/]*$" PNG_LIBRARY_WHOLE_NAME "${PNG_LIBRARY}")
    string (REGEX MATCH "[^(lib)][a-z|A-Z|0-9|_|-]+" PNG_LIBRARY_NAME ${PNG_LIBRARY_WHOLE_NAME})
	message (STATUS "Find PNG include in ${PNG_INCLUDE_DIR}")
	message (STATUS "Find PNG library in ${PNG_LIBRARY}")
else ()
	message (STATUS "Could NOT find PNG, (missing: PNG_INCLUDE_DIR PNG_LIBRARY)")
endif ()

endif (ZLIB_FOUND)
