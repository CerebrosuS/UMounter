# Script for cmake to find libxml2.
#
# Copyright (c) Christian Krippendorf 2011
#
# Published under the GPLv3.
#

FIND_PATH(XML2_INCLUDE_DIR
    NAMES 
        libxml
    PATHS 
        /usr/include
    PATH_SUFFIXES
        libxml2
)

SET(XML2_INCLUDE_DIRS ${XML2_INCLUDE_DIR})

FIND_LIBRARY(XML2_LIBRARY 
    NAMES
        xml2
    PATHS
        /lib
        /usr/lib
        /usr/local/lib
        /lib64
        /usr/lib64
        /usr/lib32
        /usr/local/lib32
)

SET(XML2_LIBRARIES ${XML2_LIBRARY})

IF(XML2_LIBRARIES AND XML2_INCLUDE_DIRS)
    SET(XML2_FOUND TRUE)
ELSE(XML2_LIBRARIES AND XML2_INCLUDE_DIRS)
    SET(XML2_FOUND FALSE)
ENDIF(XML2_LIBRARIES AND XML2_INCLUDE_DIRS)

IF(XML2_FOUND)
    IF(NOT XML2_FIND_QUIETLY)
        MESSAGE(STATUS "XML2 Includes:")
        FOREACH(_currentItem ${XML2_INCLUDE_DIRS})
            MESSAGE(STATUS " :: ${_currentItem}")
        ENDFOREACH(_currentItem)

        MESSAGE(STATUS "XML2 Libraries:")
        FOREACH(_currentItem ${XML2_LIBRARIES})
            MESSAGE(STATUS " :: ${_currentItem}")
        ENDFOREACH(_currentItem)
    ENDIF(NOT XML2_FIND_QUIETLY)
ELSE(XML2_FOUND)
    IF(XML2_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find XML2 libraries or includes.")
    ENDIF(XML2_FIND_REQUIRED)
ENDIF(XML2_FOUND)

MARK_AS_ADVANCED(XML2_LIBRARIES XML2_INCLUDE_DIRS)

