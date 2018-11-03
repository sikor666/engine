# Find SDL2
# ---------
#
# Finds the SDL2 library. This module defines:
#
#  SDL2_FOUND               - True if SDL2 library is found
#  SDL2                     - SDL2 imported target
#
# Additionally these variables are defined for internal usage:
#
#  SDL2_LIBRARY_DEBUG       - SDL2 debug library, if found
#  SDL2_LIBRARY_RELEASE     - SDL2 release library, if found
#  SDL2_INCLUDE_DIR         - Root include dir
#

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_SDL_LIBRARY_PATH_SUFFIX "lib/x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(_SDL_LIBRARY_PATH_SUFFIX "lib/x86")
endif()

find_library(SDL2_LIBRARY_RELEASE
    NAMES "SDL2"
    PATH_SUFFIXES ${_SDL_LIBRARY_PATH_SUFFIX})
find_library(SDL2_LIBRARY_DEBUG
    NAMES "SDL2d"
    PATH_SUFFIXES ${_SDL_LIBRARY_PATH_SUFFIX})

include(SelectLibraryConfigurations)
select_library_configurations(SDL2)

find_path(SDL2_INCLUDE_DIR
    NAMES "SDL_scancode.h"
    PATH_SUFFIXES "SDL2")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2 SDL2_FOUND SDL2_LIBRARY_RELEASE SDL2_INCLUDE_DIR)

mark_as_advanced(SDL2_LIBRARY_RELEASE SDL2_INCLUDE_DIR)

if(SDL2_FOUND)
    if(NOT SDL2_FIND_QUIETLY)
    endif()
else()
    if(SDL2_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find SDL2")
    else()
        message(STATUS "Optional package SDL2 was not found")
    endif()
endif()
