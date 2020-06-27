cmake_minimum_required(VERSION 3.0)

project(simMaze)
set (CMAKE_BUILD_TYPE Release)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++17 -Wall")

find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(simMaze  main.cpp simMaze.cpp maze.cpp
                        solverMaze.cpp utils.cpp)

target_link_libraries(simMaze ${OpenCV_LIBS} pthread )
