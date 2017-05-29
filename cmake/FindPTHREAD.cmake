# - This module determines the pthread library of the system
# The following variables are set if the library found:
# PTHREAD_FOUND - If false do nnt try to use pthread.
# PTHREAD_INCLUDE_DIR - where to find the headfile of library.
# PTHREAD_LIBRARY_DIR - where to find the pthread library.
# PTHREAD_LIBRARY - the library needed to use pthread.

# find the headfile of library
set (PTHREAD_HEADS pthread.h)
find_path (PTHREAD_INCLUDE_DIR ${PTHREAD_HEADS})

set (PTHREAD_NAMES pthread libpthread pthreadVC)
find_library (PTHREAD_LIBRARY NAMES ${PTHREAD_NAMES})

# just find one of dependency, guess other one.
if (NOT PTHREAD_LIBRARY AND PTHREAD_INCLUDE_DIR)
	message ("We just find the headfile, try to guess the library location.")
	set (PTHREAD_LIBRARY_DIR "${PTHREAD_INCLUDE_DIR}/../lib")
	find_library (PTHREAD_LIBRARY NAMES ${PTHREAD_NAMES} PATHS ${PTHREAD_LIBRARY_DIR})
elseif (NOT PTHREAD_INCLUDE_DIR AND PTHREAD_LIBRARY)
	message ("We just find the lib file, try to guess the include location.")
	string (REGEX REPLACE "[a-z|A-Z|0-9|_|-]+[.].*$" "" PTHREAD_LIBRARY_DIR ${PTHREAD_LIBRARY})
	find_path (PTHREAD_INCLUDE_DIR ${PTHREAD_HEADS} "${PTHREAD_LIBRARY_DIR}../include")
endif()

# find the library.
if (PTHREAD_INCLUDE_DIR AND PTHREAD_LIBRARY)
	set (PTHREAD_FOUND TRUE)
	if (NOT PTHREAD_LIBRARY_DIR)
		string (REGEX REPLACE "[a-z|A-Z|0-9|_|-]+[.].*$" "" PTHREAD_LIBRARY_DIR ${PTHREAD_LIBRARY})
	endif ()
    string (REGEX MATCH "[a-z|A-Z|0-9|_|-]+[.](a|so|lib|dll|LIB|DLL)[^/]*$" PTHREAD_LIBRARY_WHOLE_NAME "${PTHREAD_LIBRARY}")
    string (REGEX MATCH "[^(lib)][a-z|A-Z|0-9|_|-]+" PTHREAD_LIBRARY_NAME ${PTHREAD_LIBRARY_WHOLE_NAME})
	message (STATUS "Find PTHREAD include in ${PTHREAD_INCLUDE_DIR}")
	message (STATUS "Find PTHREAD library in ${PTHREAD_LIBRARY}")
else ()
	message (STATUS "Could NOT find PTHREAD, (missing: PTHREAD_INCLUDE_DIR PTHREAD_LIBRARY)")
endif ()
