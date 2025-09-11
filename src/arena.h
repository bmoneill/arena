#ifndef ARENA_H
#define ARENA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define ARENA_STATUS_FREE 0
#define ARENA_STATUS_USED 1
#define ARENA_STATUS_UNDEFINED 2
#define ARENA_TAG_NONE -1
#define ARENA_FAILURE 0
#define ARENA_SUCCESS 1

#define ARENA_PTR(arena, block) ((void *)((char *)arena->mem + block->idx))
#define ARENA_COPY(arena, dst, src) memcpy(ARENA_PTR(arena, dst), ARENA_PTR(arena, src), src->size)

typedef struct arena_block_s {
    size_t idx;
    size_t size;
    int tag;
    int status;
    struct arena_block_s *next;
    struct arena_block_s *prev;
} ArenaBlock;

typedef struct {
    void *mem;
    void *ptr;
    ArenaBlock *head;
    size_t idx;
    size_t size;
    size_t maxBlocks;
    bool managed;
} Arena;

/* Init/deinit/helpers */
Arena *arena_init(size_t size, size_t blockCount, int managed);
int arena_destroy(Arena *arena);
ArenaBlock *arena_free_block(Arena *arena, ArenaBlock *block);
ArenaBlock *arena_get_block(Arena *arena, void *p);
ArenaBlock *arena_alloc(Arena *arena, size_t size);
void arena_dump(Arena *arena, FILE *f);
void arena_print(Arena *arena);

/* Standard memory management functions */
void *arena_malloc(Arena *arena, size_t size);
void *arena_calloc(Arena *arena, size_t size, size_t num);
void *arena_realloc(Arena *arena, void *p, size_t size);
int arena_free(Arena *arena, void *p);

/* Tagging stuff */
int arena_get_tag(Arena *arena, void *p);
int arena_set_tag(Arena *arena, void *p, int tag);
void arena_collect_tag(Arena *arena, int tag);
ArenaBlock *arena_get_block_by_tag(Arena *arena, int tag, int n);
void *arena_get_ptr_by_tag(Arena *arena, int tag, int n);

#endif
