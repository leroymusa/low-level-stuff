#include "filesystem.h"
#include <stdio.h>

int main()
{
    // Format the disk
    format_disk();

    // Create a directory
    create_directory("my_directory");

    // Create a file and write data
    create_file("file1.txt");
    write_file("file1.txt", "Hello, World!");

    // Read data from the file
    char buffer[512];
    read_file("file1.txt", buffer, 13); // Read 13 bytes

    // List the directory
    list_directory();

    // Delete the file
    delete_file("file1.txt");

    // List the directory again
    list_directory();

    return 0;
}

// debug! debug! debug!
/*
#include <stdio.h>
#include <string.h>

#define VIRTUAL_DISK "virtual_disk.img"
#define BLOCK_SIZE 512

int main() {
    // Write "Hello, World!" directly to block 0
    FILE *disk = fopen(VIRTUAL_DISK, "r+b");
    if (disk == NULL) {
        perror("Error opening virtual disk");
        return 1;
    }

    const char *data = "Hello, World!";
    fseek(disk, 0 * BLOCK_SIZE, SEEK_SET);  // Move to block 0
    size_t bytes_written = fwrite(data, 1, strlen(data), disk);
    fclose(disk);

    printf("Direct write to disk, bytes_written: %zu\n", bytes_written);

    // Read back the data directly from block 0
    char buffer[BLOCK_SIZE] = {0};
    disk = fopen(VIRTUAL_DISK, "rb");
    if (disk == NULL) {
        perror("Error opening virtual disk");
        return 1;
    }

    fseek(disk, 0 * BLOCK_SIZE, SEEK_SET);  // Move to block 0
    size_t bytes_read = fread(buffer, 1, strlen(data), disk);
    fclose(disk);

    printf("Direct read from disk, bytes_read: %zu\n", bytes_read);
    printf("Data read: %s\n", buffer);

    return 0;
}
*/
