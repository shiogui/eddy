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
P_INCLUDES 	:= -I./include

.SILENT: clean
.PHONY: all, clean, build, run

# Default target for build (when we run just 'make')
all: build

# Cleanup
clean:
	rm -f $(TARGET)

# Builds the target binary
build: clean
	$(CC) $(WARNINGS) $(sort $(INCLUDES)) $(P_INCLUDES) $(SRCS) -o $(TARGET) $(sort $(LIBS))

# Run
run: build
	./eddy $(DEMO_FILE)

#Publish
publish: build
	sudo cp ./eddy /usr/local/bin
	sudo cp ./assets/images/eddy.png /usr/share/pixmaps
	cp ./assets/manifests/eddy.desktop ~/.local/share/applications
	chmod +x ~/.local/share/applications/eddy.desktop