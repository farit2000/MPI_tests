cmake_minimum_required(VERSION 3.17)
project(mpi_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

include_directories("/usr/local/Cellar/mpich/3.3.2_1/include/")

set(CMAKE_C_COMPILER "/usr/local/Cellar/mpich/3.3.2_1/bin/mpicc")

set(CMAKE_CXX_COMPILER "/usr/local/Cellar/mpich/3.3.2_1/bin/mpicxx")
set(CMAKE_CXX_STANDARD 20)
#cmake_minimum_required(VERSION 3.6)
add_executable(mpi_test main.cpp)