cmake_minimum_required(VERSION 3.20)

project(server)

FILE(GLOB SRC "src/*.cpp")
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -g)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE include)