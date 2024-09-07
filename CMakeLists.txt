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
    src/lvx.cpp
    src/lvx_recorder.cpp
)

target_include_directories(${LVX_RECORDDER}
    PUBLIC
        inc
)

# livox_sdk_static : https://github.com/Livox-SDK/Livox-SDK/commit/9306596a2bf15c1343bc023b497465ed0a32909d
target_link_libraries(${LVX_RECORDDER}
    livox_sdk_static    
)