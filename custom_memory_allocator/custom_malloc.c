#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define POOL_SIZE (1024 * 1024) // 1 MB

// Metadata structure to manage each memory block
typedef struct Block
{
    size_t size;
    int free;
    struct Block *next;
} Block;

void *pool_start = NULL;
Block *free_list = NULL;

// Align memory to 16 bytes (common for memory alignment)
#define ALIGN16(x) (((((x) - 1) >> 4) << 4) + 16)

void initialize_pool()
{
    pool_start = sbrk(POOL_SIZE);
    if (pool_start == (void *)-1)
    {
        perror("sbrk");
        exit(1);
    }
    free_list = (Block *)pool_start;
    free_list->size = POOL_SIZE - sizeof(Block);
    free_list->free = 1;
    free_list->next = NULL;
}

Block *find_free_block(size_t size)
{
    Block *current = free_list;
    while (current && !(current->free && current->size >= size))
    {
        current = current->next;
    }
    return current;
}

void split_block(Block *block, size_t size)
{
    Block *new_block = (Block *)((char *)block + sizeof(Block) + size);
    new_block->size = block->size - size - sizeof(Block);
    new_block->free = 1;
    new_block->next = block->next;

    block->size = size;
    block->free = 0;
    block->next = new_block;
}

void *my_malloc(size_t size)
{
    if (!pool_start)
    {
        initialize_pool();
    }

    size = ALIGN16(size);

    Block *block = find_free_block(size);
    if (!block)
    {
        return NULL;
    }

    if (block->size >= size + sizeof(Block) + 16)
    {
        split_block(block, size);
    }
    else
    {
        block->free = 0;
    }

    return (char *)block + sizeof(Block);
}

void merge_free_blocks()
{
    Block *current = free_list;
    while (current && current->next)
    {
        if (current->free && current->next->free)
        {
            current->size += sizeof(Block) + current->next->size;
            current->next = current->next->next;
        }
        current = current->next;
    }
}

void my_free(void *ptr)
{
    if (!ptr)
        return;

    Block *block = (Block *)((char *)ptr - sizeof(Block));
    block->free = 1;
    merge_free_blocks();
}

void *my_realloc(void *ptr, size_t size)
{
    if (!ptr)
    {
        return my_malloc(size);
    }

    Block *block = (Block *)((char *)ptr - sizeof(Block));

    if (block->size >= size)
    {
        return ptr;
    }

    void *new_ptr = my_malloc(size);
    if (new_ptr)
    {
        memcpy(new_ptr, ptr, block->size);
        my_free(ptr);
    }

    return new_ptr;
}

void print_free_list()
{
    Block *current = free_list;
    printf("Free list:\n");
    while (current)
    {
        printf("Block at %p, size: %zu, free: %d\n", (void *)current, current->size, current->free);
        current = current->next;
    }
}

int main()
{
    printf("Allocating 100 bytes...\n");
    void *a = my_malloc(100);
    print_free_list();

    printf("Allocating 500 bytes...\n");
    void *b = my_malloc(500);
    print_free_list();

    printf("Freeing first block...\n");
    my_free(a);
    print_free_list();

    printf("Reallocating second block to 1000 bytes...\n");
    b = my_realloc(b, 1000);
    print_free_list();

    return 0;
}
