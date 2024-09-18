#############################################################################################
# KUserFeedback
# Framework for collecting feedback from application users via telemetry and targeted surveys.
# https://api.kde.org/frameworks/kuserfeedback/html/index.html
# https://invent.kde.org/libraries/kuserfeedback/-/tags for the download listing
#############################################################################################

cmake_minimum_required(VERSION 3.24)

message(STATUS "building KUserFeedback")

set(c_archiveFileName kuserfeedback-v6.5.0)

file(ARCHIVE_EXTRACT 
    INPUT ${CMAKE_CURRENT_SOURCE_DIR}/${c_archiveFileName}.tar.gz 
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

set(c_kuf_args ${c_kde_cmakeArgs})
list(APPEND c_kuf_args
    -DENABLE_DOCS=OFF
    -DENABLE_CONSOLE=OFF
    -DENABLE_CLI=OFF
    -DENABLE_PHP=OFF
    -DENABLE_PHP_UNIT=OFF
    -DENABLE_SURVEY_TARGET_EXPRESSIONS=OFF
)

ExternalProject_Add(kuserfeedback_make
    DEPENDS c_extra_modules
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${c_archiveFileName}
    BUILD_IN_SOURCE ON
    LIST_SEPARATOR |
    CMAKE_ARGS ${c_kuf_args}
)

add_dependencies(${CMAKE_PROJECT_NAME} kuserfeedback_make)
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
    BA_ENABLE_KUSERFEEDBACK
)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include/KF6/KUserFeedback"
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include/KF6/KUserFeedbackCore"
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include/KF6/KUserFeedbackWidgets"
)

set(kfNameCore "KF6UserFeedbackCore")
set(kfNameWidgets "KF6UserFeedbackWidgets")
if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(kfNameCore "${kfNameCore}d")
        set(kfNameWidgets "${kfNameWidgets}d")
    endif()
elseif(APPLE)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(kfNameCore "${kfNameCore}_debug")
        set(kfNameWidgets "${kfNameWidgets}_debug")
    endif()
endif()

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${kfNameCore})
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${kfNameWidgets})
list(APPEND cList_libs_to_add ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/bin/${kfNameCore}${CMAKE_SHARED_LIBRARY_SUFFIX})
list(APPEND cList_libs_to_add ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/bin/${kfNameWidgets}${CMAKE_SHARED_LIBRARY_SUFFIX})
