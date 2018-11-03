cmake_minimum_required(VERSION 3.10)

project(engine VERSION 1.0.0 DESCRIPTION "game engine")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Determine the operating system
message(STATUS "${CMAKE_SYSTEM_NAME}")
if(UNIX)
    if(APPLE)
    else()
    endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl2)
else()
    message(FATAL_ERROR "Unknown OS ${CMAKE_SYSTEM_NAME}")
endif()

find_package(SDL2 2.0.8 REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/glad)

add_library(${PROJECT_NAME} STATIC src/engine.cpp src/engine.h
			ext/glad/glad.c ext/glad/glad.h
			cmake/FindSDL2.cmake Dockerfile)

target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARY_RELEASE})
	
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        LIBRARY DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        ARCHIVE DESTINATION ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})