cmake_minimum_required(VERSION 3.20)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

add_library(threadpool
        src/thread_pool.cpp)

add_executable(main
    main.cpp)

target_link_libraries(main
    threadpool)

enable_testing()

add_subdirectory(tests)