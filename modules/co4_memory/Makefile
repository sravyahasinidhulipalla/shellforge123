
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

PROGRAMS = memory_layout virtual_memory memory_allocation copy_on_write memory_debug demand_paging

all: $(PROGRAMS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

memory_debug: memory_debug.c
	$(CC) $(CFLAGS) -g -fsanitize=address -fno-omit-frame-pointer $< -o $@

clean:
	rm -f $(PROGRAMS)

.PHONY: all clean
