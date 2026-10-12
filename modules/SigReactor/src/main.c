
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>

#include "signal_manager.h"
#include "process_manager.h"
#include "ipc.h"
#include "reactor.h"

static void show_banner(void)
{
    printf("\n");
    printf("==================================================\n");
    printf("             SHELLFORGE OS LAB\n");
    printf("       Signal-Driven Operating System Demos\n");
    printf("==================================================\n");
    printf("[INIT] Process Manager         : READY\n");
    printf("[INIT] IPC Manager             : READY\n");
    printf("[INIT] Signal Manager          : READY\n");
    printf("[INIT] Event Reactor           : READY\n");
    printf("==================================================\n");
}

static void menu(void)
{
    printf("\n");
    printf("---------------- SHELLFORGE MENU -----------------\n");
    printf("1. Process Creation + Synchronization\n");
    printf("2. Anonymous Pipe IPC\n");
    printf("3. Signal Event Reactor\n");
    printf("4. Named Pipe (FIFO)\n");
    printf("5. Process Group\n");
    printf("6. System Information\n");
    printf("7. SIGCHLD Worker Monitor\n");
    printf("8. Memory Management Lab\n");
    printf("9. File System Lab\n");
    printf("10. Concurrency & Synchronization Lab\n");
    printf("0. Exit\n");
    printf("--------------------------------------------------\n");
    printf("Enter choice: ");
    fflush(stdout);
}

static void system_info(void)
{
    printf("\n[SYSTEM] Operating System : Linux\n");
    printf("[SYSTEM] Process ID       : %d\n", getpid());
    printf("[SYSTEM] Parent Process ID: %d\n", getppid());
    printf("[SYSTEM] Process Group ID : %d\n", getpgrp());
    printf("[SYSTEM] User Space       : ACTIVE\n");
}

static void run_signal_reactor(void)
{
    printf("\n[SIGNAL] Reactor listening for 10 seconds.\n");
    printf("[SIGNAL] Send SIGUSR1 or SIGUSR2 from another terminal.\n");
    printf("[SIGNAL] Reactor PID: %d\n", getpid());

    for (int i = 0; i < 100; i++)
        reactor_run_once();

    printf("\n[SIGNAL] Listening period completed.\n");
}

static void run_sigchld_demo(void)
{
    printf("\n[SIGCHLD] Starting worker-monitor demonstration.\n");
    signal_worker_demo();

    for (int i = 0; i < 50; i++)
        reactor_run_once();

    printf("[SIGCHLD] Monitoring completed.\n");
}

static void launch_demo(const char *module,
                        const char *program,
                        const char *label)
{
    const char *home = getenv("HOME");

    if (home == NULL)
    {
        fprintf(stderr, "[ERROR] HOME environment variable is missing.\n");
        return;
    }

    char path[PATH_MAX];

    int length = snprintf(
        path,
        sizeof(path),
        "%s/shellforge/modules/%s/%s",
        home,
        module,
        program
    );

    if (length < 0 || (size_t)length >= sizeof(path))
    {
        fprintf(stderr, "[ERROR] Executable path is too long.\n");
        return;
    }

    printf("\n[LAB] Selected: %s\n", label);

    if (access(path, X_OK) == -1)
    {
        printf("[LAB] Executable not found or not executable:\n%s\n", path);
        printf("[LAB] Build the corresponding module first.\n");
        return;
    }

    printf("[LAB] Sending task to the worker launcher.\n");

    if (reactor_launch_worker(path, label) == 0)
        printf("[LAB] Worker completed successfully.\n");
    else
        printf("[LAB] Worker failed or did not exit normally.\n");
}

static void memory_lab(void)
{
    int choice;

    printf("\n========== MEMORY MANAGEMENT LAB ================\n");
    printf("1. Memory Layout\n");
    printf("2. Virtual Memory\n");
    printf("3. Dynamic Memory Allocation\n");
    printf("4. Copy-on-Write\n");
    printf("5. Memory Debugging\n");
    printf("6. Demand Paging\n");
    printf("0. Return to Main Menu\n");
    printf("Select demonstration: ");

    if (scanf("%d", &choice) != 1)
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        printf("[ERROR] Invalid input.\n");
        return;
    }

    switch (choice)
    {
        case 1:
            launch_demo("co4_memory", "memory_layout", "Memory Layout");
            break;
        case 2:
            launch_demo("co4_memory", "virtual_memory", "Virtual Memory");
            break;
        case 3:
            launch_demo("co4_memory", "memory_allocation",
                        "Dynamic Memory Allocation");
            break;
        case 4:
            launch_demo("co4_memory", "copy_on_write", "Copy-on-Write");
            break;
        case 5:
            launch_demo("co4_memory", "memory_debug", "Memory Debugging");
            break;
        case 6:
            launch_demo("co4_memory", "demand_paging", "Demand Paging");
            break;
        case 0:
            break;
        default:
            printf("[ERROR] Invalid memory demonstration.\n");
    }
}

static void filesystem_lab(void)
{
    int choice;

    printf("\n============== FILE SYSTEM LAB ==================\n");
    printf("1. File Operations\n");
    printf("2. Buffered and Unbuffered I/O\n");
    printf("3. Memory-Mapped File I/O\n");
    printf("0. Return to Main Menu\n");
    printf("Select demonstration: ");

    if (scanf("%d", &choice) != 1)
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        printf("[ERROR] Invalid input.\n");
        return;
    }

    switch (choice)
    {
        case 1:
            launch_demo("co5_filesystem", "file_operations",
                        "File Operations");
            break;
        case 2:
            launch_demo("co5_filesystem", "buffered_io",
                        "Buffered and Unbuffered I/O");
            break;
        case 3:
            launch_demo("co5_filesystem", "mmap_file",
                        "Memory-Mapped File I/O");
            break;
        case 0:
            break;
        default:
            printf("[ERROR] Invalid file-system demonstration.\n");
    }
}

static void concurrency_lab(void)
{
    int choice;

    printf("\n======= CONCURRENCY & SYNCHRONIZATION LAB =======\n");
    printf("1. POSIX Threads\n");
    printf("2. Mutex Synchronization\n");
    printf("3. Condition Variables\n");
    printf("4. Counting Semaphore\n");
    printf("5. Deadlock Prevention\n");
    printf("0. Return to Main Menu\n");
    printf("Select demonstration: ");

    if (scanf("%d", &choice) != 1)
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        printf("[ERROR] Invalid input.\n");
        return;
    }

    switch (choice)
    {
        case 1:
            launch_demo("co6_concurrency", "threads", "POSIX Threads");
            break;
        case 2:
            launch_demo("co6_concurrency", "mutex_sync",
                        "Mutex Synchronization");
            break;
        case 3:
            launch_demo("co6_concurrency", "condition_variable",
                        "Condition Variables");
            break;
        case 4:
            launch_demo("co6_concurrency", "semaphore_demo",
                        "Counting Semaphore");
            break;
        case 5:
            launch_demo("co6_concurrency", "deadlock_safe",
                        "Deadlock Prevention");
            break;
        case 0:
            break;
        default:
            printf("[ERROR] Invalid concurrency demonstration.\n");
    }
}

int main(void)
{
    show_banner();

    if (reactor_init() == -1)
    {
        perror("reactor_init");
        return 1;
    }

    if (signal_manager_init(reactor_get_fd()) == -1)
    {
        perror("signal_manager_init");
        reactor_close();
        return 1;
    }

    printf("\n[REACTOR] Shellforge is running.\n");
    printf("[REACTOR] PID: %d\n", getpid());

    int running = 1;

    while (running)
    {
        menu();

        int choice;

        if (scanf("%d", &choice) != 1)
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("[ERROR] Enter a valid menu number.\n");
            continue;
        }

        switch (choice)
        {
            case 1:
                process_demo();
                break;
            case 2:
                pipe_demo();
                break;
            case 3:
                run_signal_reactor();
                break;
            case 4:
                fifo_demo();
                break;
            case 5:
                process_group_demo();
                break;
            case 6:
                system_info();
                break;
            case 7:
                run_sigchld_demo();
                break;
            case 8:
                memory_lab();
                break;
            case 9:
                filesystem_lab();
                break;
            case 10:
                concurrency_lab();
                break;
            case 0:
                printf("\n[SHUTDOWN] Stopping Shellforge...\n");
                running = 0;
                break;
            default:
                printf("[ERROR] Invalid option.\n");
        }
    }

    signal_manager_cleanup();
    reactor_close();

    printf("[SHUTDOWN] Shellforge stopped safely.\n");
    return 0;
}
