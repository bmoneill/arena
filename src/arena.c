#include "arena.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ArenaBlock *arena_find_empty_block(Arena *arena);

/**
 * @brief Initializes an Arena with a given size.
 *
 * @param size The total size of the arena to allocate.
 * @param maxBlocks The maximum number of blocks that can be allocated at once.
 * @param managed If non-zero, the arena will manage blocks, allowing for freeing and dynamic reallocation.
 * @return A pointer to the initialized Arena structure, or NULL on failure.
 */
Arena *arena_init(size_t size, size_t maxBlocks, int managed) {
    Arena *arena;

    if (!(arena = (Arena *) malloc(sizeof(Arena)))) {
        return NULL;
    }

    arena->idx = 0;
    arena->size = size;
    arena->maxBlocks = maxBlocks;
    arena->managed = managed;

    if (!(arena->mem = malloc(size))) {
        free(arena);
        return NULL;
    }

    if (managed) {
        if (!(arena->head = (ArenaBlock *) malloc(sizeof(ArenaBlock) * maxBlocks))) {
            free(arena->mem);
            free(arena);
            return NULL;
        }
        arena->head[0].idx = 0;
        arena->head[0].size = size;
        arena->head[0].tag = ARENA_TAG_NONE;
        arena->head[0].status = ARENA_STATUS_FREE;
        arena->head[0].prev = NULL;
        for (size_t i = 1; i < maxBlocks; i++) {
            arena->head[i].idx = -1;
            arena->head[i].size = 0;
            arena->head[i].tag = ARENA_TAG_NONE;
            arena->head[i].prev = &arena->head[i - 1];
            arena->head[i - 1].next = &arena->head[i];
            arena->head[i].status = ARENA_STATUS_UNDEFINED;
        }
        arena->head[maxBlocks - 1].next = NULL;
    } else {
        arena->head = NULL;
        arena->ptr = arena->mem;
    }

    return arena;
}

/**
 * @brief Destroys the given Arena, freeing all associated memory.
 *
 * @param arena Pointer to the Arena structure to destroy.
 * @return 1 on success.
 */
int arena_destroy(Arena *arena) {
    if (arena->mem) {
        free(arena->mem);
    }

    if (arena->managed) {
        free(arena->head);
    }

    free(arena);
    return 1;
}

/**
 * @brief Free the given block of memory and return the next one
 *
 * This function can only be used if the arena is in managed mode.
 *
 * @param arena Pointer to the Arena structure.
 * @param block Pointer to the ArenaBlock to free.
 * @return Pointer to the next ArenaBlock after the freed block.
 */
ArenaBlock *arena_free_block(Arena *arena, ArenaBlock *block) {
    if (!arena->managed) {
        return NULL;
    }

    ArenaBlock *tmp;
    block->status = ARENA_STATUS_FREE;
    block->tag = ARENA_TAG_NONE;

    if (block->next != NULL && block->next->status == ARENA_STATUS_FREE) {
        block->size += block->next->size;
        tmp = block->next;
        block->next = block->next->next;
        block->next->prev = block;
    }

    if (block->prev != NULL && block->prev->status == ARENA_STATUS_FREE) {
        block->idx = block->prev->idx;
        block->size += block->prev->size;

        tmp = block->prev;
        block->prev = block->prev->prev;
        block->prev->next = block;
    }

    if (tmp) {
        tmp->status = ARENA_STATUS_UNDEFINED;
    }
    return block->next;
}


/**
 * @brief Retrieves the ArenaBlock corresponding to the given pointer.
 *
 * This function can only be used if the arena is in managed mode.
 *
 * @param arena Pointer to the Arena structure.
 * @param p Pointer to the memory block.
 * @return Pointer to the corresponding ArenaBlock, or NULL if not found.
 */
ArenaBlock *arena_get_block(Arena *arena, void *p) {
    if (!arena->managed) {
        return NULL;
    }

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
 * @brief Allocates a block of memory of the specified size within the arena.
 *
 * @param arena Pointer to the Arena structure.
 * @param size Size of the memory block to allocate.
 * @return Pointer to the allocated ArenaBlock, or NULL if allocation fails.
 */
ArenaBlock *arena_alloc(Arena *arena, size_t size) {
    if (!arena->managed) {
        return NULL;
    }

    ArenaBlock *current = arena->head;
    while (current) {
        if (current->status == ARENA_STATUS_FREE && current->size >= size) {
            if (current->size > size) {
                // Create another block in between
                ArenaBlock *oldNext = current->next;
                ArenaBlock *newNext;

                newNext = arena_find_empty_block(arena);
                newNext->next = oldNext;
                newNext->idx = current->idx + size;
                newNext->size = current->size - size;
                newNext->status = ARENA_STATUS_FREE;
                newNext->tag = ARENA_TAG_NONE;

                current->next = newNext;
                current->size = size;
            }
            current->status = ARENA_STATUS_USED;
            return current;
        }
        current = current->next;
    }

    return NULL;
}

/**
 * @brief Allocates a block of memory of the specified size within the arena.
 *
 * @param arena Pointer to the Arena structure.
 * @param size Size of the memory block to allocate.
 * @return Pointer to the allocated memory, or NULL if allocation fails.
 */
void *arena_malloc(Arena *arena, size_t size) {
    if (!arena->managed) {
        void *oldHead = arena->ptr;
        arena->ptr = (char *)arena->ptr + size;
        return oldHead;
    }

    ArenaBlock *block = arena_alloc(arena, size);
    if (!block) {
        return NULL;
    }

    return ARENA_PTR(arena, block);
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

/**
 * @brief Reallocates a block of memory to a new size within the arena.
 *
 * This function can only be used if the arena is in managed mode.
 *
 * @param arena Pointer to the Arena structure.
 * @param p Pointer to the existing memory block.
 * @param size New size for the memory block.
 * @return Pointer to the reallocated memory, or NULL on failure.
 */
void *arena_realloc(Arena *arena, void *p, size_t size) {
    if (!arena->managed) {
        return NULL;
    }

    ArenaBlock *block = arena_get_block(arena, p);
    if (!block) {
        // Invalid block
        return NULL;
    }

    if (size == block->size) {
        // New size equal to old size
        return p;
    } else if (size < block->size) {
        // New size less than old size
        int oldSize = block->size;
        int delta = oldSize - size;
        block->size = size;
        ArenaBlock *next = block->next;
        if (next) {
            if (next->status == ARENA_STATUS_FREE) {
                // Expand next block
                next->idx -= delta;
                next->size += delta;
            } else {
                // Create new free block in between
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
        // New size greater than old size
        ArenaBlock *newBlock = arena_malloc(arena, size);
        ARENA_COPY(arena, newBlock, block);
        arena_free_block(arena, block);
        return ARENA_PTR(arena, newBlock);
    }
    return NULL;
}

/**
 * @brief Frees a block of memory within the arena.
 *
 * This function can only be used if the arena is in managed mode.
 *
 * @param arena Pointer to the Arena structure.
 * @param p Pointer to the memory block to free.
 * @return 1 on success, 0 on failure.
 */
int arena_free(Arena *arena, void *p) {
    if (!arena->managed) {
        return ARENA_FAILURE;
    }

    ArenaBlock *tmp;
    ArenaBlock *block = arena_get_block(arena, p);
    if (!block) {
        // Couldn't find block from pointer
        return ARENA_FAILURE;
    }

    arena_free_block(arena, block);
    return ARENA_SUCCESS;
}

/**
 * @brief Retrieves the tag associated with a memory block.
 *
 * This function can only be used if the arena is in managed mode.
 *
 * @param arena Pointer to the Arena structure.
 * @param p Pointer to the memory block.
 * @return The tag of the block, or ARENA_FAILURE if the block is not found.
 */
int arena_get_tag(Arena *arena, void *p) {
    if (!arena->managed) {
        return ARENA_FAILURE;
    }

    ArenaBlock *block = arena_get_block(arena, p);
    if (block) {
        return block->tag;
    }
    return ARENA_FAILURE;
}

/**
 * @brief Sets the tag for a memory block.
 *
 * This function can only be used if the arena is in managed mode.
 *
 * @param arena Pointer to the Arena structure.
 * @param p Pointer to the memory block.
 * @param tag The tag value to set.
 * @return ARENA_SUCCESS on success, ARENA_FAILURE if the block is not found.
 */
int arena_set_tag(Arena *arena, void *p, int tag) {
    if (!arena->managed) {
        return ARENA_FAILURE;
    }

    ArenaBlock *block = arena_get_block(arena, p);
    if (block) {
        block->tag = tag;
        return ARENA_SUCCESS;
    }

    return ARENA_FAILURE;
}

/**
 * @brief Frees all memory blocks with the specified tag.
 *
 * This function can only be used if the arena is in managed mode.
 *
 * @param arena Pointer to the Arena structure.
 * @param tag The tag value to collect.
 */
void arena_collect_tag(Arena *arena, int tag) {
    if (!arena->managed) {
        return;
    }

    ArenaBlock *block;
    while ((block = arena_get_block_by_tag(arena, tag, 0))){
        arena_free_block(arena, block);
    }
}

/**
 * @brief Retrieves the n-th block with the specified tag.
 *
 * This function can only be used if the arena is in managed mode.
 *
 * @param arena Pointer to the Arena structure.
 * @param tag The tag value to search for.
 * @param n The index of the block to retrieve.
 * @return Pointer to the n-th ArenaBlock with the specified tag, or NULL if not found.
 */
ArenaBlock *arena_get_block_by_tag(Arena *arena, int tag, int n) {
    if (!arena->managed) {
        return NULL;
    }

    int count = 0;
    for (size_t i = 0; i < arena->maxBlocks; i++) {
        ArenaBlock *block = &arena->head[i];
        if (block->tag == tag && block->status == ARENA_STATUS_USED) {
            if (count == n) {
                return block;
            }
            count++;
        }
    }

    return NULL;
}

/**
 * @brief Retrieves the pointer to the n-th block with the specified tag.
 *
 * This function can only be used if the arena is in managed mode.
 *
 * @param arena Pointer to the Arena structure.
 * @param tag The tag value to search for.
 * @param n The index of the block to retrieve.
 * @return Pointer to the n-th memory block with the specified tag, or NULL if not found.
 */
void *arena_get_ptr_by_tag(Arena *arena, int tag, int n) {
    if (!arena->managed) {
        return NULL;
    }

    ArenaBlock *block = arena_get_block_by_tag(arena, tag, n);
    if (block) {
        return ARENA_PTR(arena, block);
    }
    return NULL;
}

/**
 * @brief Finds an empty (undefined) block in the arena's block list.
 *
 * This function is used internally to manage block allocations.
 * It can only be used if the arena is in managed mode.
 *
 * @param arena Pointer to the Arena structure.
 * @return Pointer to an empty ArenaBlock, or NULL if none are available.
 */
static ArenaBlock *arena_find_empty_block(Arena *arena) {
    if (!arena->managed) {
        return NULL;
    }

    ArenaBlock *current = arena->head;
    size_t count = 0;

    for (size_t i = 0; i < arena->maxBlocks; i++) {
        if (current->status == ARENA_STATUS_UNDEFINED) {
            return current;
        }
        current++;
    }
    return NULL;
}
