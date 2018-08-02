cmake_minimum_required(VERSION 2.8.3)
project(syft)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_BUILD_TYPE Debug)

set(CMAKE_MODULE_PATH
    "${CMAKE_MODULE_PATH}"
    "${CMAKE_ROOT_DIR}/cmake/Modules"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")
  
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

find_package(catkin REQUIRED)

find_package(cudd REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

catkin_package(
  LIBRARIES ${PROJECT_NAME} 
)

add_subdirectory(src)
