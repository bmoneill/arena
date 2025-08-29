#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

#define ARENA_STATUS_FREE 0
#define ARENA_STATUS_USED 1
#define ARENA_TAG_NONE -1

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
    ArenaBlock *head;
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
int arena_collect_tag(Arena *arena, int tag);
*/

#endif
