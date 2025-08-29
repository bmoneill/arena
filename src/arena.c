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

    ArenaBlock *current = arena->head;
    while (current != NULL) {
        ArenaBlock *next = current->next;
        free(current);
        current = next;
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
            if (current->size > size) {
                ArenaBlock *oldNext = current->next;
                ArenaBlock *newNext;
                if (!(newNext = (ArenaBlock *) malloc(sizeof(ArenaBlock)))) {
                    return NULL;
                }

                current->next = newNext;
                current->size = size;

                newNext->next = oldNext;
                newNext->idx = current->idx + size;
                newNext->size = current->size - size;
                newNext->status = ARENA_STATUS_FREE;
                newNext->tag = ARENA_TAG_NONE;
            }
            current->status = ARENA_STATUS_USED;
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
    ArenaBlock *tmp = NULL;
    size_t idx = (size_t) (&p - &arena->mem);
    while (current != NULL) {
        if (current->idx == idx) {
            current->status = ARENA_STATUS_FREE;

            if (current->next != NULL && current->next->status == ARENA_STATUS_FREE) {
                current->size += current->next->size;

                tmp = current->next;
                current->next = current->next->next;
                current->next->prev = current;
                free(tmp);
            }

            if (current->prev != NULL && current->prev->status == ARENA_STATUS_FREE) {
                current->idx = current->prev->idx;
                current->size += current->prev->size;

                tmp = current->prev;
                current->prev = current->prev->prev;
                current->prev->next = current;
                free(tmp);
            }

            return;
        }

        current = current->next;
    }
}
