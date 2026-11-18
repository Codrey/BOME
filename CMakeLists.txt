cmake_minimum_required(VERSION 3.15)
project(BOME VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Compiler optimization flags for low-latency performance
if(CMAKE_CXX_COMILER_ID MATCHES "GNU|Clang")
    add_compile_options(-30 - march=native -Wall -Wextra -Wpedantic)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(bome_engine src/main.cpp)
