cmake_minimum_required(VERSION 3.16)
project(stereo_matching_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs)

add_library(stereo_matching SHARED IMPORTED)
set_target_properties(stereo_matching PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libstereo_matching.so"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(stereo_matching_demo demo/demo.cpp)
target_compile_options(stereo_matching_demo PRIVATE
    -O3 -Wall -Wextra -Wpedantic)
target_include_directories(stereo_matching_demo PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(stereo_matching_demo PRIVATE
    stereo_matching ${OpenCV_LIBS})
set_target_properties(stereo_matching_demo PROPERTIES
    BUILD_RPATH "$ORIGIN/../lib")
