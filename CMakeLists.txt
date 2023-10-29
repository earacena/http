cmake_minimum_required(VERSION 3.18...3.27)
project(
  http
  VERSION 0.1.0
  DESCRIPTION "An implementation of non-production HTTP server written in C++20."
  LANGUAGES CXX
)

# Main project 
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  # Ensure -std=c++20 instead of -std=g++20
  set(CMAKE_CXX_EXTENSIONS OFF)

  # Enable IDE folder support
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# Add source code directory
add_subdirectory(src)

# Add executable directory
add_subdirectory(apps)

