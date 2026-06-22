# Compiler config and targets
CC       	:= clang
WARNINGS 	:= -Wall -Wextra

# Libs to include
INCLUDES 	:= $(shell pkgconf --cflags sdl3)
INCLUDES 	+= $(shell pkgconf --cflags freetype2)
INCLUDES 	+= $(shell pkgconf --cflags fontconfig)

# Libs to link
LIBS     	:= $(shell pkgconf --libs sdl3)
LIBS     	+= $(shell pkgconf --libs freetype2)
LIBS     	+= $(shell pkgconf --libs fontconfig)

# Project specific config
SRC_DIR  	:= ./src
TARGET   	:= ./eddy
SRCS     	:= $(shell find $(SRC_DIR) -name "*.c")
DEMO_FILE 	:= assets/code-files/example.html
INCLUDES 	+= -I./include

.PHONY: all, clean, build, run

# Default target for build (when we run just 'make')
all: build

# Cleanup
clean:
	rm -f $(TARGET)

# Builds the target binary
build: clean
	$(CC) $(WARNINGS) $(INCLUDES) $(SRCS) -o $(TARGET) $(LIBS)

# Run
run: build
	$(shell $(TARGET) $(DEMO_FILE))