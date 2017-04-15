
cmake_minimum_required(VERSION 2.8)

project(virtual_viewpoint)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -pthread -std=c++11 -O0 -g -ggdb")


enable_testing()

configure_file(
	${CMAKE_SOURCE_DIR}/project_name_config.h.in
	${CMAKE_BINARY_DIR}/project_name_config.h
	)
include_directories(${CMAKE_BINARY_DIR})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory(include inc)
add_subdireictory(src lib)
add_subdirectory(test)
add_subdirectory(work)
# add_subdirectory(tool)
