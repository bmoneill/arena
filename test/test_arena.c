#include "unity.h"
#include "arena.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INIT_MANAGED(s, b) arena = arena_init(s, b, 1)
#define INIT_UNMANAGED(s) arena = arena_init(s, 0, 0)

Arena *arena;

void setUp(void) { }

void tearDown(void) {
    if (arena) {
        arena_destroy(arena);
        arena = NULL;
    }
}

void test_arena_init_and_destroy_managed(void) {
    size_t arena_size = 1024;
    size_t max_blocks = 10;

    INIT_MANAGED(1024, 10);
    TEST_ASSERT_NOT_NULL(arena);
    TEST_ASSERT_EQUAL(arena_size, arena->size);
    TEST_ASSERT_EQUAL(max_blocks, arena->maxBlocks);
    TEST_ASSERT_EQUAL(1, arena->managed);
    TEST_ASSERT_NOT_NULL(arena->mem);
    TEST_ASSERT_NOT_NULL(arena->head);

    int result = arena_destroy(arena);
    arena = NULL;
    TEST_ASSERT_EQUAL(1, result);
}

void test_arena_init_and_destroy_unmanaged(void) {
    size_t arena_size = 2048;

    INIT_UNMANAGED(arena_size);
    TEST_ASSERT_NOT_NULL(arena);
    TEST_ASSERT_EQUAL(arena_size, arena->size);
    TEST_ASSERT_EQUAL(0, arena->managed);
    TEST_ASSERT_NOT_NULL(arena->mem);
    TEST_ASSERT_NOT_NULL(arena->ptr);

    int result = arena_destroy(arena);
    TEST_ASSERT_EQUAL(1, result);
    arena = NULL;
}

void test_arena_alloc_managed(void) {
    size_t size = 128;
    INIT_MANAGED(1024, 10);
    ArenaBlock *block = arena_alloc(arena, size);
    TEST_ASSERT_NOT_NULL(block);
    TEST_ASSERT_EQUAL(0, block->idx);
    TEST_ASSERT_EQUAL(size, block->size);
    TEST_ASSERT_EQUAL(ARENA_STATUS_USED, block->status);
    TEST_ASSERT_EQUAL(ARENA_TAG_NONE, block->tag);
}

void test_arena_alloc_unmanaged(void) {
    INIT_UNMANAGED(1024);
    void *ptr = arena_alloc(arena, 128);
    TEST_ASSERT_NULL(ptr);
}

void test_arena_malloc_managed(void) {
    INIT_MANAGED(1024, 10);
    void *ptr = arena_malloc(arena, 128);
    TEST_ASSERT_NOT_NULL(ptr);
    memset(ptr, 0xAA, 128);
    for (size_t i = 0; i < 128; i++) {
        TEST_ASSERT_EQUAL_HEX8(0xAA, ((uint8_t *)ptr)[i]);
    }
}

void test_arena_malloc_unmanaged(void) {
    INIT_UNMANAGED(1024);
    void *ptr = arena_malloc(arena, 128);
    TEST_ASSERT_NOT_NULL(ptr);
    memset(ptr, 0xBB, 128);
    for (size_t i = 0; i < 128; i++) {
        TEST_ASSERT_EQUAL_HEX8(0xBB, ((uint8_t *)ptr)[i]);
    }
}

void test_arena_calloc_managed(void) {
    size_t n = 10;
    size_t size = 16;
    INIT_MANAGED(1024, 10);
    void *ptr = arena_calloc(arena, n, size);
    TEST_ASSERT_NOT_NULL(ptr);
    for (size_t i = 0; i < n * size; i++) {
        TEST_ASSERT_EQUAL_HEX8(0x00, ((uint8_t *)ptr)[i]);
    }
}

void test_arena_calloc_unmanaged(void) {
    size_t n = 10;
    size_t size = 16;
    INIT_UNMANAGED(1024);
    void *ptr = arena_calloc(arena, n, size);
    TEST_ASSERT_NOT_NULL(ptr);
    for (size_t i = 0; i < n * size; i++) {
        TEST_ASSERT_EQUAL_HEX8(0x00, ((uint8_t *)ptr)[i]);
    }
}

void test_arena_realloc_managed(void) {
    INIT_MANAGED(1024, 10);
    void *ptr = arena_malloc(arena, 128);
    TEST_ASSERT_NOT_NULL(ptr);
    memset(ptr, 0xCC, 128);
    for (size_t i = 0; i < 128; i++) {
        TEST_ASSERT_EQUAL_HEX8(0xCC, ((uint8_t *)ptr)[i]);
    }

    void *new_ptr = arena_realloc(arena, ptr, 256);
    TEST_ASSERT_NOT_NULL(new_ptr);
    for (size_t i = 0; i < 128; i++) {
        TEST_ASSERT_EQUAL_HEX8(0xCC, ((uint8_t *)new_ptr)[i]);
    }
    memset(new_ptr, 0xDD, 256);
    for (size_t i = 0; i < 256; i++) {
        TEST_ASSERT_EQUAL_HEX8(0xDD, ((uint8_t *)new_ptr)[i]);
    }
}

void test_arena_realloc_unmanaged(void) {
    INIT_UNMANAGED(1024);
    void *ptr = arena_realloc(arena, NULL, 128);
    TEST_ASSERT_NULL(ptr);
}

void test_arena_free_block_managed(void) {
    INIT_MANAGED(1024, 10);
    ArenaBlock *block1 = arena_alloc(arena, 128);
    ArenaBlock *block2 = arena_alloc(arena, 256);
    TEST_ASSERT_NOT_NULL(block1);
    TEST_ASSERT_NOT_NULL(block2);
    TEST_ASSERT_EQUAL(ARENA_STATUS_USED, block1->status);
    TEST_ASSERT_EQUAL(ARENA_STATUS_USED, block2->status);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_arena_init_and_destroy_managed);
    RUN_TEST(test_arena_init_and_destroy_unmanaged);
    RUN_TEST(test_arena_alloc_managed);
    RUN_TEST(test_arena_alloc_unmanaged);
    RUN_TEST(test_arena_malloc_managed);
    RUN_TEST(test_arena_malloc_unmanaged);
    RUN_TEST(test_arena_calloc_managed);
    RUN_TEST(test_arena_calloc_unmanaged);
    RUN_TEST(test_arena_realloc_managed);
    RUN_TEST(test_arena_realloc_unmanaged);
    //RUN_TEST(test_arena_free_block_managed);
    return UNITY_END();
}
