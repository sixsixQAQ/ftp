cmake_minimum_required(VERSION 3.20)

project(FreeFTPPublic)

FILE(GLOB SRC "src/*.cpp")

set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -g)

add_library(${PROJECT_NAME} STATIC ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE include)