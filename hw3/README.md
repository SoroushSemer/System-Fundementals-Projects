# HW3 - ```SFMM``` DYNAMIC MEMORY ALLOCATOR #

```SFMM``` is a dynamic memory allocator  with custom ```malloc```, ```free```, and ```realloc``` functions written in C that uses the following memory management methods:
   * Free lists segregated by size class, using first-fit policy within each size class,augmented with a set of "quick lists" holding small blocks segregated by size.
   * Immediate coalescing of large blocks on free with adjacent free blocks; delayed coalescing on free of small blocks.
   * Boundary tags to support efficient coalescing, with footer optimization that allows footers to be omitted from allocated blocks.
   * Block splitting without creating splinters.
   * Allocated blocks aligned to "double memory row" (16-byte) boundaries.
   * Free lists maintained using last in first out (LIFO) discipline.
   * Obfuscation of block headers and footers to detect heap corruption and attempts to free blocks not previously obtained via allocation.

The goal of this project was to gain an understanding of Dynamic Memory Allocation, Memory Padding and Alignment, Structs and Linked Lists, ```errno``` Numbers, and Unit Testing in C.
