cmake_minimum_required(VERSION 3.31.6)
project("libarena" C)
include(CTest)

option(TEST "Enable tests" OFF)

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Version
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DARENA_VERSION='\"${GIT_COMMIT_HASH}\"'")

# C standard
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700 -D_FILE_OFFSET_BITS=64")

# Warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-missing-field-initializers")

function(Build_Library)
    add_subdirectory(src)
    include_directories(src)
endfunction()

function(Build_Tests)
    add_subdirectory(external)
    add_subdirectory(test)
endfunction()

Build_Library()
if(TEST)
    Build_Tests()
endif()
