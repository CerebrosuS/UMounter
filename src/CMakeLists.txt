set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")

find_package(GLIB2 REQUIRED)

include_directories(${GLIB2_INCLUDE_DIRS})

set(SOURCES main.c automounter.c config.c volume.c volumes.c)
add_executable(umounter ${SOURCES})

target_link_libraries(umounter ${GLIB2_LIBRARIES})

# Installing...

install(TARGETS umounter RUNTIME DESTINATION bin)

