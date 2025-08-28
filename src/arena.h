#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

typedef struct arena_block_s {
    size_t idx;
    size_t size;
    void *ptr;
    struct arena_block_s *next;
    struct arena_block_s *prev;
} ArenaBlock;

typedef struct {
    void *mem;
    ArenaBlock **blocks;
    ArenaBlock *head;
    ArenaBlock *tail;
    size_t idx;
    size_t size;
    size_t maxBlocks;
} Arena;

Arena *arena_init(size_t size, size_t blockCount);
int arena_destroy(Arena *arena);

void *arena_calloc(Arena *arena, size_t size, size_t num);
void *arena_malloc(Arena *arena, size_t size);
void *arena_realloc(Arena *arena, void *p, size_t n, size_t size);
void arena_free(Arena *arena, void *p);

/*
int arena_compact(Arena *arena);
int arena_collect(Arena *arena);
*/

#endif
