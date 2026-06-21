# Configurações do compilador e alvos
CC       := gcc
SRC_DIR  := ./src
TARGET   := ./eddy
CFLAGS 	 := -Wall -Wextra $(shell pkgconf --cflags sdl3) $(shell pkgconf --cflags freetype2) -I./include
LIBS     := $(shell pkgconf --libs sdl3) $(shell pkgconf --libs freetype2)
SRCS     := $(shell find $(SRC_DIR) -name "*.c")

DEMO_FILE := assets/code-files/example.html

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
	./eddy $(DEMO_FILE)