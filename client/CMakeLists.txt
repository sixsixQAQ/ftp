cmake_minimum_required(VERSION 3.20)

project(client)

FILE(GLOB SRC "src/*.cpp")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(${PROJECT_NAME} ${SRC})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -g)
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    "${CMAKE_SOURCE_DIR}/public/include"
)
target_link_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/lib")
target_link_libraries(${PROJECT_NAME} PRIVATE FreeFTPPublic)