#ifndef ARENA_H
#define ARENA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#ifndef ARENA_VERSION
/**
 * @brief Version of libarena
 */
#define ARENA_VERSION "unknown"
#endif

/**
 * @brief ArenaBlock status enum.
 */
typedef enum {
    ARENA_STATUS_FREE      = 0,
    ARENA_STATUS_USED      = 1,
    ARENA_STATUS_UNDEFINED = 2
} ArenaStatus;

/**
 * @brief No tag placeholder.
 */
#define ARENA_TAG_NONE -1

/**
 * @brief Success status.
 */
#define ARENA_SUCCESS 0

/**
 * @brief Failure status.
 */
#define ARENA_FAILURE -1

/**
 * @brief Get pointer from ArenaBlock
 */
#define ARENA_PTR(arena, block) ((void*) ((char*) arena->mem + block->idx))

/**
 * @brief Copy data from src to dst
 */
#define ARENA_COPY(arena, dst, src) memcpy(ARENA_PTR(arena, dst), ARENA_PTR(arena, src), src->size)

/**
 * @struct ArenaBlock
 * @brief Arena block structure
 *
 * This structure represents a block of memory within an arena.
 * It contains information about the block's index, size, tag, status, and pointers to the next and previous blocks.
 *
 * @param idx    The index of the block within the arena.
 * @param size   The size of the block in bytes.
 * @param tag    An optional tag associated with the block.
 * @param status The status of the block (free, used, or undefined).
 * @param next   A pointer to the next block in the arena.
 * @param prev   A pointer to the previous block in the arena.
 */
typedef struct arena_block_s {
    size_t                idx;
    size_t                size;
    int                   tag;
    ArenaStatus           status;
    struct arena_block_s* next;
    struct arena_block_s* prev;
} ArenaBlock;

/**
 * @struct Arena
 * @brief Arena structure
 *
 * This structure represents an arena of memory.
 * It contains information about the arena's memory, pointer, head block, index, size, maximum
 * block count, and management status.
 *
 * @param mem       A pointer to the memory block of the arena.
 * @param ptr       A pointer to the current position in the memory block.
 * @param head      A pointer to the head block of the arena.
 * @param idx       The index of the current block within the arena.
 * @param size      The size of the memory block in bytes.
 * @param maxBlocks The maximum number of blocks that can be allocated in the arena.
 * @param managed   A flag indicating whether the arena is managed or not.
 */
typedef struct {
    void*       mem;
    void*       ptr;
    ArenaBlock* head;
    size_t      idx;
    size_t      size;
    size_t      maxBlocks;
    bool        managed;
} Arena;

/* Init/deinit/helpers */
Arena*      arena_init(size_t size, size_t blockCount, int managed);
int         arena_destroy(Arena* arena);
ArenaBlock* arena_free_block(Arena* arena, ArenaBlock* block);
ArenaBlock* arena_get_block(Arena* arena, void* p);
ArenaBlock* arena_alloc(Arena* arena, size_t size);
void        arena_dump(Arena* arena, FILE* f);
void        arena_print(Arena* arena);

/* Standard memory management functions */
void* arena_malloc(Arena* arena, size_t size);
void* arena_calloc(Arena* arena, size_t size, size_t num);
void* arena_realloc(Arena* arena, void* p, size_t size);
int   arena_free(Arena* arena, void* p);

/* Tagging stuff */
int         arena_get_tag(Arena* arena, void* p);
int         arena_set_tag(Arena* arena, void* p, int tag);
void        arena_collect_tag(Arena* arena, int tag);
ArenaBlock* arena_get_block_by_tag(Arena* arena, int tag, int n);
void*       arena_get_ptr_by_tag(Arena* arena, int tag, int n);

const char* arena_version();

#endif
