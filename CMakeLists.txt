cmake_minimum_required(VERSION 3.16)
project(proc-inspect LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Header-only library.
add_library(procinspect INTERFACE)
target_include_directories(procinspect INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# CLI executable.
add_executable(proc-inspect src/main.cpp)
target_link_libraries(proc-inspect PRIVATE procinspect)
