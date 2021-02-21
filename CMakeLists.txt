cmake_minimum_required(VERSION 3.15)
project(code)

set(CMAKE_CXX_STANDARD 14)

add_executable(code main.cpp
        "blocklist.cpp" "blocklist.h"
        book.cpp book.h
        error.cpp error.h
        program.cpp program.h
        user.cpp user.h
        )

#add_executable(testfunc testfunc.cpp)