project(client_gui)

FILE(GLOB SRC "src/*.cpp")
FILE(GLOB INC "include/*.hpp")
FILE(GLOB UI  "ui/*.ui")

set(CMAKE_PREFIX_PATH "/opt/Qt/6.3.2/gcc_64")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS "ui")

add_executable(${PROJECT_NAME} ${SRC} ${INC} ${UI})

find_package(Qt6 COMPONENTS Widgets Network REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE "include")
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Network)