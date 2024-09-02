#ifndef HYPERVISOR_H
#define HYPERVISOR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "vm.h"

#define MAX_VMS 10
#define TIME_SLICE 5

    typedef struct
    {
        VM vms[MAX_VMS];
        int current_vm, vm_count, active_vms;
    } Hypervisor;

    void run_vm(Hypervisor *hypervisor, int time_slice);
    void create_vm(Hypervisor *hypervisor, int *program, int priority);
    void switch_vm(Hypervisor *hypervisor);
    void destroy_vm(Hypervisor *hypervisor, int vm_index);
    void send_message(Hypervisor *hypervisor, int sender_vm, int receiver_vm, const char *message);
    void receive_message(Hypervisor *hypervisor, int vm_index);
    void set_vm_priority(Hypervisor *hypervisor, int vm_index, int priority);

#ifdef __cplusplus
}
#endif

#endif /* HYPERVISOR_H */