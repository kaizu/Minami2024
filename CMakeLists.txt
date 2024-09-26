cmake_minimum_required(VERSION 3.13)
project(test_cmake CXX)

find_package(GSL REQUIRED)
include_directories({${GSL_INCLUDE_DIRS})

file(GLOB CPP_FILES bd/*.cpp)

add_executable(a.out main.cpp ${CPP_FILES})

target_compile_options(a.out PUBLIC -O3)
target_link_libraries(a.out ${GSL_LIBRARIES})
