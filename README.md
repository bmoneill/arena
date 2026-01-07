# Arena {#mainpage}

[![Build Status](https://github.com/bmoneill/arena/actions/workflows/cmake-single-platform.yml/badge.svg?branch=main)](https://github.com/bmoneill/arena/actions/workflows/cmake-single-platform.yml)
[![Doxygen Status](https://github.com/bmoneill/arena/actions/workflows/doxygen.yml/badge.svg?branch=main)](https://bmoneill.github.io/arena)
[![Clang-format status](https://github.com/bmoneill/arena/actions/workflows/clang-format.yml/badge.svg?branch=main)](https://github.com/bmoneill/arena/actions/workflows/clang-format.yml)

This is an Arena memory manager with basic bookkeeping and support for basic memory management
functions (malloc, calloc, realloc, memcpy, and free). Arena memory managers are useful for
short-lived data, such as game state information, ASTs, and other use-cases where the needed size is
predictable and speed is important. Arenas can also be easily dumped for debugging.

## Features

* Bookkeeping: Block metadata is internally stored. When a block is freed, that memory may be used by a newly
  allocated block.
* Tagging: Each block can have an assigned integer tag. It is possible to find a block by its tag or free all
  blocks with a given tag.

Bookkeeping can be disabled for better performance, but tags will not work. When an Arena is initialized with
`managed` set to `false`, whenever malloc or calloc is called, an internal pointer will simply be incremented,
and blocks will not be managed internally.

## Building

```shell
# build library
cmake -S . -B build -DTARGET_GROUP=all
cmake --build build

# install
cmake --install build
```

## Testing

```shell
git submodule update
cmake -S . -B build -DTARGET_GROUP=test
cmake --build build
cd build
ctest --verbose
```

## Documentation

[Library documentation is available here](https://bmoneill.github.io/arena/).

## Further Reading

* [Fallout's Memory Model](https://youtu.be/6kB_fko6SIg)
* [Untangling Lifetimes: The Arena Allocator](https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator)

## Bugs

If you find a bug, submit an issue, PR, or email me with a description and/or patch.

## License

Copyright (c) 2025 Ben O'Neill <ben@oneill.sh>. This work is released under the
terms of the MIT License. See [LICENSE](LICENSE) for the license terms.
