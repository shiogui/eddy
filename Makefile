# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2

# Build targets and files
TARGET = eddy
SRCS = main.c
OBJS = $(SRCS:.c=.o)

# OS Detection and flag adjustments
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
    # Automatically query installed SDL3 paths via pkg-config
    CFLAGS += $(shell pkg-config --cflags sdl3)
    LIBS += $(shell pkg-config --libs sdl3)
else ifeq ($(UNAME_S), Darwin) # macOS
    CFLAGS += $(shell pkg-config --cflags sdl3)
    LIBS += $(shell pkg-config --libs sdl3)
else # Windows (MinGW / MSYS2 default environment)
    CFLAGS += -I/usr/local/include
    LIBS += -L/usr/local/lib -lSDL3
endif

# Default build target
all: $(TARGET)

# Link object files into final executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target to wipe generated files
clean:
	rm -f $(TARGET) $(OBJS)

# Run target to quickly build and test
run: all
	./$(TARGET)

.PHONY: all clean run
