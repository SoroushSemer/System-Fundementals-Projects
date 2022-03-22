/**
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "sfmm.h"

/*
 ---------------------------------------------PERSONAL MACROS----------------------------------------------------
*/
#define ALIGN_SIZE 16
#define HEADER_SIZE 8
#define MIN_BLOCK_SIZE 32

#define BLOCK_BYTES 0x00000000FFFFFFF0

#define SET_PAYLOAD_BYTES 0x000000000FFFFFFFF
#define SET_BLOCK_BYTES 0xFFFFFFFF0000000F
#define SET_ALLOC_BYTES 0xFFFFFFFFFFFFFFFB
#define SET_PRV_ALLOC_BYTES 0xFFFFFFFFFFFFFFFD
#define SET_IN_QCKLST_BYTES 0xFFFFFFFFFFFFFFFE

#define ENOMEM -1

/*
 ---------------------------------------GLOBAL VARIABLES----------------------------------------------------
*/
int first_alloc = 1;
sf_block *epilogue;

/*
 ---------------------------------------GETTER/SETTER FUNCTIONS----------------------------------------------------
*/
void set_block_payload_size(sf_header *header, sf_header size)
{
    size <<= 32;
    *header = (*header & SET_PAYLOAD_BYTES) + size;
}

int get_block_size(sf_header *header)
{
    return (*header & BLOCK_BYTES) >> 4;
}
void set_block_size(sf_header *header, int new_size)
{
    new_size = new_size << 4;
    *header = (*header & SET_BLOCK_BYTES) + new_size;
}

int get_header_alloc(sf_header *header)
{
    return (*header & THIS_BLOCK_ALLOCATED) >> 2;
}
void set_block_alloc(sf_header *header, int alloc)
{
    alloc <<= 2;
    *header = (*header & SET_ALLOC_BYTES) + alloc;
}

int get_header_prv_alloc(sf_header *header)
{
    return (*header & PREV_BLOCK_ALLOCATED) >> 1;
}
void set_block_prv_alloc(sf_header *header, int prv_alloc)
{
    prv_alloc <<= 1;
    *header = (*header & SET_PRV_ALLOC_BYTES) + prv_alloc;
}

void set_block_in_qcklst(sf_header *header, int in_qcklst)
{
    *header = (*header & SET_IN_QCKLST_BYTES) + in_qcklst;
}

/*
 ---------------------------------------------PERSONAL FUNCTIONS----------------------------------------------------
*/
int valid_pointer(void *pp)
{
    if (!pp)
        return 0;
    if ((long int)pp % ALIGN_SIZE != 0)
        return 0;

    // TO BE IMPLEMENTED

    return 1;
}

sf_size_t correct_size(sf_size_t size)
{
    size += HEADER_SIZE;
    if (size % ALIGN_SIZE != 0)
        size += ALIGN_SIZE - (size % ALIGN_SIZE);
    if (size < MIN_BLOCK_SIZE)
        size = MIN_BLOCK_SIZE;
    return size;
}

void *check_quick_lists(sf_size_t size)
{
    int quick_list_index = (size - MIN_BLOCK_SIZE) / ALIGN_SIZE;

    if (quick_list_index > 9 || quick_list_index < 0)
        return NULL;
    if (!sf_quick_lists[quick_list_index].length)
        return NULL;

    sf_block *currentBlock = sf_quick_lists[quick_list_index].first;
    for (int i = 0; i < sf_quick_lists[quick_list_index].length; i++)
    {
        if (get_block_size(&currentBlock->header) == size)
        {
            sf_block *prevBlock = currentBlock->body.links.prev;
            sf_block *nextBlock = currentBlock->body.links.next;
            prevBlock->body.links.next = nextBlock;
            nextBlock->body.links.prev = prevBlock;
            set_block_in_qcklst(&currentBlock->header, 0);
            sf_quick_lists[quick_list_index].length--;
            return currentBlock;
        }
        currentBlock = currentBlock->body.links.next;
    }
    return NULL;
}

int get_free_list_index(sf_size_t size)
{
    int list_min = MIN_BLOCK_SIZE;
    if (list_min == size)
        return 0;
    int index = 0;
    while ((size > list_min) || index == NUM_FREE_LISTS - 1)
    {
        if (size <= list_min * 2)
        {
            return index;
        }
        index++;
        list_min *= 2;
    }
    return NUM_FREE_LISTS - 1;
}

sf_block *get_prev_heap_block(sf_block *block)
{
    sf_footer *prev_footer = &block->prev_footer;
    sf_block *prev_block = (sf_block *)(prev_footer - (get_block_size(prev_footer) * 8) + (HEADER_SIZE * 8));
    return prev_block;
}

sf_block *coalesce(sf_block *block)
{
    if (!get_header_prv_alloc(&block->header))
    {
        sf_block *prev_block = get_prev_heap_block(block);
        int prev_block_size = get_block_size(&prev_block->header);
        int curr_block_size = get_block_size(&block->header);
        set_block_size(&prev_block->header, prev_block_size + curr_block_size);
        return prev_block;
    }
    else
        return block;
    // TO BE IMPLEMENTED
    // gotta coalesce the next block
}

sf_block *mem_grow_block()
{
    sf_block *new_page = epilogue;
    void *mem_grow = sf_mem_grow();
    if (!mem_grow)
        return NULL;
    epilogue = memcpy((void *)(sf_mem_end() - 64), epilogue, 8);
    set_block_alloc(&new_page->header, 0);
    set_block_in_qcklst(&new_page->header, 0);
    set_block_prv_alloc(&new_page->header, 0); // fix
    set_block_size(&new_page->header, PAGE_SZ);
    return new_page;
}

void *check_free_lists(sf_size_t size)
{

    for (int free_list_index = get_free_list_index(size); free_list_index < NUM_FREE_LISTS; free_list_index++)
    {
        sf_block list_head = sf_free_list_heads[free_list_index];
        sf_block *current_block = list_head.body.links.next;
        if (!list_head.body.links.next)
            continue;
        while (&list_head != current_block)
        {
            if (get_block_size(&current_block->header) >= size)
            {
                sf_block *prev_block = current_block->body.links.prev;
                sf_block *next_block = current_block->body.links.next;
                prev_block->body.links.next = next_block;
                next_block->body.links.prev = prev_block;
                return current_block;
            }
            current_block = current_block->body.links.next;
        }
    }
    sf_block *coalesced_block;
    while (size > 0)
    {
        sf_block *new_block = mem_grow_block();
        if (!new_block)
            return NULL;
        coalesced_block = coalesce(new_block);
        size -= PAGE_SZ + get_block_size(&coalesced_block->header);
    }
    return coalesced_block;
}

void place_in_free_list(sf_block *block)
{
    int block_size = get_block_size(&block->header);
    int free_list_index = get_free_list_index(block_size);
    sf_block *sentinell = &sf_free_list_heads[free_list_index];
    sf_block *next_block = sentinell->body.links.next;
    block->body.links.prev = sentinell;

    if (next_block || next_block == sentinell)
    {
        block->body.links.next = next_block;
        next_block->body.links.prev = block;
    }
    else
    {
        block->body.links.next = sentinell;
        sentinell->body.links.prev = block;
    }
    sentinell->body.links.next = block;
}

void split(sf_block *block, sf_size_t size)
{
    int curr_size = get_block_size(&block->header);
    set_block_size(&block->header, size);
    sf_block *new_block = (sf_block *)((long int)block + (size * 8));
    set_block_size(&new_block->header, curr_size - size);
    set_block_alloc(&new_block->header, 0);
    set_block_prv_alloc(&new_block->header, 1);
    set_block_in_qcklst(&new_block->header, 0);
    place_in_free_list(new_block);
    new_block->prev_footer = block->header;
}

void allocate(sf_block *block)
{
    set_block_alloc(&block->header, 1);
    set_block_in_qcklst(&block->header, 0);
}

void initialize_quick_lists()
{
    for (int i = 0; i < NUM_QUICK_LISTS; i++)
    {
        if (!&sf_quick_lists[i])
        {
            sf_quick_lists[i].length = 0;
            sf_quick_lists[i].first = NULL;
        }
    }
}

void initialize_free_lists()
{
    for (int i = 0; i < NUM_FREE_LISTS; i++)
    {
        if (!&sf_free_list_heads[i])
        {
            sf_free_list_heads[i].body.links.next = &sf_free_list_heads[i];
            sf_free_list_heads[i].body.links.prev = &sf_free_list_heads[i];
        }
    }
}

/*
 ---------------------------------------------REQUIRED FUNCTIONS----------------------------------------------------
*/
void *sf_malloc(sf_size_t size)
{
    sf_size_t client_size = size;
    if (!size)
        return NULL;
    size = correct_size(size);
    if (first_alloc)
    {
        initialize_quick_lists();
        initialize_free_lists();
        void *mem_grow = sf_mem_grow();
        if (!mem_grow)
        {
            sf_errno = ENOMEM;
            return NULL;
        }
        sf_block *prologue = (sf_block *)(mem_grow + 64);
        set_block_payload_size(&prologue->header, 0);
        set_block_size(&prologue->header, MIN_BLOCK_SIZE);
        set_block_alloc(&prologue->header, 1);
        set_block_in_qcklst(&prologue->header, 0);

        sf_block *first_block = (sf_block *)(prologue + MIN_BLOCK_SIZE * 8);
        set_block_size(&first_block->header, PAGE_SZ - MIN_BLOCK_SIZE - HEADER_SIZE - HEADER_SIZE);
        set_block_alloc(&first_block->header, 0);
        set_block_prv_alloc(&first_block->header, 1);
        set_block_in_qcklst(&first_block->header, 0);

        epilogue = (sf_block *)(first_block + (PAGE_SZ - MIN_BLOCK_SIZE - HEADER_SIZE - HEADER_SIZE));
        set_block_payload_size(&prologue->header, 0);
        set_block_size(&epilogue->header, 0);
        set_block_alloc(&epilogue->header, 1);
        set_block_prv_alloc(&epilogue->header, 0);
        set_block_in_qcklst(&epilogue->header, 0);

        place_in_free_list(first_block);
        first_alloc = 0;
    }
    // sf_show_blocks();
    sf_block *address = check_quick_lists(size);
    if (address)
        goto block_found;

    address = check_free_lists(size);
    if (!address)
    {
        sf_errno = ENOMEM;
        return NULL;
    }
block_found:
    if (get_block_size(&address->header) - size >= MIN_BLOCK_SIZE)
    {
        split(address, size);
    }
    allocate(address);
    set_block_payload_size(&address->header, client_size);
    return address + 64;
}

void sf_free(void *pp)
{
    // TO BE IMPLEMENTED
    abort();
}

void *sf_realloc(void *pp, sf_size_t rsize)
{
    if (!valid_pointer(pp))
        return NULL;
    if (!rsize)
        return NULL;

    void *newmem = sf_malloc(rsize);
    size_t ppsize = sizeof(pp);
    memcpy(newmem, pp, ppsize);
    sf_free(pp);

    return newmem;
}

double sf_internal_fragmentation()
{
    // TO BE IMPLEMENTED
    abort();
}

double sf_peak_utilization()
{
    // TO BE IMPLEMENTED
    abort();
}
