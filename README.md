
# Shellforge — Linux Operating System Demonstrations

## Overview

Shellforge is a C-based project demonstrating Linux operating system concepts through practical programs.

## Modules

### CO-4: Memory Management

Folder: `modules/co4_memory/`

- `memory_layout.c` — Code, data, BSS, stack, and heap
- `virtual_memory.c` — Virtual address observations
- `memory_allocation.c` — Dynamic memory allocation using malloc and free
- `copy_on_write.c` — Copy-on-write behavior after fork()
- `memory_debug.c` — Memory debugging with AddressSanitizer
- `demand_paging.c` — Page-fault observations using memory mapping

### CO-5: File Systems

Folder: `modules/co5_filesystem/`

- `file_operations.c` — Linux file operations using system calls
- `buffered_io.c` — Buffered and unbuffered file I/O
- `mmap_file.c` — Memory-mapped file I/O

### CO-6: Concurrency and Synchronization

Folder: `modules/co6_concurrency/`

- `threads.c` — POSIX threads
- `mutex_sync.c` — Shared-data protection using mutexes
- `condition_variable.c` — Condition-variable coordination
- `semaphore_demo.c` — Counting semaphore
- `deadlock_safe.c` — Consistent mutex-locking order

### SigReactor

Folder: `modules/SigReactor/`

A separate C program demonstrating signals, process management, IPC, and event handling.

## Build Instructions

Build each module separately from its folder.

CO-4:

```bash
cd ~/shellforge/modules/co4_memory
make
```

CO-5:

```bash
cd ~/shellforge/modules/co5_filesystem
make
```

CO-6:

```bash
cd ~/shellforge/modules/co6_concurrency
make
```

SigReactor:

```bash
cd ~/shellforge/modules/SigReactor
make
```

## Run Examples

For example:

```bash
cd ~/shellforge/modules/co4_memory
./demand_paging
```

```bash
cd ~/shellforge/modules/co5_filesystem
./mmap_file
```

```bash
cd ~/shellforge/modules/co6_concurrency
./mutex_sync
```

## Notes

These programs are educational demonstrations of operating-system concepts. They do not implement or replace Linux kernel subsystems such as the virtual memory manager or VFS.
