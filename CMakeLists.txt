cmake_minimum_required(VERSION 3.5)

project(monopoly VERSION 0.1.0 LANGUAGES C)

add_executable(monopoly src/z6.c src/monopoly.c)

target_include_directories(monopoly PRIVATE include)

find_library(PDCURSES_LIBRARY NAMES pdcurses PATHS "C:/MinGW/lib" NO_DEFAULT_PATH)

if(PDCURSES_LIBRARY)
    message(STATUS "Library pdcurses found: ${PDCURSES_LIBRARY}")
    target_link_libraries(monopoly ${PDCURSES_LIBRARY})
else()
    message(FATAL_ERROR "Error finding pdcurses library")
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
