cmake_minimum_required(VERSION 3.28)

project(livox_mapping)
message(STATUS "Project Dir: " ${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# LVX_RECORDDER

set(LVX_RECORDDER lvx_recorder)

add_executable(${LVX_RECORDDER}
    src/lvx.t.hpp
    src/lvx_recorder.cpp
)

target_include_directories(${LVX_RECORDDER}
    PUBLIC
        inc
)

target_link_libraries(${LVX_RECORDDER}
    livox_sdk_static    
)

# LVX_TO_LAS

set(LVX_TO_LAS lvx_to_las)

add_executable(${LVX_TO_LAS}
    src/lvx.t.hpp
    src/las.t.hpp
    src/lvx_to_las.cpp
)

target_include_directories(${LVX_TO_LAS}
    PUBLIC
        inc
)
