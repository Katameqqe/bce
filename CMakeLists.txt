cmake_minimum_required(VERSION 3.16)
project(ChessEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_library(engine_lib STATIC
  src/board.cpp
  src/movegen.cpp
)
target_include_directories(engine_lib PUBLIC src)

if(MSVC)
  target_compile_options(engine_lib PUBLIC /W4)
else()
  target_compile_options(engine_lib PUBLIC -Wall -Wextra -Wpedantic)
endif()

add_executable(engine src/main.cpp)
target_link_libraries(engine PRIVATE engine_lib)

enable_testing()
add_executable(tests tests/test_main.cpp)
target_link_libraries(tests PRIVATE engine_lib)
add_test(NAME all_tests COMMAND tests)