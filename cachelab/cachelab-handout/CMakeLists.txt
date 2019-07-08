cmake_minimum_required(VERSION 3.10)
project(csim C)

#include_directories(/Users/soso/csapp/cachelab/cachelab-handout)

set(CMAKE_C_STANDARD 99)

add_executable(csim csim.c cachelab.c)
# target_link_libraries(apue /Users/soso/apue/apue.3e/lib/libapue.a)