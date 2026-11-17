cmake_minimum_required(VERSION 3.16)
project(CPUBoostAI VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(cpuboost-ai
    src/main.cpp
    src/cpu_info.cpp
)

target_include_directories(cpuboost-ai PRIVATE src)

if(MSVC)
    target_compile_options(cpuboost-ai PRIVATE /W4 /permissive-)
else()
    target_compile_options(cpuboost-ai PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS cpuboost-ai RUNTIME DESTINATION bin)
