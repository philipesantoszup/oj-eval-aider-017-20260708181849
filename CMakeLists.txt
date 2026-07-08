cmake_minimum_required(VERSION 3.10)
project(TrainTicketSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(code main.cpp system.cpp)

target_compile_options(code PRIVATE -O3 -Wall -Wextra)
