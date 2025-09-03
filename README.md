# Arena

This is an Arena memory manager with basic bookkeeping and support for basic memory management
functions (malloc, calloc, realloc, memcpy, and free). Arena memory managers are useful for
short-lived data, such as game state information, ASTs, and other use-cases where the needed size is
predictable and speed is important. Arenas can also be easily dumped for debugging.

## Features

* Bookkeeping: Blocks are stored in a linked list that utilizes memory allocated upon initialization.
* Tagging: Each block can have an assigned integer tag. Tags can be used to find blocks and free them if necessary.

Bookkeeping can be disabled for better performance, but tags will not work. When an Arena is initialized with
`managed` set to `false`, whenever malloc or calloc is called, an internal pointer will simply be incremented,
and blocks will not be created or managed internally.

## Building

### Linux

```shell
cmake --build .
```

## Testing

```shell
cmake -DTARGET_GROUP='test' .
cmake --build .
ctest .
```

## Further Reading

* [Fallout's Memory Model](https://youtu.be/6kB_fko6SIg)
* [Untangling Lifetimes: The Arena Allocator](https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator)

## Bugs

If you find a bug, submit an issue, PR, or email me with a description and/or patch.

## License

Copyright (c) 2025 Ben O'Neill <ben@oneill.sh>. This work is released under the
terms of the MIT License. See [LICENSE](LICENSE) for the license terms.
