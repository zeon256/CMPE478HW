cmake_minimum_required(VERSION 3.24)
project(cmpe478_hw1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add address sanitizer to debug build
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
endif()

add_executable(cmpe478_hw1 src/main.cpp)

# Find OpenMP
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    message("Found OpenMP!")
    target_link_libraries(cmpe478_hw1 PRIVATE OpenMP::OpenMP_CXX)
endif()

# Set compiler-specific flags to treat nodiscard warnings as errors
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(cmpe478_hw1 PRIVATE)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(cmpe478_hw1 PRIVATE)
endif()