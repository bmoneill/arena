#include "arena.h"

#include <stdlib.h>
#include <string.h>

#define ARENA_GET_PTR()

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

    if (!(arena->mem = malloc(size))) {
        free(arena);
        return NULL;
    }

    if (!(arena->head = (ArenaBlock *) malloc(sizeof(ArenaBlock)))) {
        free(arena->mem);
        free(arena);
        return NULL;
    }

    arena->head->idx = 0;
    arena->head->size = size;
    arena->head->tag = ARENA_TAG_NONE;
    arena->head->status = ARENA_STATUS_FREE;
    arena->head->next = NULL;
    arena->head->prev = NULL;
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

ArenaBlock *arena_get_block(Arena *arena, void *p) {
    ArenaBlock *current = arena->head;
    size_t idx = (size_t)((char *)p - (char *)arena->mem);

    while (current) {
        if (current->idx == idx) {
            return current;
        }
        current = current->next;
    }

    return NULL;
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
    void *result = arena_malloc(arena, num * size);
    memset(result, 0, num * size);
    return result;
}

void *arena_malloc(Arena *arena, size_t size) {
    ArenaBlock *current = arena->head;
    while (current) {
        if (current->status == ARENA_STATUS_FREE && current->size >= size) {
            if (current->size > size) {
                // Create another block in between
                ArenaBlock *oldNext = current->next;
                ArenaBlock *newNext;
                if (!(newNext = (ArenaBlock *) malloc(sizeof(ArenaBlock)))) {
                    return NULL;
                }

                newNext->next = oldNext;
                newNext->idx = current->idx + size;
                newNext->size = current->size - size;
                newNext->tag = ARENA_TAG_NONE;

                current->next = newNext;
                current->size = size;
            }
            current->status = ARENA_STATUS_USED;
            return ARENA_PTR(arena, current);
        }

        current = current->next;
    }

    return NULL;
}

void *arena_realloc(Arena *arena, void *p, size_t size) {
    ArenaBlock *block = arena_get_block(arena, p);
    if (!block) {
        return NULL;
    }

    if (size == block->size) {
        return p;
    } else if (size < block->size) {
        int oldSize = block->size;
        int delta = oldSize - size;
        block->size = size;
        ArenaBlock *next = block->next;
        if (next) {
            if (next->status == ARENA_STATUS_FREE) {
                next->idx -= delta;
                next->size += delta;
            } else {
                ArenaBlock *oldNext = next;
                if (!(next = (ArenaBlock *) malloc(sizeof(ArenaBlock)))) {
                    return NULL;
                }

                block->next = next;
                next->size = delta;
                next->idx = block->idx + size;
                next->status = ARENA_STATUS_FREE;
                next->next = oldNext;
                next->prev = block;
            }
        }

        return p;
    } else {
        int oldSize = block->size;
        size_t delta = size - oldSize;
        ArenaBlock *next = block->next;
        ArenaBlock *prev = block->prev;
        if (next && next->status == ARENA_STATUS_FREE && next->size >= delta) {
            if (next->size == delta) {
                block->next = next->next;
                block->next->prev = block;
                block->size = size;
                return p;
            } else {
                // Shrink next block
            }
        } else if (prev && prev->status == ARENA_STATUS_FREE && prev->size >= delta) {
            if (prev->size == delta) {
                int oldIdx = block->idx;
                block->prev = prev->prev;
                block->prev->next = block;
                block->idx -= delta;
                block->prev->size -= delta;

                // Move contents back
            }
        } else {
            // Move to completely new block
        }
    }
    return NULL;
}

/**
 * @brief Free the given block of memory and return the next one
 */
ArenaBlock *arena_free_block(Arena *arena, ArenaBlock *block) {
    ArenaBlock *tmp;
    block->status = ARENA_STATUS_FREE;
    block->tag = ARENA_TAG_NONE;

    if (block->next != NULL && block->next->status == ARENA_STATUS_FREE) {
        block->size += block->next->size;
        tmp = block->next;
        block->next = block->next->next;
        block->next->prev = block;
        free(tmp);
    }

    if (block->prev != NULL && block->prev->status == ARENA_STATUS_FREE) {
        block->idx = block->prev->idx;
        block->size += block->prev->size;

        tmp = block->prev;
        block->prev = block->prev->prev;
        block->prev->next = block;
        free(tmp);
    }

    return block->next;
}

int arena_free(Arena *arena, void *p) {
    ArenaBlock *tmp;
    ArenaBlock *block = arena_get_block(arena, p);
    if (!block) {
        // Couldn't find block from pointer
        return ARENA_FAILURE;
    }

    arena_free_block(arena, block);
    return ARENA_SUCCESS;
}

int arena_get_tag(Arena *arena, void *p) {
    ArenaBlock *block = arena_get_block(arena, p);
    if (block) {
        return block->tag;
    }
    return ARENA_FAILURE;
}

int arena_set_tag(Arena *arena, void *p, int tag) {
    ArenaBlock *block = arena_get_block(arena, p);
    if (block) {
        block->tag = tag;
        return ARENA_SUCCESS;
    }

    return ARENA_FAILURE;
}

void arena_collect_tag(Arena *arena, int tag) {
    ArenaBlock *block = arena->head;

    while (block) {
        if (block->tag == tag) {
            block = arena_free_block(arena, block);
        } else {
            block = block->next;
        }
    }
}