cmake_minimum_required(VERSION 3.20)
project(ThreadPool
        VERSION 1.0.0
        LANGUAGES CXX)


#   全局C++标准与编译属性设置

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


#   显式查找线程库

find_package(Threads REQUIRED)


#   Target: threadpool

add_library(threadpool
        src/thread_pool.cpp)


#

target_include_directories(threadpool
PUBLIC
    include)


#   线程库应直接绑定到 threadpool Target，外部链接该库时会自动继承依赖

target_link_libraries(threadpool
PUBLIC
    Threads::Threads)


#   Target: main

add_executable(main
    main.cpp)

target_link_libraries(main
    threadpool)


#测试模块

enable_testing()

add_subdirectory(tests)