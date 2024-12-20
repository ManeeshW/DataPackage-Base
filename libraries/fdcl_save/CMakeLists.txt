cmake_minimum_required (VERSION 3.1)
project(FDCL-SAVE)

set (CMAKE_CXX_STANDARD 11)

# link_directories("/usr/local/lib/")
# include_directories("/usr/local/include/")

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/libraries/eigen)


set(fdcl_save_src
    src/save.cpp
)
add_library(fdcl_save STATIC ${fdcl_save_src})

add_executable(test_fdcl_save src/test_fdcl_save.cpp)
target_compile_options(test_fdcl_save
    PRIVATE -Wall -O3 -std=c++11
)
target_link_libraries(test_fdcl_save
    fdcl_save
)

