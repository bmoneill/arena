cmake_minimum_required(VERSION 3.31.6)

add_executable(arena_tests test_arena.c)
target_link_libraries(arena_tests arena Unity)
add_test(arena arena_tests)
