cmake_minimum_required(VERSION 3.10)

project(livox_mapping)
message(STATUS "main project dir: " ${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        include
)

target_sources(${PROJECT_NAME}
    PUBLIC
        src/lvx.cpp
)

target_link_libraries(${PROJECT_NAME}
    livox_sdk_static    
)