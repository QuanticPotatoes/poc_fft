cmake_minimum_required(VERSION 3.1.1) # CMake version check
project(poc_fft)               # Create project "simple_example"

set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ") # Enable c++11 standard
include_directories(include)
set(SOURCE_FILES main.cpp png.cpp)            # Add main.cpp file of project root directory as source file
file(COPY input.wav DESTINATION .)
add_executable(poc_fft ${SOURCE_FILES})       # Add executable target with source files listed in SOURCE_FILES variable
target_link_libraries(poc_fft fftw3 sndfile png) # Add library to poc_fft
