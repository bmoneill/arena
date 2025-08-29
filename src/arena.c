#include "arena.h"

#include <stdlib.h>

/**
 * @brief Initializes an Arena with a given size.
 *
 * @param size The total size of the arena to allocate.
 * @param maxBlocks The maximum number of blocks that can be allocated at once.
 * @return A pointer to the initialized Arena structure, or NULL on failure.
 */
Arena *arena_init(size_t size, size_t maxBlocks) {
    Arena *arena;

    if (!(arena = (Arena *) malloc(sizeof(Arena)))) {
        return NULL;
    }

    arena->idx = 0;
    arena->size = size;
    arena->maxBlocks = maxBlocks;

    if (!(arena->head = (ArenaBlock *) malloc(sizeof(ArenaBlock)))) {
        return NULL;
    }

    arena->head->idx = 0;
    arena->head->size = size;
    arena->head->tag = ARENA_TAG_NONE;
    arena->head->status = ARENA_STATUS_FREE;
    arena->head->next = NULL;
    return arena;
}

int arena_destroy(Arena *arena) {
    if (arena->mem) {
        free(arena->mem);
    }

    free(arena);
    return 1;
}

/**
 * @brief Allocates memory for an array of elements, initializing all bytes to zero.
 *
 * @param arena Pointer to the Arena structure.
 * @param num Number of elements to allocate.
 * @param size Size of each element.
 * @return Pointer to the allocated memory, or NULL on failure.
 */
void *arena_calloc(Arena *arena, size_t num, size_t size) {
    // TODO implement
}

void *arena_malloc(Arena *arena, size_t size) {
    ArenaBlock *current = arena->head;
    while (current != NULL) {
        if (current->status == ARENA_STATUS_FREE && current->size >= size) {
            // Create another block in between
            ArenaBlock *oldNext = current->next;
            ArenaBlock *newNext;
            if (!(newNext = (ArenaBlock *) malloc(sizeof(ArenaBlock)))) {
                return NULL;
            }

            current->next = newNext;
            current->size = size;
            current->status = ARENA_STATUS_USED;

            newNext->next = oldNext;
            newNext->idx = current->idx + size;
            newNext->size = current->size - size;
            newNext->status = ARENA_STATUS_FREE;
            newNext->tag = ARENA_TAG_NONE;

            return &arena->mem + current->idx;
        }

        current = current->next;
    }
}

void *arena_realloc(Arena *arena, void *p, size_t n, size_t size) {
    // TODO implement
    return NULL;
}

void arena_free(Arena *arena, void *p) {
    ArenaBlock *current = arena->head;
    size_t idx = (size_t) (&p - &arena->mem);
    while (current != NULL) {
        if (current->idx == idx) {
            current->status = ARENA_STATUS_FREE;
            return;
        }

        current = current->next;
    }
}

static ArenaBlock *create_arenablock(Arena *arena, size_t size) {
    ArenaBlock *block = NULL;
    void *ptr;

    if (size > arena->size) {
        // Block too big
        return NULL;
    }

    for (int i = 0; i < arena->maxBlocks; i++) {
        if (arena->blocks[i]->ptr == NULL) {
            block = arena->blocks[i];
            block->next = NULL;
            block->prev = arena->tail;
            block->size = size;
        }
    }

    if (block == NULL) {
        // Max blocks already alloced
        return NULL;
    }

    if (arena->head == NULL) {
        arena->head = block;
    }

}

/**
 * @brief Find a free block with the given size
 */
static void *find_free_block(Arena *arena, size_t size) {
    ArenaBlock *block = create_arenablock(arena, size);
    if (block == NULL) {
        return NULL;
    }

    ArenaBlock *cur = arena->head;
    ArenaBlock *prev = NULL;

    while (cur != NULL) {

    }
}
