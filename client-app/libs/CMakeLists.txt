cmake_minimum_required(VERSION 3.21)

#############################################################################################
# Target for recompiling libraries
#############################################################################################
add_custom_target(BAN_additional_libs)
target_sources(BAN_additional_libs PRIVATE
    mac_install_name_tool.sh.in
    mac_setenv.sh.in
)

#############################################################################################
# General settings for libraries
#############################################################################################
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    message(FATAL_ERROR "variable CMAKE_LIBRARY_OUTPUT_DIRECTORY not set")
endif()
if(NOT EXISTS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
    file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib)
    file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/bin)
    file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include)
endif()

set(CMAKE_LIBRARY_TMP_SOURCES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/sources)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include
)
target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib
)

if(WIN32)
    #on MSVC 17.3.2 not working anymore.
    #if("${CMAKE_EXE_LINKER_FLAGS}" STREQUAL "/machine:x64")
        set(win_cpu_type x64)
    #elseif("${CMAKE_EXE_LINKER_FLAGS}" STREQUAL "/machine:ARM64")
    #    set(c_cpu_type arm64)
    #endif()

endif()

include(ExternalProject)

if(UNIX OR APPLE)
    find_program(cmd_make make)
    if (NOT cmd_make)
        message(FATAL_ERROR "make not found")
    endif ()
endif()

if(APPLE)
    #On APPLE wit Externalproject_add, you always have to call
    #configuration for the new M1 chip, this is done in the mac_setenv.sh script
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/mac_setenv.sh.in
        ${CMAKE_LIBRARY_TMP_SOURCES}/mac_setenv.sh
        @ONLY
        FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    )
    set(cmd_setenv
        /bin/sh ${CMAKE_LIBRARY_TMP_SOURCES}/mac_setenv.sh
    )
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/mac_install_name_tool.sh.in
        ${CMAKE_LIBRARY_TMP_SOURCES}/mac_install_name_tool.sh
        @ONLY
        FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    )
else()
    set(cmd_setenv "")
endif()


# Create a list with an alternate separator e.g. pipe symbol
string(REPLACE ";" "|" CMAKE_PREFIX_PATH_ALT_SEP "${CMAKE_PREFIX_PATH}")


#############################################################################################
# MACRO
# call install_name_tool after creation of an externalProject
# to change dynamic shared library install names after install
#############################################################################################
macro(run_install_name_tool a_extProjectName a_libName)
    if(APPLE)
        ExternalProject_Add_Step(${a_extProjectName} ${a_libName}_name_tool
            COMMAND /bin/sh ${CMAKE_LIBRARY_TMP_SOURCES}/mac_install_name_tool.sh ${a_libName}
            WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib
            DEPENDEES install
        )
    endif()
endmacro()


add_subdirectory(zlib)
add_subdirectory(kde)


#############################################################################################
# OPEN SSL
#############################################################################################
if(WIN32)
    set(t_openSSL_lib_name libssl-3-${win_cpu_type}${CMAKE_SHARED_LIBRARY_SUFFIX})
    set(t_openSSL_crypto_name libcrypto-3-${win_cpu_type}${CMAKE_SHARED_LIBRARY_SUFFIX})
    find_package(OpenSSL QUIET)
    if(OPENSSL_FOUND)
        #copy in the %path% (see .vs/launch.json)
        set(c_openSSLdllsDir ${OPENSSL_INCLUDE_DIR}/../bin)
    else()
        message(STATUS "OPENSSL -> Using project pre compiled libs")
        set(c_openSSLdllsDir ${CMAKE_CURRENT_SOURCE_DIR}/openssl/win/)
    endif()
    if(c_openSSLdllsDir AND CMAKE_INSTALL_LIBDIR)
        file(COPY_FILE
            ${c_openSSLdllsDir}/${t_openSSL_lib_name}
            ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/bin/${t_openSSL_lib_name}
            ONLY_IF_DIFFERENT
        )
        file(COPY_FILE
            ${c_openSSLdllsDir}/${t_openSSL_crypto_name}
            ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/bin/${t_openSSL_crypto_name}
            ONLY_IF_DIFFERENT
        )
    endif()
elseif(ANDROID)
    message(WARNING "Android needs OpenSSL to be distributed")
elseif(APPLE)
    #apple already provides ssl libs
elseif(UNIX)
    message(WARNING "OpenSSL needs to be distributed")
endif()

if(t_openSSL_lib_name AND CMAKE_INSTALL_LIBDIR)
    install(FILES
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/bin/${t_openSSL_lib_name}
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/bin/${t_openSSL_crypto_name}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

#############################################################################################
# System dependent libraries
#############################################################################################
if(APPLE)
    target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
        "-framework AppKit"
        "-framework Carbon"
        "-framework CoreFoundation"
        "-framework CoreServices"
    )
endif()

#############################################################################################
# Installing all the above libraries
#############################################################################################
if(WIN32 AND cList_libs_to_add)
    install(PROGRAMS
            ${cList_libs_to_add}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT System
    )
endif()
