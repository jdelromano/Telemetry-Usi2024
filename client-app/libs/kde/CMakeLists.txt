#############################################################################################
# KDE frameworks
# https://api.kde.org/
#############################################################################################

cmake_minimum_required(VERSION 3.24)

set(c_kde_cmakeArgs
    -GNinja
    -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
)

#############################################################################################
# KDE Extra CMake Modules (ECM)
# https://api.kde.org/
# Adds to the modules provided by CMake, including ones used by find_package()
# to find common software, and common build settings used in software produced by the
# KDE community.
# https://github.com/KDE/extra-cmake-modules/tags
#############################################################################################

set(c_extraModules_name extra-cmake-modules-6.5.0)

file(ARCHIVE_EXTRACT 
    INPUT ${CMAKE_CURRENT_SOURCE_DIR}/${c_extraModules_name}.tar.gz 
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

ExternalProject_Add(
    c_extra_modules
    DEPENDS zlib_make
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${c_extraModules_name}
    BUILD_IN_SOURCE ON
    LIST_SEPARATOR |
    CMAKE_ARGS
        ${c_kde_cmakeArgs}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_LIBRARY_SOURCES}/extra-cmake-modules
        -DBUILD_TESTING=OFF
)


#add c_extra_modules to the prefix path
set(t_kde_prefixPath ${CMAKE_PREFIX_PATH})
list(APPEND t_kde_prefixPath
    ${CMAKE_LIBRARY_SOURCES}/extra-cmake-modules
)
string(REPLACE ";" "|" t_kde_prefixPath "${t_kde_prefixPath}")

#set the common cmake args for KDE projects
list(APPEND c_kde_cmakeArgs
    -DQT_MAJOR_VERSION=${QT_DEFAULT_MAJOR_VERSION}
    -DCMAKE_PREFIX_PATH=${t_kde_prefixPath}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    -DZLIB_INCLUDE_DIR=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include
    -DBUILD_TESTING=OFF
)
if(APPLE)
    string(REPLACE ";" "|" t_arch "${CMAKE_OSX_ARCHITECTURES}")
    list(APPEND c_kde_cmakeArgs
        -DCMAKE_OSX_ARCHITECTURES=${t_arch}
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
    )
endif()

add_subdirectory(KUserFeedback)
