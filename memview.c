// William Ng
// Custom Linux Shell Command: memview

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
    fprintf(stderr, "Usage: %s [-p pid] [-h]\n", progname);
}

// Function to view memory mappings from given path
void view_memory_maps(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        perror("Error opening memory maps file");
        return;
    }

    char line[512];
    printf("%-35s %-20s %-10s %s\n", "Address Range", "Permissions", "Size", "Region");
    printf("--------------------------------------------------------------------------------------\n");

    while (fgets(line, sizeof(line), fp) && !stop_requested) {
        char addr[64], perms[8], dev[8], mapname[256] = "[anonymous]";
        unsigned long offset, inode;
        sscanf(line, "%63[^ ] %7s %lx %7s %lu %255[^"]", addr, perms, &offset, dev, &inode, mapname);

        unsigned long start, end;
        sscanf(addr, "%lx-%lx", &start, &end);

        printf("%-35s %-20s %-10lu %s\n", addr, perms, end - start, mapname);
    }

    fclose(fp);
}

int main(int argc, char *argv[]) {
    int opt;
    char maps_path[256] = "/proc/self/maps"; // Default to current process

    // Set up signal handling
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    // Parse command-line options
    while ((opt = getopt(argc, argv, "p:h")) != -1) {
        switch (opt) {
            case 'p':
                snprintf(maps_path, sizeof(maps_path), "/proc/%s/maps", optarg);
                break;
            case 'h':
            default:
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    printf("Viewing memory map: %s\n", maps_path);

    if (!stop_requested)
        view_memory_maps(maps_path);

    if (stop_requested)
        printf("\nSIGINT received. Exiting Now.\n");

    return 0;
}
// gcc -o memview memview.c
// ./memview -p PID to view specific process
// ./memview -h for help
// ./memview to view memory
