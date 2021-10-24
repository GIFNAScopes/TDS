
# - Try to find NIVISA instalation

find_path(VISA_INCLUDE_DIRS visa.h
    HINTS /usr/include/ni-visa/ /usr/include/ /usr/local/include)


find_library(VISA_LIBRARIES NAMES libvisa.so visa
    HINTS /usr/lib/x86_64-linux-gnu /usr/lib /usr/lib/64)


if (VISA_INCLUDE_DIRS AND VISA_LIBRARIES)
    set (VISA_FOUND TRUE)
endif()

if (NOT VISA_FOUND)
    message(FATAL_ERROR "VISA required, but not found")
else()
	message(STATUS "Find VISA include ${VISA_INCLUDE_DIRS} and libs ${VISA_LIBRARIES}")
endif()
