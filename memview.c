// William Ng - CSC 332
// Custom Linux Shell Command: memview

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <getopt.h>

// Global variable to detect if SIGINT (Ctrl+C) was received
volatile sig_atomic_t stop_requested = 0;

// Signal handler function for SIGINT
void handle_sigint(int sig) {
    (void)sig; // Mark parameter as unused
    stop_requested = 1; // Set stop flag
}

// Prints usage/help message
void print_usage(const char *progname) {
    fprintf(stderr, "Usage: %s [-p pid]\n", progname);
}

// Function to open, map, and display memory map file
void view_memory_maps(const char *path) {
    int fd = open(path, O_RDONLY); // Open the memory maps file
    if (fd == -1) {
        perror("Error opening maps file");
        return;
    }

    struct stat statbuf;
    if (fstat(fd, &statbuf) == -1) { // Get file metadata (e.g., size)
        perror("Error getting file size");
        close(fd);
        return;
    }

    if (statbuf.st_size == 0) { // If file is empty
        fprintf(stderr, "Empty maps file.\n");
        close(fd);
        return;
    }

    // Map the file into memory
    char *file_map = mmap(NULL, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_map == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return;
    }

    // Output the contents of the memory map file to stdout
    write(STDOUT_FILENO, file_map, statbuf.st_size);

    // Cleanup: Unmap and close file descriptor
    munmap(file_map, statbuf.st_size);
    close(fd);
}

int main(int argc, char *argv[]) {
    int opt;
    char maps_path[256] = "/proc/self/maps"; // Default path to current process maps

    // Set up signal handling for Ctrl+C (SIGINT)
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Parse command line options
    while ((opt = getopt(argc, argv, "p:h")) != -1) {
        switch (opt) {
            case 'p':
                // If user provides a process ID, adjust the path to /proc/<pid>/maps
                snprintf(maps_path, sizeof(maps_path), "/proc/%s/maps", optarg);
                break;
            case 'h':
            default:
                // Show usage and exit if invalid option or help requested
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Inform the user what memory map we are viewing
    printf("Viewing memory map: %s\n", maps_path);

    // Only proceed if SIGINT has not been received
    if (!stop_requested)
        view_memory_maps(maps_path);

    // If Ctrl+C was pressed
    if (stop_requested)
        printf("\nSIGINT received. Exiting gracefully.\n");

    return 0;
}

// gcc -o memview memview.c
// ./memview -p to view process
// ./memview -h for help
// ./memview to view memory
