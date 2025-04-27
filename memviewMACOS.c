// William Ng - CSC 332
// Custom Linux Shell Command: memview (macOS version)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <mach/mach.h>
#include <mach/mach_vm.h>

// Global variable to detect if SIGINT was received
volatile sig_atomic_t stop_requested = 0;

// Signal handler function for SIGINT
void handle_sigint(int sig) {
    (void)sig; // Mark parameter as unused
    stop_requested = 1; // Set stop flag
}

// Prints usage/help message
void print_usage(const char *progname) {
    fprintf(stderr, "Usage: %s [-h]\n", progname);
}

// Function to view memory regions using macOS Mach APIs
void view_memory_regions() {
    mach_port_t task = mach_task_self();
    mach_vm_address_t address = 0;
    mach_vm_size_t size = 0;
    kern_return_t kr;
    vm_region_basic_info_data_64_t info;
    mach_msg_type_number_t count;
    memory_object_name_t object;

    printf("Memory regions (macOS version):\n");

    while (!stop_requested) {
        count = VM_REGION_BASIC_INFO_COUNT_64;
        kr = mach_vm_region(task, &address, &size, VM_REGION_BASIC_INFO_64,
                            (vm_region_info_t)&info, &count, &object);

        if (kr != KERN_SUCCESS)
            break;

        printf("Start: 0x%016llx End: 0x%016llx Size: %-10llu bytes\n",
               address, address + size, size);

        address += size; // Move to next region
    }
}

int main(int argc, char *argv[]) {
    int opt;

    // Set up signal handling (SIGINT)
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Parse command line options
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
            case 'h':
            default:
                // Show usage and exit if invalid option or help requested
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Inform the user what we are viewing
    printf("Viewing memory regions of the current process (macOS version)...\n");

    // Only proceed if SIGINT has not been received
    if (!stop_requested)
        view_memory_regions();

    if (stop_requested)
        printf("\nSIGINT received. Exiting gracefully.\n");

    return 0;
}

