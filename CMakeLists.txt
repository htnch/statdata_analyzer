cmake_minimum_required(VERSION 3.10)
project(StatDataAnalyzer)

set(CMAKE_C_STANDARD 11)

add_library(statdata STATIC src / statdata.c)
target_include_directories(statdata PUBLIC include)

add_executable(statdata_analyzer src / main.c)
target_link_libraries(statdata_analyzer statdata)

add_subdirectory(tests)
