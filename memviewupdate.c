// William Ng
// custom linux command: memview
// ============================================
// memviewupdate.c - For macOS
// ============================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <signal.h>
#include <getopt.h>

// Global variable to detect Ctrl+C
volatile sig_atomic_t stop_requested = 0;

// Signal handler to set stop flag
void handle_sigint(int sig) {
    (void)sig;
    stop_requested = 1;
}

// Prints usage message
void print_usage(const char *progname) {
    fprintf(stderr, "Usage: %s [-h]\n", progname);
}

// Print memory protections as rwx format
void print_protections(vm_prot_t prot) {
    printf("%c%c%c",
           (prot & VM_PROT_READ) ? 'r' : '-',
           (prot & VM_PROT_WRITE) ? 'w' : '-',
           (prot & VM_PROT_EXECUTE) ? 'x' : '-');
}

// Function to view memory regions using Mach APIs
void view_memory_regions() {
    mach_port_t task = mach_task_self();
    mach_vm_address_t address = 0;
    mach_vm_size_t size = 0;
    kern_return_t kr;
    vm_region_basic_info_data_64_t info;
    mach_msg_type_number_t count;
    memory_object_name_t object;

    printf("%-35s %-10s %-10s\n", "Address Range", "Size", "Permissions");
    printf("--------------------------------------------------------------\n");

    while (!stop_requested) {
        count = VM_REGION_BASIC_INFO_COUNT_64;
        kr = mach_vm_region(task, &address, &size, VM_REGION_BASIC_INFO_64,
                            (vm_region_info_t)&info, &count, &object);

        if (kr != KERN_SUCCESS)
            break;

        printf("0x%016llx-0x%016llx %-10llu ", address, address + size, size);
        print_protections(info.protection);
        printf("\n");

        address += size;
    }
}

int main(int argc, char *argv[]) {
    int opt;

    // Set up signal handling
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    // Parse command-line options
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
            case 'h':
            default:
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    printf("Viewing memory regions (macOS version)...\n");

    if (!stop_requested)
        view_memory_regions();

    if (stop_requested)
        printf("\nSIGINT received. Exiting Now.\n");

    return 0;
}

// gcc -o memview memviewupdate.c
// ./memview
