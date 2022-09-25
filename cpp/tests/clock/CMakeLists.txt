cmake_minimum_required(VERSION 3.13)    # CMake version check
set(PROJECT_NAME Clock-Test)
project(${PROJECT_NAME})                # Create project "Clock-Test" 
set(CMAKE_CXX_STANDARD 17)              # Enable c++17 standard (g++ 7-8)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(${PROJECT_NAME} clock.cpp)
