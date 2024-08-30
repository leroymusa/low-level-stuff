#ifndef VM_H
#define VM_H

#define STACK_SIZE 256
#define MEM_SIZE 1024
#define MSG_QUEUE_SIZE 10
#define MSG_LENGTH 128

typedef enum {
    HALT, PUSH, POP, ADD, SUB, MUL, DIV, PRINT, JUMP, JUMP_IF_ZERO, SEND, RECV
} OpCode;

typedef struct {
    int stack[STACK_SIZE];
    int sp, ip, running, *program;
    char memory[MEM_SIZE];
    char message_queue[MSG_QUEUE_SIZE][MSG_LENGTH];
    int msg_count;
    int priority; // priority level of vm!
} VM;

void debug_stack(VM *vm);
void store_memory(VM *vm, int address, char value);
char load_memory(VM *vm, int address);
void snapshot_vm(VM *vm, const char *snapshot_file);
void restore_vm(VM *vm, const char *snapshot_file);

#endif /* VM_H */