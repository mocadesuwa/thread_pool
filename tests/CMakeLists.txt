#1.基础测试
add_executable(test_basic
    test_basic.cpp)

target_link_libraries(
    test_basic
    threadpool
)

add_test(
    NAME basic
    COMMAND test_basic
)

#2.优先级测试
add_executable(test_priority
    test_priority.cpp)

target_link_libraries(
    test_priority
    threadpool
)

add_test(
    NAME priority
    COMMAND test_priority
)

#3.状态查询测试
add_executable(test_task_state
    test_task_state.cpp)

target_link_libraries(
    test_task_state
    threadpool
)

add_test(
    NAME task_state
    COMMAND test_task_state
)

#4.异常捕获测试
add_executable(test_exception
    test_exception.cpp)

target_link_libraries(
    test_exception
    threadpool
)

add_test(
    NAME exception
    COMMAND test_exception
)

#5.shutdown测试
add_executable(test_shutdown
    test_shutdown.cpp)

target_link_libraries(
    test_shutdown
    threadpool
)

add_test(
    NAME shutdown
    COMMAND test_shutdown
)

#6.queue_size测试
add_executable(test_queue_size
    test_queue_size.cpp)

target_link_libraries(
    test_queue_size
    threadpool)

add_test(
    NAME queue_size
    COMMAND test_queue_size
)

#7.stress测试
add_executable(test_stress
    test_stress.cpp)

target_link_libraries(
    test_stress
    threadpool
)

add_test(
    NAME stress
    COMMAND test_stress
)