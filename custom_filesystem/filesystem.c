#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "filesystem.h"

#define VIRTUAL_DISK "virtual_disk.img"
#define SUPERBLOCK_SIZE BLOCK_SIZE // Reserve block 0 for the superblock
Superblock superblock;

void format_disk()
{
    FILE *disk = fopen(VIRTUAL_DISK, "wb");
    if (!disk)
    {
        perror("Failed to create virtual disk");
        exit(EXIT_FAILURE);
    }

    // Initialize superblock
    memset(&superblock, 0, sizeof(Superblock));
    fwrite(&superblock, sizeof(Superblock), 1, disk);

    // Fill the rest of the disk with zeros
    uint8_t zero_block[BLOCK_SIZE] = {0};
    for (int i = 0; i < NUM_BLOCKS; ++i)
    {
        fwrite(zero_block, BLOCK_SIZE, 1, disk);
    }

    fclose(disk);
    printf("Disk formatted successfully.\n");
}

int allocate_block()
{
    for (int i = 1; i < NUM_BLOCKS; ++i)
    { // Start from block 1 to skip the superblock
        if (superblock.free_blocks[i] == 0)
        {                                  // Free block
            superblock.free_blocks[i] = 1; // Mark as used
            return i;
        }
    }
    return -1; // No free blocks
}

void create_file(const char *name)
{
    // Check if file already exists
    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (strcmp(superblock.inodes[i].name, name) == 0 && !superblock.inodes[i].is_directory)
        {
            printf("File '%s' already exists.\n", name);
            return;
        }
    }

    // Find a free inode
    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (superblock.inodes[i].name[0] == '\0')
        { // Free inode
            strncpy(superblock.inodes[i].name, name, FILENAME_MAX_LEN);
            superblock.inodes[i].is_directory = 0;
            superblock.inodes[i].creation_time = time(NULL);
            superblock.inodes[i].modification_time = time(NULL);

            // Initialize block indices to 0
            memset(superblock.inodes[i].block_indices, 0, sizeof(superblock.inodes[i].block_indices));

            // Update superblock on disk
            FILE *disk = fopen(VIRTUAL_DISK, "r+b");
            fseek(disk, 0, SEEK_SET);
            fwrite(&superblock, sizeof(Superblock), 1, disk);
            fclose(disk);

            printf("File '%s' created successfully.\n", name);
            return;
        }
    }

    printf("Failed to create file. Maximum number of files reached.\n");
}

void create_directory(const char *name)
{
    // Check if directory already exists
    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (strcmp(superblock.inodes[i].name, name) == 0 && superblock.inodes[i].is_directory)
        {
            printf("Directory '%s' already exists.\n", name);
            return;
        }
    }

    // Find a free inode
    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (superblock.inodes[i].name[0] == '\0')
        { // Free inode
            strncpy(superblock.inodes[i].name, name, FILENAME_MAX_LEN);
            superblock.inodes[i].is_directory = 1;
            superblock.inodes[i].creation_time = time(NULL);
            superblock.inodes[i].modification_time = time(NULL);
            // Initialize block indices to 0
            memset(superblock.inodes[i].block_indices, 0, sizeof(superblock.inodes[i].block_indices));

            // Update superblock on disk
            FILE *disk = fopen(VIRTUAL_DISK, "r+b");
            fseek(disk, 0, SEEK_SET);
            fwrite(&superblock, sizeof(Superblock), 1, disk);
            fclose(disk);

            printf("Directory '%s' created successfully.\n", name);
            return;
        }
    }

    printf("Failed to create directory. Maximum number of directories reached.\n");
}

void delete_file(const char *name)
{
    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (strcmp(superblock.inodes[i].name, name) == 0)
        {
            // Free all allocated blocks
            for (int j = 0; j < MAX_BLOCKS_PER_FILE; ++j)
            {
                if (superblock.inodes[i].block_indices[j] != 0)
                {
                    superblock.free_blocks[superblock.inodes[i].block_indices[j]] = 0;
                }
            }
            memset(&superblock.inodes[i], 0, sizeof(Inode));

            // Update superblock on disk
            FILE *disk = fopen(VIRTUAL_DISK, "r+b");
            fseek(disk, 0, SEEK_SET);
            fwrite(&superblock, sizeof(Superblock), 1, disk);
            fclose(disk);

            printf("File '%s' deleted successfully.\n", name);
            return;
        }
    }
    printf("File '%s' not found.\n", name);
}

void write_file(const char *name, const char *data)
{
    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (strcmp(superblock.inodes[i].name, name) == 0 && !superblock.inodes[i].is_directory)
        {
            int data_length = strlen(data);
            int blocks_needed = (data_length + BLOCK_SIZE - 1) / BLOCK_SIZE;

            int blocks_allocated = 0;
            int current_data_index = 0;

            // Allocate required blocks, skipping reserved superblock space
            for (int j = 1; j < NUM_BLOCKS && blocks_allocated < blocks_needed; ++j)
            { // Start from block 1
                if (superblock.free_blocks[j] == 0)
                { // Free block
                    superblock.inodes[i].block_indices[blocks_allocated] = j;
                    superblock.free_blocks[j] = 1; // Mark block as used
                    FILE *disk = fopen(VIRTUAL_DISK, "r+b");

                    if (disk == NULL)
                    {
                        perror("Error opening virtual disk for writing");
                        return;
                    }

                    // Write data to this block
                    if (fseek(disk, j * BLOCK_SIZE, SEEK_SET) != 0)
                    {
                        perror("Error seeking to write block");
                        fclose(disk);
                        return;
                    }
                    size_t write_size = (data_length - current_data_index) < BLOCK_SIZE ? (data_length - current_data_index) : BLOCK_SIZE;
                    size_t bytes_written = fwrite(data + current_data_index, 1, write_size, disk);
                    fflush(disk); // Ensure data is flushed to disk
                    fclose(disk);

                    // Debug: Check if the correct number of bytes were written
                    printf("Writing to block %d: %.*s, bytes_written: %zu\n", j, (int)write_size, data + current_data_index, bytes_written);

                    if (bytes_written != write_size)
                    {
                        printf("Error: Expected to write %zu bytes but wrote %zu bytes.\n", write_size, bytes_written);
                    }

                    // Verification: Read back the block immediately to verify
                    char verify_buffer[BLOCK_SIZE] = {0};
                    disk = fopen(VIRTUAL_DISK, "rb");
                    if (disk == NULL)
                    {
                        perror("Error opening virtual disk for verification");
                        return;
                    }
                    if (fseek(disk, j * BLOCK_SIZE, SEEK_SET) != 0)
                    {
                        perror("Error seeking to verify block");
                        fclose(disk);
                        return;
                    }
                    size_t bytes_read = fread(verify_buffer, 1, BLOCK_SIZE, disk);
                    fclose(disk);

                    // Debug: Output the verification buffer
                    printf("Verification read from block %d, bytes_read: %zu\n", j, bytes_read);
                    printf("Verification buffer (hex): ");
                    for (int k = 0; k < bytes_read; ++k)
                    {
                        printf("%02x ", (unsigned char)verify_buffer[k]);
                    }
                    printf("\n");

                    blocks_allocated++;
                    current_data_index += write_size;
                }
            }

            if (blocks_allocated < blocks_needed)
            {
                printf("Not enough space to write the file.\n");
                return;
            }

            superblock.inodes[i].size = data_length;
            superblock.inodes[i].modification_time = time(NULL);

            // Debug: Show block indices
            printf("Block indices for '%s':", name);
            for (int k = 0; k < blocks_allocated; ++k)
            {
                printf(" %d", superblock.inodes[i].block_indices[k]);
            }
            printf("\n");

            // Update superblock
            FILE *disk = fopen(VIRTUAL_DISK, "r+b");
            if (disk == NULL)
            {
                perror("Error opening virtual disk to update superblock");
                return;
            }
            fseek(disk, 0, SEEK_SET); // Reserve block 0 for superblock
            fwrite(&superblock, sizeof(Superblock), 1, disk);
            fclose(disk);

            printf("Data written to file '%s'.\n", name);
            return;
        }
    }
    printf("File '%s' not found.\n", name);
}

void read_file(const char *name, char *buffer, uint32_t size)
{
    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (strcmp(superblock.inodes[i].name, name) == 0 && !superblock.inodes[i].is_directory)
        {
            int total_size = superblock.inodes[i].size;              // Total size of the file
            int block_index = superblock.inodes[i].block_indices[0]; // Simplify to only read the first block for now

            // Open the disk for reading
            FILE *disk = fopen(VIRTUAL_DISK, "rb");
            if (disk == NULL)
            {
                perror("Error opening virtual disk");
                return;
            }

            // Move the file pointer to the correct block
            if (fseek(disk, block_index * BLOCK_SIZE, SEEK_SET) != 0)
            {
                perror("Error seeking to block");
                fclose(disk);
                return;
            }

            // Read the entire block directly
            size_t bytes_read = fread(buffer, 1, BLOCK_SIZE, disk);
            fclose(disk);

            // Debug: Print the buffer content in hex
            printf("Reading from block %d, bytes_read: %zu\n", block_index, bytes_read);
            printf("Buffer (hex): ");
            for (int j = 0; j < bytes_read; ++j)
            {
                printf("%02x ", (unsigned char)buffer[j]);
            }
            printf("\n");

            // Null-terminate and print the buffer as a string
            buffer[bytes_read] = '\0';
            printf("Data read from file '%s': %s\n", name, buffer);
            return;
        }
    }

    printf("File '%s' not found or is a directory.\n", name);
}

void list_directory()
{
    printf("Directory listing:\n");
    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (superblock.inodes[i].name[0] != '\0')
        { // Non-empty inode
            printf("%s %s\n", superblock.inodes[i].is_directory ? "[DIR]" : "[FILE]", superblock.inodes[i].name);
        }
    }
}
