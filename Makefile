# Configurações do compilador e alvos
CC       := gcc
SRC_DIR  := ./src
TARGET   := ./eddy
CFLAGS 	 := -Wall -Wextra $(shell pkg-config --cflags sdl3) -I./include
LIBS     := $(shell pkg-config --libs sdl3)
SRCS     := $(shell find $(SRC_DIR) -name "*.c")

.SILENT:
.PHONY: all, clean, build, run

# Default target for build
all: build

# Cleanup
clean:
	rm -f $(TARGET)

# Builds the target binary
build: clean
	@echo "Files to build: $(SRCS)"
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

# Run
run: build
	./eddy