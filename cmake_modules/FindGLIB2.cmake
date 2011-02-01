# Script for cmake to find glib2.
#
# Copyright (c) Christian Krippendorf 2011
#
# Published under the GPLv3.
#

# If the variables already exist, no need to search.

FIND_PATH(GLIB2_INCLUDE_DIR
    NAMES 
        glib.h
    PATHS 
        /usr/include
    PATH_SUFFIXES
        glib-2.0
)

FIND_PATH(GLIB2CONFIG_INCLUDE_DIR
    NAMES 
        glibconfig.h
    PATHS
        /usr/lib64
        /usr/lib/
        /usr/lib32
    PATH_SUFFIXES
        glib-2.0/include
)

IF(GLIB2_INCLUDE_DIR AND GLIB2CONFIG_INCLUDE_DIR)
    SET(GLIB2_INCLUDE_DIRS ${GLIB2_INCLUDE_DIR} ${GLIB2CONFIG_INCLUDE_DIR})
ENDIF(GLIB2_INCLUDE_DIR AND GLIB2CONFIG_INCLUDE_DIR)

FIND_LIBRARY(GLIB2_LIBRARY 
    NAMES 
        glib
        glib-2.0
    PATHS
        /lib
        /usr/lib
        /usr/local/lib
        /lib64
        /usr/lib64
        /usr/lib32
        /usr/local/lib32
)

FIND_LIBRARY(GLIB2GIO_LIBRARY 
    NAMES 
        gio
        gio-2.0
    PATHS
        /lib
        /usr/lib
        /usr/local/lib
        /usr/lib64
        /lib64
        /usr/lib32
        /usr/local/lib32
)

FIND_LIBRARY(GLIB2GOBJECT_LIBRARY 
    NAMES 
        gobject
        gobject-2.0
    PATHS
        /lib
        /usr/lib
        /usr/local/lib
        /usr/lib64
        /lib64
        /usr/lib32
        /usr/local/lib32
)

FIND_LIBRARY(GLIB2GTHREAD_LIBRARY 
    NAMES 
        gthread
        gthread-2.0
    PATHS
        /lib
        /usr/lib
        /usr/local/lib
        /usr/lib64
        /lib64
        /usr/lib32
        /usr/local/lib32
)

SET(GLIB2_LIBRARIES ${GLIB2_LIBRARY} ${GLIB2GIO_LIBRARY} 
    ${GLIB2GOBJECT_LIBRARY} ${GLIB2GTHREAD_LIBRARY})

IF(GLIB2_LIBRARIES AND GLIB2_INCLUDE_DIRS)
    SET(GLIB2_FOUND TRUE)
ELSE(GLIB2_LIBRARIES AND GLIB2_INCLUDE_DIRS)
    SET(GLIB2_FOUND FALSE)
ENDIF(GLIB2_LIBRARIES AND GLIB2_INCLUDE_DIRS)

IF(GLIB2_FOUND)
    IF(NOT GLIB2_FIND_QUIETLY)
        MESSAGE(STATUS "GLIB2 Includes:")
        FOREACH(_currentItem ${GLIB2_INCLUDE_DIRS})
            MESSAGE(STATUS " :: ${_currentItem}")
        ENDFOREACH(_currentItem)

        MESSAGE(STATUS "GLIB2 Libraries:")
        FOREACH(_currentItem ${GLIB2_LIBRARIES})
            MESSAGE(STATUS " :: ${_currentItem}")
        ENDFOREACH(_currentItem)
    ENDIF(NOT GLIB2_FIND_QUIETLY)
ELSE(GLIB2_FOUND)
    IF(GLIB2_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find GLIB2 libraries or includes.")
    ENDIF(GLIB2_FIND_REQUIRED)
ENDIF(GLIB2_FOUND)

MARK_AS_ADVANCED(GLIB2_LIBRARIES GLIB2_INCLUDE_DIRS)

