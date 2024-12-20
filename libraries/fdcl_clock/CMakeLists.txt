cmake_minimum_required (VERSION 3.1)
project(FDCL-Clock)

set (CMAKE_CXX_STANDARD 11)

link_directories("/usr/local/lib/")
include_directories("/usr/local/include/")

include_directories(${PROJECT_SOURCE_DIR}/include)

## Compile FDCL-Clock Project. #################################################
set(fdcl_clock_src
    ${PROJECT_SOURCE_DIR}/src/fdcl_clock.cpp
)
add_library(fdcl_clock STATIC ${fdcl_clock_src})


## Compile example. ############################################################
set(fdcl_clock_src
    src/test_fdcl_clock.cpp
)
add_executable(test_fdcl_clock ${fdcl_clock_src})
target_link_libraries(test_fdcl_clock
    fdcl_clock
)
target_compile_options(test_fdcl_clock
  PUBLIC -Wall -lrt -lm -O3 -std=c++11
)
