cmake_minimum_required (VERSION 3.1)
project(FDCL-PARAM)

set (CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/libraries/eigen)


set(fdcl_param_src
    src/param.cpp
)
add_library(fdcl_param STATIC ${fdcl_param_src})

add_executable(test_fdcl_param src/test_fdcl_param.cpp)
target_compile_options(test_fdcl_param
    PRIVATE -Wall -O3 -std=c++11
)
target_link_libraries(test_fdcl_param
    fdcl_param
)

