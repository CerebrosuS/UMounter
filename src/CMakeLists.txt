set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")

find_package(GLIB2 REQUIRED)
find_package(XML2 REQUIRED)

include_directories(${GLIB2_INCLUDE_DIRS})
include_directories(${XML2_INCLUDE_DIRS})

set(SOURCES main.c automounter.c config.c volume.c volumes.c rulesparser.c
    logging.c string.c)
add_executable(umounter ${SOURCES})

target_link_libraries(umounter ${GLIB2_LIBRARIES})
target_link_libraries(umounter ${XML2_LIBRARIES})

# Installing...

install(TARGETS umounter RUNTIME DESTINATION bin)

