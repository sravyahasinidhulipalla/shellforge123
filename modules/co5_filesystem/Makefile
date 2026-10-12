
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

PROGRAMS = file_operations buffered_io mmap_file

all: $(PROGRAMS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(PROGRAMS)

.PHONY: all clean
