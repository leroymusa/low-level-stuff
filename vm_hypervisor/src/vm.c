#include <stdio.h>
#include "vm.h"

void debug_stack(VM *vm) {
    printf("VM Stack (sp=%d): ", vm->sp);
    for (int i = 0; i < vm->sp; ++i) printf("%d ", vm->stack[i]);
    printf("\n");
}

void store_memory(VM *vm, int address, char value) {
    if (address >= 0 && address < MEM_SIZE) vm->memory[address] = value;
}

char load_memory(VM *vm, int address) {
    if (address >= 0 && address < MEM_SIZE) return vm->memory[address];
    return 0;
}

void snapshot_vm(VM *vm, const char *snapshot_file) {
    FILE *file = fopen(snapshot_file, "wb");
    if (file) {
        fwrite(vm, sizeof(VM), 1, file);
        fclose(file);
        printf("Snapshot saved to %s\n", snapshot_file);
    } else {
        printf("Failed to save snapshot to %s\n", snapshot_file);
    }
}

void restore_vm(VM *vm, const char *snapshot_file) {
    FILE *file = fopen(snapshot_file, "rb");
    if (file) {
        fread(vm, sizeof(VM), 1, file);
        fclose(file);
        printf("Snapshot restored from %s\n", snapshot_file);
    } else {
        printf("Failed to restore snapshot from %s\n", snapshot_file);
    }
}