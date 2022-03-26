/**
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "sfmm.h"
#include "errno.h"

/*
 ---------------------------------------------PERSONAL MACROS----------------------------------------------------
*/
#define ALIGN_SIZE 16
#define HEADER_SIZE 8
#define MIN_BLOCK_SIZE 32

#define BLOCK_BYTES 0x00000000FFFFFFF0
#define PAYLOAD_BYTES 0xFFFFFFFF00000000

#define SET_PAYLOAD_BYTES 0x00000000FFFFFFFF
#define SET_BLOCK_BYTES 0xFFFFFFFF0000000F
#define SET_ALLOC_BYTES 0xFFFFFFFFFFFFFFFB
#define SET_PRV_ALLOC_BYTES 0xFFFFFFFFFFFFFFFD
#define SET_IN_QCKLST_BYTES 0xFFFFFFFFFFFFFFFE

/*
 ---------------------------------------GLOBAL VARIABLES----------------------------------------------------
*/
// int first_alloc = 1;
int current_payload = 0;
int max_payload = 0;
/*
 ---------------------------------------GETTER/SETTER FUNCTIONS----------------------------------------------------
*/
sf_header obfiscate(sf_header *header)
{
    return ((long int)*header) ^ MAGIC;
}

void set_block_payload_size(sf_header *header, sf_header size)
{
    size <<= 32;
    *header = obfiscate(header);
    *header = (*header & SET_PAYLOAD_BYTES) + size;
    *header = obfiscate(header);
}
int get_block_payload_size(sf_header *header)
{
    sf_header unobfiscated_header = obfiscate(header);
    return (unobfiscated_header & PAYLOAD_BYTES) >> 32;
}

int get_block_size(sf_header *header)
{
    sf_header unobfiscated_header = obfiscate(header);
    return (unobfiscated_header & BLOCK_BYTES);
}

void set_block_size(sf_header *header, int new_size)
{
    // new_size <<= ;
    sf_header unobfiscated_header = obfiscate(header);
    *header = (unobfiscated_header & SET_BLOCK_BYTES) + new_size;
    *header = obfiscate(header);
}

int get_header_alloc(sf_header *header)
{
    sf_header unobfiscated_header = obfiscate(header);
    return (unobfiscated_header & THIS_BLOCK_ALLOCATED) >> 2;
}
void set_block_alloc(sf_header *header, int alloc)
{
    alloc <<= 2;
    *header = obfiscate(header);
    *header = (*header & SET_ALLOC_BYTES) + alloc;
    *header = obfiscate(header);
}

int get_header_prv_alloc(sf_header *header)
{
    sf_header unobfiscated_header = obfiscate(header);
    return (unobfiscated_header & PREV_BLOCK_ALLOCATED) >> 1;
}
void set_block_prv_alloc(sf_header *header, int prv_alloc)
{
    prv_alloc <<= 1;
    *header = obfiscate(header);
    *header = (*header & SET_PRV_ALLOC_BYTES) + prv_alloc;
    *header = obfiscate(header);
}

void set_block_in_qcklst(sf_header *header, int in_qcklst)
{
    *header = obfiscate(header);
    *header = (*header & SET_IN_QCKLST_BYTES) + in_qcklst;
    *header = obfiscate(header);
}
int get_header_in_qcklst(sf_header *header)
{
    sf_header unobfiscated_header = obfiscate(header);
    return (unobfiscated_header & IN_QUICK_LIST);
}

/*
 ---------------------------------------------PERSONAL FUNCTIONS----------------------------------------------------
*/
void validate_pointer(void *pp)
{
    if (!pp)
        abort();

    if ((long int)pp % HEADER_SIZE != 0)
        abort();

    sf_block *pp_block = (sf_block *)((long int)pp - HEADER_SIZE - HEADER_SIZE);
    pp_block->body.links.next = NULL;
    pp_block->body.links.prev = NULL;
    if ((get_block_size(&pp_block->header)) < MIN_BLOCK_SIZE)
        abort();

    sf_block *first_block = (sf_block *)(sf_mem_start() + 4 * HEADER_SIZE);
    if ((long int)&pp_block->header < (long int)first_block)
        abort();

    sf_block *heap_block_end = (sf_block *)(sf_mem_end() - 2 * HEADER_SIZE);
    if ((long int)pp_block + get_block_size(&pp_block->header) > (long int)heap_block_end)
        abort();

    if (!get_header_alloc(&pp_block->header))
        abort();

    if (get_header_prv_alloc(&pp_block->header) == 0 && get_header_alloc(&pp_block->prev_footer))
        abort();
    return;
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
    int quick_list_index = ((int)size - MIN_BLOCK_SIZE) / ALIGN_SIZE;

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
            if (prevBlock)
                prevBlock->body.links.next = nextBlock;
            if (nextBlock)
                nextBlock->body.links.prev = prevBlock;
            if (currentBlock == sf_quick_lists[quick_list_index].first)
                sf_quick_lists[quick_list_index].first = nextBlock;
            set_block_in_qcklst(&currentBlock->header, 0);
            sf_quick_lists[quick_list_index].length--;
            currentBlock->body.links.next = NULL;
            currentBlock->body.links.prev = NULL;
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
    while ((size > list_min) && index != NUM_FREE_LISTS - 1)
    {
        if (size <= list_min * 2)
        {
            return index + 1;
        }
        index++;
        list_min *= 2;
    }
    return NUM_FREE_LISTS - 1;
}

sf_block *get_prev_heap_block(sf_block *block)
{
    // sf_block *prev_block = (sf_block *)(prev_footer - (get_block_size(prev_footer) * 8) + (HEADER_SIZE * 8));
    if (!block->prev_footer)
        return NULL;
    sf_block *prev_block = (sf_block *)((long int)block - (get_block_size(&block->prev_footer)));
    return prev_block;
}

void place_in_free_list(sf_block *block)
{
    int block_size = get_block_size(&block->header);
    int free_list_index = get_free_list_index(block_size);
    sf_block *sentinell = &sf_free_list_heads[free_list_index];
    sf_block *next_block = sentinell->body.links.next;
    block->body.links.prev = sentinell;
    if (block == next_block)
        return;
    if (next_block != sentinell)
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

    // int block_size = get_block_size(&block->header);
    // int free_list_index = get_free_list_index(block_size);
    // sf_block *sentinell = &sf_free_list_heads[free_list_index];
    // sf_block *prev_block = sentinell->body.links.next;
    // block->body.links.next = sentinell;

    // if (prev_block != sentinell)
    // {
    //     block->body.links.prev = prev_block;
    //     prev_block->body.links.next = block;
    // }
    // else
    // {
    //     block->body.links.prev = sentinell;
    //     sentinell->body.links.next = block;
    // }
    // sentinell->body.links.prev = block;
}

void remove_from_free_list(sf_block *block)
{
    sf_block *prev_block = block->body.links.prev;
    sf_block *next_block = block->body.links.next;
    if (prev_block)
        prev_block->body.links.next = next_block;
    if (next_block)
        next_block->body.links.prev = prev_block;
    block->body.links.next = NULL;
    block->body.links.prev = NULL;
}

sf_block *coalesce(sf_block *block)
{

    sf_block *epilogue = (sf_block *)(sf_mem_end() - HEADER_SIZE - HEADER_SIZE);
    sf_block *next_block = (sf_block *)((long int)block + get_block_size(&block->header));
    if (!get_header_prv_alloc(&block->header))
    {
        if (block->body.links.next || block->body.links.prev)
            remove_from_free_list(block);
        sf_block *prev_block = get_prev_heap_block(block);
        if (!prev_block)
            return block;
        if (prev_block->body.links.next || prev_block->body.links.prev)
            remove_from_free_list(prev_block);
        int prev_block_size = get_block_size(&prev_block->header);
        int curr_block_size = get_block_size(&block->header);
        set_block_size(&prev_block->header, prev_block_size + curr_block_size);
        place_in_free_list(prev_block);
        sf_footer *footer = (sf_footer *)((long int)prev_block + prev_block_size + curr_block_size);
        *footer = prev_block->header;
        if (!get_header_prv_alloc(&prev_block->header))
            prev_block = coalesce(prev_block);
        next_block = (sf_block *)((long int)prev_block + get_block_size(&prev_block->header));
        set_block_prv_alloc(&next_block->header, 0);
        if (!get_header_alloc(&next_block->header))
        {
            sf_footer *footer = (sf_footer *)((long int)next_block + get_block_size(&next_block->header));
            *footer = next_block->header;
            sf_block *next = (sf_block *)footer;
            next->prev_footer = next_block->header;
            if (next_block != epilogue)
                return coalesce(next_block);
        }
        return prev_block;
    }
    if (next_block != epilogue && !get_header_alloc(&next_block->header))

        return coalesce(next_block);
    return block;
    // TO BE IMPLEMENTED
    // gotta coalesce the next block
}

sf_block *mem_grow_block()
{
    sf_block *epilogue = (sf_block *)(sf_mem_end() - HEADER_SIZE - HEADER_SIZE);
    sf_block *new_page = epilogue;
    void *mem_grow = sf_mem_grow();
    if (!mem_grow)
        return NULL;
    size_t epilogue_size = sizeof(*epilogue);
    epilogue = memcpy((void *)(sf_mem_end() - HEADER_SIZE - HEADER_SIZE), epilogue, epilogue_size);
    set_block_alloc(&new_page->header, 0);
    set_block_in_qcklst(&new_page->header, 0);
    set_block_prv_alloc(&new_page->header, 0); // fix
    set_block_size(&new_page->header, PAGE_SZ);
    new_page->prev_footer = epilogue->prev_footer;
    // new_page->prev_footer = *((sf_footer *)((long int)new_page));
    epilogue->prev_footer = new_page->header;
    return new_page;
}

void *check_free_lists(sf_size_t size)
{

    for (int free_list_index = get_free_list_index(size); free_list_index < NUM_FREE_LISTS; free_list_index++)
    {
        sf_block *list_head = &sf_free_list_heads[free_list_index];
        sf_block *current_block = list_head->body.links.next;
        if (!list_head->body.links.next)
            continue;
        while (list_head != current_block)
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
    int keep_looping = 1;
    while (keep_looping)
    {
        sf_block *new_block = mem_grow_block();
        // sf_show_heap();
        if (!new_block)
            return NULL;
        coalesced_block = coalesce(new_block);
        if (get_block_size(&coalesced_block->header) >= (int)size)
        {
            keep_looping = 0;
        }
    }
    return coalesced_block;
}

void split(sf_block *block, sf_size_t size)
{
    sf_block *epilogue = (sf_block *)(sf_mem_end() - HEADER_SIZE - HEADER_SIZE);
    // sf_show_heap();
    int curr_size = get_block_size(&block->header);
    set_block_size(&block->header, size);
    remove_from_free_list(block);
    sf_block *new_block = (sf_block *)((long int)block + (size));
    set_block_size(&new_block->header, curr_size - size);
    set_block_payload_size(&new_block->header, 0);
    set_block_alloc(&new_block->header, 0);
    set_block_prv_alloc(&new_block->header, 1);
    set_block_in_qcklst(&new_block->header, 0);
    sf_footer *footer = (sf_footer *)((long int)block + curr_size);
    *footer = new_block->header;
    place_in_free_list(new_block);
    new_block->prev_footer = block->header;
    set_block_prv_alloc(&((sf_block *)((long int)new_block + get_block_size(&new_block->header)))->header, 0);
    if ((sf_block *)((long int)new_block + get_block_size(&new_block->header)) == epilogue)
    {
        set_block_prv_alloc(&epilogue->header, 0);
    }
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
        sf_quick_lists[i].length = 0;
        sf_quick_lists[i].first = NULL;
    }
}

void initialize_free_lists()
{
    for (int i = 0; i < NUM_FREE_LISTS; i++)
    {
        if (!sf_free_list_heads[i].body.links.next || !sf_free_list_heads[i].body.links.prev)
        {
            sf_free_list_heads[i].body.links.next = &sf_free_list_heads[i];
            sf_free_list_heads[i].body.links.prev = &sf_free_list_heads[i];
        }
    }
}

void place_in_quick_list(sf_block *block)
{
    int quick_list_index = (get_block_size(&block->header) - MIN_BLOCK_SIZE) / ALIGN_SIZE;
    set_block_in_qcklst(&block->header, 1);

    if (sf_quick_lists[quick_list_index].length == 0)
    {
        sf_quick_lists[quick_list_index].first = block;
        sf_quick_lists[quick_list_index].length++;
    }
    else if (sf_quick_lists[quick_list_index].length != QUICK_LIST_MAX)
    {
        sf_block *head = sf_quick_lists[quick_list_index].first;
        block->body.links.next = head;
        block->body.links.prev = NULL;

        head->body.links.prev = block;
        sf_quick_lists[quick_list_index].first = block;
        sf_quick_lists[quick_list_index].length++;
    }
    else
    {
        sf_block *current_block = sf_quick_lists[quick_list_index].first;
        sf_block *next_block = (sf_block *)current_block->body.links.next;
        for (int i = 0; i < sf_quick_lists[quick_list_index].length; i++)
        {
            set_block_in_qcklst(&current_block->header, 0);
            set_block_alloc(&current_block->header, 0);
            sf_footer *footer = (sf_footer *)((long int)current_block + get_block_size(&current_block->header));
            *footer = current_block->header;
            sf_block *next = (sf_block *)footer;
            next->prev_footer = current_block->header;
            set_block_prv_alloc(&next->header, 0);
            current_block = coalesce(current_block);
            footer = (sf_footer *)((long int)current_block + get_block_size(&current_block->header));
            *footer = current_block->header;
            next = (sf_block *)footer;
            next->prev_footer = current_block->header;
            place_in_free_list(current_block);
            current_block = next_block;
            if (next_block)
                next_block = (sf_block *)next_block->body.links.next;
        }
        sf_quick_lists[quick_list_index].first = block;
        sf_quick_lists[quick_list_index].length = 1;

        // sf_block *head = sf_quick_lists[quick_list_index].first;
        // sf_block *next_block = (sf_block *)head->body.links.next;
        // block->body.links.next = next_block;
        // block->body.links.prev = NULL;

        // next_block->body.links.prev = block;
        // sf_quick_lists[quick_list_index].first = block;
        // set_block_in_qcklst(&head->header, 0);
        // set_block_alloc(&head->header, 0);
        // sf_footer *footer = (sf_footer *)((long int)head + get_block_size(&head->header));
        // *footer = head->header;
        // sf_block *next = (sf_block *)footer;
        // next->prev_footer = head->header;
        // head = coalesce(head);
        // place_in_free_list(head);
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
    sf_block *epilogue;
    if (sf_mem_start() == sf_mem_end())
    {
        initialize_quick_lists();
        initialize_free_lists();
        void *mem_grow = sf_mem_grow();
        epilogue = (sf_block *)(sf_mem_end() - HEADER_SIZE - HEADER_SIZE);
        if (!mem_grow)
        {
            sf_errno = ENOMEM;
            return NULL;
        }
        sf_block *prologue = (sf_block *)(mem_grow);
        set_block_payload_size(&prologue->header, 0);
        set_block_size(&prologue->header, MIN_BLOCK_SIZE);
        set_block_alloc(&prologue->header, 1);
        set_block_in_qcklst(&prologue->header, 0);

        sf_block *first_block = (sf_block *)(sf_mem_start() + 4 * HEADER_SIZE);
        set_block_size(&first_block->header, PAGE_SZ - MIN_BLOCK_SIZE - 2 * HEADER_SIZE);
        set_block_alloc(&first_block->header, 0);
        set_block_prv_alloc(&first_block->header, 1);
        set_block_in_qcklst(&first_block->header, 0);
        place_in_free_list(first_block);

        set_block_payload_size(&prologue->header, 0);
        set_block_size(&epilogue->header, 0);
        set_block_alloc(&epilogue->header, 1);
        set_block_prv_alloc(&epilogue->header, 0);
        set_block_in_qcklst(&epilogue->header, 0);
        // epilogue->header = first_block->header;

        sf_footer *footer = (sf_footer *)((long int)epilogue);
        *footer = first_block->header;
        epilogue->prev_footer = first_block->header;
        // sf_show_heap();

        // first_alloc = 0;
    }
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
    if (get_block_size(&address->header) - (int)size >= MIN_BLOCK_SIZE)
    {
        split(address, size);
    }
    allocate(address);
    if (address->body.links.next)
    {
        remove_from_free_list(address);
    }
    sf_block *next_block = (sf_block *)((long int)address + get_block_size(&address->header));
    set_block_prv_alloc(&next_block->header, 1);
    if ((sf_block *)((long int)address + get_block_size(&address->header)) == epilogue)
    {
        set_block_prv_alloc(&epilogue->header, 1);
    }
    set_block_payload_size(&address->header, client_size);

    // printf("address: 0x%.8lx\n", (long int)address);
    current_payload += client_size;
    if (current_payload > max_payload)
        max_payload = current_payload;
    return &address->body.payload;
}

void sf_free(void *pp)
{
    validate_pointer(pp);
    sf_block *pp_block = (sf_block *)((long int)pp - HEADER_SIZE - HEADER_SIZE);
    int quick_list_index = (get_block_size(&pp_block->header) - MIN_BLOCK_SIZE) / ALIGN_SIZE;
    // printf("%d", quick_list_index);
    // abort();

    current_payload -= get_block_payload_size(&pp_block->header);
    set_block_payload_size(&pp_block->header, 0);
    if (quick_list_index < NUM_QUICK_LISTS)
    {
        place_in_quick_list(pp_block);
    }
    else
    {
        sf_block *epilogue = (sf_block *)(sf_mem_end() - HEADER_SIZE - HEADER_SIZE);
        set_block_alloc(&pp_block->header, 0);
        sf_footer *footer = (sf_footer *)((long int)pp_block + get_block_size(&pp_block->header));
        sf_block *next_block = (sf_block *)footer;
        next_block->prev_footer = pp_block->header;
        set_block_prv_alloc(&next_block->header, 0);
        *footer = pp_block->header;
        int pp_size = get_block_size(&pp_block->header);
        pp_block = coalesce(pp_block);
        if (pp_size == get_block_size(&pp_block->header))
            place_in_free_list(pp_block);
        if ((sf_block *)((long int)pp_block + get_block_size(&pp_block->header)) == epilogue)
        {
            set_block_prv_alloc(&epilogue->header, 1);
        }

        sf_block *next = (sf_block *)((long int)pp_block + get_block_size(&pp_block->header));
        set_block_prv_alloc(&next->header, 0);
    }
}

void *sf_realloc(void *pp, sf_size_t rsize)
{
    validate_pointer(pp);
    if (!rsize)
    {
        sf_free(pp);
        return NULL;
    }
    int client_size = rsize;
    rsize = correct_size(rsize);
    sf_block *pp_block = (sf_block *)((long int)pp - HEADER_SIZE - HEADER_SIZE);

    if (rsize > get_block_size(&pp_block->header))
    {
        void *newmem = sf_malloc(client_size);
        int pp_size = get_block_payload_size(&pp_block->header);
        memcpy(newmem, pp, pp_size);
        sf_free(pp);
        return newmem;
    }
    else if (rsize < get_block_size(&pp_block->header))
    {
        current_payload -= (get_block_payload_size(&pp_block->header) - client_size);
        if (get_block_size(&pp_block->header) - (int)rsize >= MIN_BLOCK_SIZE)
        {
            split(pp_block, rsize);
            // sf_show_heap();
            sf_block *free_block = (sf_block *)((long int)pp_block + get_block_size(&pp_block->header));
            coalesce(free_block);
            set_block_payload_size(&pp_block->header, client_size);

            // memcpy(pp_block->body.payload, pp, client_size);
            return pp_block->body.payload;
        }
        else if (get_block_size(&pp_block->header) - (int)rsize >= 0)
        {
            set_block_payload_size(&pp_block->header, client_size);
            return pp;
        }
        else
        {
            return pp;
        }
    }
    else
    {
        return pp;
    }
}

double sf_internal_fragmentation()
{
    // TO BE IMPLEMENTED
    if (sf_mem_start() == sf_mem_end())
        return 0;
    sf_block *epilogue = (sf_block *)(sf_mem_end() - HEADER_SIZE - HEADER_SIZE);
    int total_payload = 0, total_size = 0;
    for (sf_block *current_block = (sf_block *)(sf_mem_start() + 4 * HEADER_SIZE);
         current_block < epilogue;
         current_block = (sf_block *)((long int)current_block + get_block_size(&current_block->header)))
    {
        if (get_header_alloc(&current_block->header) && !get_header_in_qcklst(&current_block->header))
        {
            total_payload += (int)get_block_payload_size(&current_block->header);
            total_size += (int)get_block_size(&current_block->header);
        }
    }
    if (total_size)
        return ((double)total_payload) / total_size;
    return 0;
    // abort();
}

double sf_peak_utilization()
{
    if (sf_mem_end() - sf_mem_start() == 0)
        return 0;
    return ((double)max_payload) / (sf_mem_end() - sf_mem_start());
}
