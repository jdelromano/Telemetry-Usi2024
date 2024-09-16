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
run_install_name_tool(kuserfeedback_make libKUserFeedbackCore)
run_install_name_tool(kuserfeedback_make libKUserFeedbackWidgets)

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
    BA_ENABLE_KUSERFEEDBACK
)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include/KF6/KUserFeedback"
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include/KF6/KUserFeedbackCore"
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include/KF6/KUserFeedbackWidgets"
)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC KF6UserFeedbackCored)
    target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC KF6UserFeedbackWidgetsd)
    list(APPEND cList_libs_to_add ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/bin/KUserFeedbackCore${CMAKE_SHARED_LIBRARY_SUFFIX})
    list(APPEND cList_libs_to_add ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/bin/KUserFeedbackWidgets${CMAKE_SHARED_LIBRARY_SUFFIX})
else()

    target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC KUserFeedbackCore)
    target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC KUserFeedbackWidgets)
    list(APPEND cList_libs_to_add ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/bin/KUserFeedbackCore${CMAKE_SHARED_LIBRARY_SUFFIX})
    list(APPEND cList_libs_to_add ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/bin/KUserFeedbackWidgets${CMAKE_SHARED_LIBRARY_SUFFIX})
endif()
