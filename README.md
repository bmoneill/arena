# Arena

This is an Arena memory manager with basic bookkeeping and support for basic memory management
functions (malloc, calloc, realloc, memcpy, and free). Arena memory managers are useful for
short-lived data, such as game state information, ASTs, and other use-cases where the needed size is
predictable and speed is important. Arenas can also be easily dumped for debugging.

## Building

### Linux

```shell
make libarena.a # static
make libarena.so # shared
```

## Features

* Simple bookkeeping system
* Tagging support

### Bookkeeping

Blocks are stored in a linked list that is allocated at initialization. The maximum number of blocks must be specified upon initialization. Freed blocks can then be reused as needed.

### Tagging

Each block can have an integer tag which can be used to find blocks with a given tag, and free them if necessary.

## Further Reading

* [Fallout's Memory Model](https://youtu.be/6kB_fko6SIg)
* [Untangling Lifetimes: The Arena Allocator](https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator)

## Bugs

If you find a bug, submit an issue, PR, or email me with a description and/or patch.

## License

Copyright (c) 2025 Ben O'Neill <ben@oneill.sh>. This work is released under the
terms of the MIT License. See [LICENSE](LICENSE) for the license terms.
