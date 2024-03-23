cmake_minimum_required(VERSION 3.1)

project(ftp)

FILE(GLOB SRC "src/*.c"
              "src/raw_handler/*.c"
    )
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -g)

add_executable(main ${SRC})

target_include_directories(main PRIVATE include)