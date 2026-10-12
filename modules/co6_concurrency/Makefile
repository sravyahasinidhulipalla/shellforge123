
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread

PROGRAMS = threads mutex_sync condition_variable semaphore_demo deadlock_safe

all: $(PROGRAMS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(PROGRAMS)

.PHONY: all clean
