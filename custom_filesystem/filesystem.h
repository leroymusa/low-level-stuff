#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>
#include <time.h>

#define DISK_SIZE 1048576 // 1MB
#define BLOCK_SIZE 512
#define NUM_BLOCKS (DISK_SIZE / BLOCK_SIZE)
#define MAX_FILES 128
#define FILENAME_MAX_LEN 32
#define MAX_BLOCKS_PER_FILE (DISK_SIZE / BLOCK_SIZE)

// Structure of the Inode
typedef struct
{
    char name[FILENAME_MAX_LEN];
    uint32_t size;
    uint32_t block_indices[MAX_BLOCKS_PER_FILE]; // Array of block indices for block chaining
    uint32_t is_directory;
    time_t creation_time;
    time_t modification_time;
    uint32_t permissions; // File permissions
} Inode;

// Structure of the Superblock
typedef struct
{
    Inode inodes[MAX_FILES];
    uint8_t free_blocks[NUM_BLOCKS];
} Superblock;

// Function Prototypes
void format_disk();
void create_file(const char *name);
void create_directory(const char *name);
void delete_file(const char *name);
void write_file(const char *name, const char *data);
void read_file(const char *name, char *buffer, uint32_t size);
void list_directory();

#endif /* FILESYSTEM_H */
