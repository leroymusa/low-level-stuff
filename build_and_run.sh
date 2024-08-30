#!/bin/bash

# Script I made to make it easier for me to test

echo "Available assembly files:"
asm_files=(*.asm)
select asm_file in "${asm_files[@]}"; do

    if [[ -n $asm_file ]]; then
        echo "You selected: $asm_file"

        nasm -f elf64 "$asm_file" -o "${asm_file%.asm}.o"

        if [ $? -ne 0 ]; then
            echo "Assembly failed."
            exit 1
        fi

        ld "${asm_file%.asm}.o" -o "${asm_file%.asm}"
        
        if [ $? -ne 0 ]; then
            echo "Linking failed."
            exit 1
        fi

        echo "Running the executable:"
        ./"${asm_file%.asm}"

        exit 0
    else
        echo "Invalid! Please choose a valid option."
    fi
done
