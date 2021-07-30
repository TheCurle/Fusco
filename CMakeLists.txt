#project config

cmake_minimum_required(VERSION 3.10)

project(fusco)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB src_files
    "src/ast/*.cpp",
    "src/interpreter/*.cpp",
    "src/lexer/*.cpp",
    "src/parser/*.cpp",
    "src/*.cpp",
)

if(WIN32)

else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Werror -Wall -Wextra -O0 -static")
endif()

include_directories("inc")

add_executable(fusco ${src_files})