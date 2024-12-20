cmake_minimum_required(VERSION 2.8.4)
project(vn200_test)

set(CMAKE_SUPPRESS_REGENERATION TRUE)

add_subdirectory(../cpp libvncxx)

if(CMAKE_COMPILER_ID MATCHES "GNU|Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(SOURCE_FILES
    src/main.cpp)

include_directories(../cpp/include)

add_executable(vn200_test ${SOURCE_FILES})

target_link_libraries(vn200_test PRIVATE libvncxx)

if (UNIX OR APPLE)
	target_link_libraries(vn200_test LINK_PUBLIC pthread)
else()
	target_link_libraries(vn200_test LINK_PUBLIC Advapi32 SetupAPI)
endif()
