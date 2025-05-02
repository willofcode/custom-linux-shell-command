
# `memview` - Memory Map Viewer for Linux Processes

`memview` is a C-based utility that prints the memory mappings of a process by reading from the `/proc/<pid>/maps` file using low-level system calls. It displays each memory region's address range, permissions, size, and associated region name (or `[anonymous]` if unnamed).

---

## Features

- Displays memory layout of a specified process.
- Uses **low-level POSIX system calls** (`open`, `read`, `dprintf`, `close`) instead of high-level I/O.
- Handles `SIGINT` (Ctrl+C) for interactive use.
- Provides optional process targeting via `-p` flag.
- Defaults to inspecting its own process if no PID is specified.

---

## File Overview

```c
memview.c
```

---

## 🔧 Core Functions & Their Purpose

### `handle_sigint(int sig)`
Handles Ctrl+C (SIGINT) interrupts gracefully by setting a global flag `stop_requested`.

### `print_usage(const char *progname)`
Displays usage instructions if incorrect flags are provided or `-h` is passed.

### `view_memory_maps(const char *path)`
Main logic to open, read, and parse the `/proc/<pid>/maps` file:
- Opens the file using `open()`
- Reads with `read()`
- Parses each line using `strtok()` and `sscanf()`
- Prints details using `dprintf()` to standard output

Parsed fields:
- `Address Range`: memory address span
- `Permissions`: read/write/execute/shared/private permissions
- `Size`: memory region size in bytes
- `Region`: mapped file or `[anonymous]` if unnamed

---

## Program Flow (`main` function)

1. **Signal Handler Setup**
   - Registers `SIGINT` to enable termination with Ctrl+C.

2. **Command-Line Parsing**
   - `-p <pid>`: specify a process ID to view its memory map.
   - `-h`: print help message.

3. **Memory Map Viewing**
   - Defaults to `/proc/self/maps` if no PID is provided.
   - Passes the path to `view_memory_maps()` for processing.

4. **Exit Handling**
   - Prints message if SIGINT is received before completion.

---

## Compilation

Compile with `gcc`:

```bash
gcc -o memview memview.c
```

---

## Usage

```bash
./memview                 # View memory map of the current process
./memview -p 1234         # View memory map of process with PID 1234
./memview -h              # Print help/usage message
```

---

## Sample Output

```bash
Viewing memory map: /proc/self/maps
Address Range                       Permissions         Size       Region
--------------------------------------------------------------------------------------
00400000-0040b000                   r--p                45056      /usr/bin/memview
0040b000-0040d000                   r-xp                8192       /usr/bin/memview
...
7ffd9c06c000-7ffd9c08d000          rw-p                135168     [stack]
```

---

## Test

turn the file into exexutable 

```bash
chmod +x test_memview.sh
```
run 

```bash
./test_memview.sh
```
---

## Notes

- Designed for **Linux**, as it uses `/proc/<pid>/maps`.
- Designed for **educational and debugging** purposes.
- Requires appropriate permissions to access other processes' `/proc/<pid>/maps`.
