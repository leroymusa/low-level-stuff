# Custom Filesystem (WIP)

This is a custom filesystem implementation that reads and writes files to a virtual disk (`virtual_disk.img`). **This project is a work in progress** and may not function correctly in all cases.

## Features
- Format a virtual disk
- Create directories and files
- Write to and read from files
- List directory contents
- Delete files

## How to Use
1. **Compile the Project**:
   ```bash
   gcc main.c filesystem.c -o custom_filesystem
   ```

2. **Run the Program**:
   ```bash
   ./custom_filesystem
   ```
   This will execute the `main.c` script, which demonstrates the basic functionality of the filesystem.

## Current Status
- Basic file and directory operations are implemented.
- Data writing is verified, but reading has known issues (in progress).
- Error handling and edge cases are not fully covered.

## To-Do
- Fix the issue where `read_file` returns an empty buffer.
- Improve error handling and file management.
- Extend support for more complex file operations.