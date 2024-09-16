#############################################################################################
# ZLib
# Free, general-purpose, legally unencumbered lossless data-compression library
# for use on virtually any computer hardware and operating system
# needed by KUserFeedback
# https://www.zlib.net/
#############################################################################################
cmake_minimum_required(VERSION 3.24)

message(STATUS "building ZLIB")

file(ARCHIVE_EXTRACT 
    INPUT ${CMAKE_CURRENT_SOURCE_DIR}/zlib-1.3.1.tar.gz
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

ExternalProject_Add(
    zlib_make
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/zlib-1.3.1
    BUILD_IN_SOURCE ON
    CONFIGURE_COMMAND
        ${cmd_setenv}
        ${CMAKE_COMMAND} .
            -GNinja
            -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
            -DCMAKE_BUILD_TYPE=Release
            -DCMAKE_INSTALL_PREFIX=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
    BUILD_COMMAND ${CMAKE_COMMAND} --build .
    INSTALL_COMMAND ${CMAKE_COMMAND} --install .
)
run_install_name_tool(zlib_make libz)
