cmake_minimum_required(VERSION 3.16)
project(cachesim CXX)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_compile_options(-Wall -Wextra -Wpedantic -Wconversion)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-fsanitize=address,undefined -g)
  add_link_options(-fsanitize=address,undefined)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_options(-O2)
endif()

add_library(cachesim_lib
    src/replacement_policy.cpp
)
target_include_directories(cachesim_lib PUBLIC include)

add_executable(cachesim src/main.cpp)
target_link_libraries(cachesim PRIVATE cachesim_lib)

enable_testing()
add_subdirectory(tests)