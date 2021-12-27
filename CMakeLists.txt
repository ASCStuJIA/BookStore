cmake_minimum_required(VERSION 3.20)
project(bookstore2)

set(CMAKE_CXX_STANDARD 14)

add_executable(bookstore2 main.cpp
        bookstore.cpp
        blocklist.cpp
        usermanager.cpp)
