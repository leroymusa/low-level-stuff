#!/bin/bash

BOOTLOADER_DIR="bootloader"
KERNEL_DIR="kernel"
ISO_DIR="iso"
LINKER_SCRIPT="linker.ld"
BOOTLOADER_ASM="$BOOTLOADER_DIR/bootloader.asm"
BOOTLOADER_BIN="$BOOTLOADER_DIR/bootloader.bin"
KERNEL_C="$KERNEL_DIR/kernel.c"
KERNEL_O="$KERNEL_DIR/kernel.o"
KERNEL_BIN="$KERNEL_DIR/kernel.bin"
FLOPPY_IMG="$ISO_DIR/floppy.img"
ISO_IMG="$ISO_DIR/os-image.iso"

echo "Compiling bootloader..."
nasm -f bin $BOOTLOADER_ASM -o $BOOTLOADER_BIN
if [ $? -ne 0 ]; then
    echo "Error: Failed to compile bootloader."
    exit 1
fi

echo "Compiling kernel..."
gcc -m32 -ffreestanding -fno-pic -c $KERNEL_C -o $KERNEL_O
if [ $? -ne 0 ]; then
    echo "Error: Failed to compile kernel."
    exit 1
fi

echo "Linking kernel..."
ld -m elf_i386 -T $LINKER_SCRIPT -o $KERNEL_BIN $KERNEL_O --oformat binary
if [ $? -ne 0 ]; then
    echo "Error: Failed to link kernel."
    exit 1
fi

echo "Creating floppy image..."
dd if=/dev/zero of=$FLOPPY_IMG bs=512 count=2880
dd if=$BOOTLOADER_BIN of=$FLOPPY_IMG conv=notrunc
dd if=$KERNEL_BIN of=$FLOPPY_IMG bs=512 seek=1 conv=notrunc

echo "Creating ISO image..."
genisoimage -R -b floppy.img -o $ISO_IMG $ISO_DIR/
if [ $? -ne 0 ]; then
    echo "Error: Failed to create ISO image."
    exit 1
fi

echo "Running in QEMU..."
qemu-system-i386 -cdrom $ISO_IMG

echo "Build and run completed successfully."