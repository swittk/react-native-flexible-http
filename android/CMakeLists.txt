cmake_minimum_required(VERSION 3.4.1)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 11)

# If this doesn't work maybe follow modifications from https://ospfranco.com/post/2021/02/24/how-to-create-a-javascript-jsi-module/

add_library(cpp
            SHARED
            ../cpp/react-native-flexible-http.cpp
            cpp-adapter.cpp
)

# Specifies a path to native header files.
include_directories(
            ../cpp
)
