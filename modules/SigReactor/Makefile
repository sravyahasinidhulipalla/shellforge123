CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -Iinclude

TARGET = sigreactor

SRC = src/main.c \
      src/signal_manager.c \
      src/process_manager.c \
      src/ipc.c \
      src/reactor.c \
      src/event_dispatcher.c

all:
	@echo "========================================"
	@echo "       BUILDING SIGREACTOR"
	@echo "========================================"
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
	@echo "[BUILD] Compilation successful."
	@echo "[BUILD] Executable: $(TARGET)"

clean:
	rm -f $(TARGET)
	@echo "[CLEAN] Build files removed."

run: all
	./$(TARGET)
