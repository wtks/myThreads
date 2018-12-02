cmake_minimum_required(VERSION 2.8)
project(myThreads C ASM)

set(CMAKE_C_FLAGS "-std=c99 -pedantic -Wall -Wextra -Werror -g -m32 -mstackrealign")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}")
add_library(myThreads STATIC swtch.S mythread.c context.c)

add_executable(thtest1 thtest1.c)
target_link_libraries(thtest1 myThreads)

add_executable(thtest2 thtest2.c)
target_link_libraries(thtest2 myThreads)

add_executable(thtest3 thtest3.c)
target_link_libraries(thtest3 myThreads)
