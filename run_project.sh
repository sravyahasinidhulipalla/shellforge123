
#!/bin/bash

ROOT="$HOME/shellforge"

while true; do
    echo
    echo "=================================="
    echo "   SHELLFORGE - PROJECT MENU"
    echo "=================================="
    echo "1. CO-4: Memory Management"
    echo "2. CO-5: File Systems"
    echo "3. CO-6: Concurrency"
    echo "4. SigReactor"
    echo "0. Exit"
    echo "=================================="
    read -rp "Enter your choice: " choice

    case "$choice" in
        1)
            cd "$ROOT/modules/co4_memory" || continue
            make || continue
            echo
            echo "Available CO-4 programs:"
            echo "1. memory_layout"
            echo "2. virtual_memory"
            echo "3. memory_allocation"
            echo "4. copy_on_write"
            echo "5. memory_debug"
            echo "6. demand_paging"
            read -rp "Select a program: " program
            case "$program" in
                1) ./memory_layout ;;
                2) ./virtual_memory ;;
                3) ./memory_allocation ;;
                4) ./copy_on_write ;;
                5) ./memory_debug ;;
                6) ./demand_paging ;;
                *) echo "Invalid program choice." ;;
            esac
            ;;
        2)
            cd "$ROOT/modules/co5_filesystem" || continue
            make || continue
            echo
            echo "Available CO-5 programs:"
            echo "1. file_operations"
            echo "2. buffered_io"
            echo "3. mmap_file"
            read -rp "Select a program: " program
            case "$program" in
                1) ./file_operations ;;
                2) ./buffered_io ;;
                3) ./mmap_file ;;
                *) echo "Invalid program choice." ;;
            esac
            ;;
        3)
            cd "$ROOT/modules/co6_concurrency" || continue
            make || continue
            echo
            echo "Available CO-6 programs:"
            echo "1. threads"
            echo "2. mutex_sync"
            echo "3. condition_variable"
            echo "4. semaphore_demo"
            echo "5. deadlock_safe"
            read -rp "Select a program: " program
            case "$program" in
                1) ./threads ;;
                2) ./mutex_sync ;;
                3) ./condition_variable ;;
                4) ./semaphore_demo ;;
                5) ./deadlock_safe ;;
                *) echo "Invalid program choice." ;;
            esac
            ;;
        4)
            cd "$ROOT/modules/SigReactor" || continue
            make || continue
            ./sigreactor
            ;;
        0)
            echo "Exiting Shellforge."
            break
            ;;
        *)
            echo "Invalid choice. Please try again."
            ;;
    esac
done
