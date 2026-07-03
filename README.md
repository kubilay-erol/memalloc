# Custom Static Block Memory Allocator

A lightweight, header-only custom memory allocator (`memalloc`) written in C++17/20. This allocator manages a fixed-size compile-time memory pool without relying on the standard heap (`malloc` or standard `new`), making it ideal for bare-metal embedded systems or deterministic real-time systems.

## Features

- **Zero Dynamic Heap Allocation:** Utilizes a statically allocated internal byte pool (`main_pool`).
- **Bitmask Tracking:** Uses an optimized configuration buffer (`conf_buffer`) as a bitmap to track free/allocated chunks with minimal memory overhead.
- **Fixed-Size Chunks:** Allocates memory in highly predictable 32-byte blocks to mitigate fragmentation.
- **Placement New Friendly:** Designed to seamlessly integrate with C++ placement `new` syntax for inline object construction.

## Architecture

The allocator manages a total compile-time memory buffer of **2,560 bytes** (expandable via `buffer_size`) split into three structures:
1. `main_pool`: The raw buffer where your objects and structural data actually live.
2. `conf_buffer`: A tight bitmask array tracking which 32-byte blocks are currently rented or free.
3. `data_buffer`: Keeps track of virtual address offsets and multi-block allocation chains.
