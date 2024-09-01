#include <stdio.h>
#include <string.h>
#include "hypervisor.h"
#include "vm.h"


void run_vm(Hypervisor *hypervisor, int time_slice) {
    VM *vm = &hypervisor->vms[hypervisor->current_vm];
    int instructions_executed = 0;
    while (vm->running && instructions_executed < time_slice) {
        switch (vm->program[vm->ip++]) {
            case HALT: vm->running = 0; hypervisor->active_vms--; return;
            case PUSH: vm->stack[vm->sp++] = vm->program[vm->ip++]; break;
            case POP: if (vm->sp > 0) vm->sp--; else vm->running = 0, hypervisor->active_vms--; return;
            case ADD: if (vm->sp >= 2) { vm->sp--; vm->stack[vm->sp - 1] += vm->stack[vm->sp]; } else vm->running = 0, hypervisor->active_vms--; return;
            case SUB: if (vm->sp >= 2) { vm->sp--; vm->stack[vm->sp - 1] -= vm->stack[vm->sp]; } else vm->running = 0, hypervisor->active_vms--; return;
            case MUL: if (vm->sp >= 2) { vm->sp--; vm->stack[vm->sp - 1] *= vm->stack[vm->sp]; } else vm->running = 0, hypervisor->active_vms--; return;
            case DIV: if (vm->sp >= 2 && vm->stack[vm->sp - 1] != 0) { vm->sp--; vm->stack[vm->sp - 1] /= vm->stack[vm->sp]; } else vm->running = 0, hypervisor->active_vms--; return;
            case PRINT: if (vm->sp > 0) printf("%d\n", vm->stack[vm->sp - 1]); else vm->running = 0, hypervisor->active_vms--; return;
            case SEND: send_message(hypervisor, hypervisor->current_vm, vm->program[vm->ip++], "Hello from VM"); break;
            case RECV: receive_message(hypervisor, hypervisor->current_vm); break;
            default: vm->running = 0; hypervisor->active_vms--; return;
        }
        debug_stack(vm); instructions_executed++;
    }
}

void create_vm(Hypervisor *hypervisor, int *program, int priority) {
    if (hypervisor->vm_count < MAX_VMS) {
        VM *vm = &hypervisor->vms[hypervisor->vm_count++];
        vm->sp = 0, vm->ip = 0, vm->running = 1, vm->program = program, vm->msg_count = 0, vm->priority = priority;
        memset(vm->memory, 0, MEM_SIZE);
        hypervisor->active_vms++; printf("VM created with priority %d. Total VMs: %d\n", priority, hypervisor->vm_count);
    }
}

void set_vm_priority(Hypervisor *hypervisor, int vm_index, int priority) {
    if (vm_index >= 0 && vm_index < hypervisor->vm_count) {
        hypervisor->vms[vm_index].priority = priority; printf("VM %d priority set to %d\n", vm_index, priority);
    }
}

void switch_vm(Hypervisor *hypervisor) {
    int next_vm = -1, current_priority = hypervisor->vms[hypervisor->current_vm].priority;

    for (int i = 1; i <= hypervisor->vm_count; ++i) {
        int index = (hypervisor->current_vm + i) % hypervisor->vm_count;
        VM *candidate = &hypervisor->vms[index];

        if (candidate->running && (candidate->priority > current_priority || next_vm == -1)) {
            next_vm = index; current_priority = candidate->priority;
        }
    }

    if (next_vm != -1) {
        hypervisor->current_vm = next_vm;
        printf("Switched to VM %d with priority %d\n", next_vm, hypervisor->vms[next_vm].priority);
    } else {
        printf("No running VMs found.\n");
    }
}

void send_message(Hypervisor *hypervisor, int sender_vm, int receiver_vm, const char *message) {
    VM *receiver = &hypervisor->vms[receiver_vm];
    if (receiver->msg_count < MSG_QUEUE_SIZE) {
        strncpy(receiver->message_queue[receiver->msg_count++], message, MSG_LENGTH);
        printf("VM %d sent message to VM %d: %s\n", sender_vm, receiver_vm, message);
    } else {
        printf("Message queue full for VM %d\n", receiver_vm);
    }
}

void receive_message(Hypervisor *hypervisor, int vm_index) {
    VM *vm = &hypervisor->vms[vm_index];
    if (vm->msg_count > 0) {
        printf("VM %d received message: %s\n", vm_index, vm->message_queue[0]);
        for (int i = 1; i < vm->msg_count; ++i) strcpy(vm->message_queue[i - 1], vm->message_queue[i]);
        vm->msg_count--;
    } else {
        printf("No messages for VM %d\n", vm_index);
    }
}

int main() {
    Hypervisor hypervisor = { .current_vm = 0, .vm_count = 0, .active_vms = 0 };

    int program1[] = { PUSH, 10, PUSH, 5, SUB, PRINT, PUSH, 7, MUL, PRINT, PUSH, 10, DIV, PRINT, SEND, 1, HALT };
    int program2[] = { PUSH, 8, PUSH, 2, ADD, PRINT, RECV, HALT };

    create_vm(&hypervisor, program1, 1); create_vm(&hypervisor, program2, 2);

    while (hypervisor.active_vms > 0) {
        run_vm(&hypervisor, TIME_SLICE); switch_vm(&hypervisor);
    }

    snapshot_vm(&hypervisor.vms[0], "vm0_snapshot.bin");

    restore_vm(&hypervisor.vms[0], "vm0_snapshot.bin"); hypervisor.active_vms++;
    run_vm(&hypervisor, TIME_SLICE);

    return 0;
}
