cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(POLICY CMP0092)
    cmake_policy(SET CMP0092 NEW) # Don't add -W3 warning level by default.
endif()


file(READ "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION_FILE_CONTENTS)
string(STRIP "${VERSION_FILE_CONTENTS}" VERSION_FILE_CONTENTS)
set(DYNCALL_VERSION "${VERSION_FILE_CONTENTS}" CACHE STRING "Set version name")
set(DYNCALL_PACKAGE "dyncall" CACHE STRING "Set package name")
string(REPLACE "v" "" DYNCALL_VERSION "${DYNCALL_VERSION}")
string(REGEX REPLACE "[.+-]" ";" DYNCALL_VERSION_LIST ${DYNCALL_VERSION})
list(GET DYNCALL_VERSION_LIST 0 DYNCALL_VERSION_MAJOR)
list(GET DYNCALL_VERSION_LIST 1 DYNCALL_VERSION_MINOR)
list(GET DYNCALL_VERSION_LIST 2 DYNCALL_VERSION_PATCH)

project(dync 
        VERSION "${DYNCALL_VERSION_MAJOR}.${DYNCALL_VERSION_MINOR}.${DYNCALL_VERSION_PATCH}"
        DESCRIPTION "DynCall Plugin" 
        HOMEPAGE_URL "https://github.com/untrustedmodders/dyncall" 
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

#
# Format
#
include(CompatFormat)

#
# Dyncall
#
include(FetchDyncall)

#
# Plugin
#
file(GLOB_RECURSE PLUGIN_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

add_library(${PROJECT_NAME} SHARED ${PLUGIN_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} external/dyncall/include)

target_link_libraries(${PROJECT_NAME} PRIVATE dyncall_s)
include_directories(${CMAKE_BINARY_DIR}/_deps/dyncall-src/)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wextra -Wshadow -Wconversion -Wpedantic -Werror)
endif()

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME} EXPORT_MACRO_NAME PLUGIN_API EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/plugin_export.h)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/exports)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "dyncall")

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-Wl,-exported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/sym/exported_symbols.lds")
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-Wl,--version-script,${CMAKE_CURRENT_SOURCE_DIR}/sym/version_script.lds")
endif()

if(LINUX)
    target_link_libraries(${PROJECT_NAME} PRIVATE -static-libstdc++ -static-libgcc)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE 
        PLUGIFY_FORMAT_SUPPORT=$<BOOL:${COMPILER_SUPPORTS_FORMAT}> 
        PLUGIFY_IS_DEBUG=$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>)
if(NOT COMPILER_SUPPORTS_FORMAT)
    target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt-header-only)
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/dyncall.pplugin.in
    ${CMAKE_CURRENT_BINARY_DIR}/dyncall.pplugin
)