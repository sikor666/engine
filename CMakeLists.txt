cmake_minimum_required(VERSION 3.10)

set(PROJECT_VERSION "1.0.0")

project(engine VERSION ${PROJECT_VERSION} DESCRIPTION "game engine")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(SDL2 2.0.8 REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/glad)

add_library(${PROJECT_NAME} SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/engine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/exported.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/glad.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/glad.h
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindSDL2.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/Dockerfile)

# This will name engine output .so files "libsomething.1.0.0" which is pretty useful
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION})

target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARY})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
