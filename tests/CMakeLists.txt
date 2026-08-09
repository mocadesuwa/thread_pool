#1.
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

#2.
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

#3.
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