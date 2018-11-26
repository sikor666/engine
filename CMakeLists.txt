cmake_minimum_required(VERSION 3.10)

set(PROJECT_VERSION "1.0.0")

project(engine VERSION ${PROJECT_VERSION} DESCRIPTION "game engine")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(SDL2 2.0.8 REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/glm)

file(GLOB_RECURSE SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

file(GLOB_RECURSE SRC_SHADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/media/shaders/*.glsl"
)

file(GLOB_RECURSE SRC_EXTERNALS
    "${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ext/sb7/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ext/sb7/*.h"
)

add_library(${PROJECT_NAME} SHARED
    ${SRC}
    ${SRC_SHADERS}
    ${SRC_EXTERNALS})

source_group("Sources"        FILES ${SRC})
source_group("Shaders"        FILES ${SRC_SHADERS})
source_group("Externals"      FILES ${SRC_EXTERNALS})

# This will name engine output .so files "libsomething.1.0.0" which is pretty useful
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION})

target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARY})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMENT "Copy engine resources and dependencies"
                   COMMAND cmake -E copy_directory
                        ${CMAKE_CURRENT_SOURCE_DIR}/media
                        ${CMAKE_BINARY_DIR}/media)
