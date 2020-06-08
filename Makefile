TARGET		:= exo
SUFFIX		:=
PLATFORM	:= macOS

# Compiler flags. Optmised for debugging, not speed
CPPFLAGS	:= -O3 -std=c++17 -Wno-unknown-pragmas
LDFLAGS		:= -O3 -lSDL2_gfx -lSDL2

ifeq ($(PLATFORM), win32)
    CXX         := $(WIN32_HOME)/usr/bin/i686-w64-mingw32.shared-g++
    SUFFIX      := .exe
    LDFLAGS     := -static-libgcc -static-libstdc++ -lgcc_eh -Dmain=SDL_main -lmingw32 -lSDL2main -lSDL2_gfx -lSDL2 -mconsole
endif

# Required directories for building
SOURCE_DIR	:= ./src
BUILD_DIR	:= ./build
TESTS_DIR	:= ./tests
PRODUCT_DIR	:= $(BUILD_DIR)/product
OBJECTS_DIR	:= $(BUILD_DIR)/intermediate
INSTALL_DIR	:= $(HOME)/bin

# Simple project, we only scan one directory (no subdirectory)
SOURCES		:= $(wildcard $(SOURCE_DIR)/*.cpp) $(wildcard $(SOURCE_DIR)/*/*.cpp)
OBJECTS		:= $(patsubst $(SOURCE_DIR)/%.cpp, $(OBJECTS_DIR)/%.$(PLATFORM).o, $(SOURCES))
PALS		:= $(wildcard $(TESTS_DIR)/*/*.txt) $(wildcard $(TESTS_DIR)/*/*/*.txt)

# Check if we should compile colors with that
ifeq ($(COLORFUL), 1)
CPPFLAGS 	+= -DERROR_COLORFUL
endif

.PHONY: clean
all: $(TARGET)

tests: $(PALS)

install: $(TARGET)
	@echo "[installing product $(TARGET)]"
	@cp $(PRODUCT_DIR)/$(TARGET) $(INSTALL_DIR)/$(TARGET)

%.json: systems/%.json $(TARGET)
	@echo "[running $(TARGET)]"
	@$(PRODUCT_DIR)/$(TARGET) -s 1000 -w 600 -h 480 systems/$@

# Build the PAL compiler
$(TARGET): $(PRODUCT_DIR) $(OBJECTS_DIR) $(OBJECTS)
	@echo "[linking product $(TARGET)]"
	@$(CXX) $(OBJECTS) $(LDFLAGS) -o $(PRODUCT_DIR)/$(TARGET)$(SUFFIX)

# Build a single object file from its .cpp counterpart
$(OBJECTS_DIR)/%.$(PLATFORM).o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "[compiling $(notdir $<)]"
	@$(CXX) $(CPPFLAGS) -c $< -o $@

# Create the build products directory
$(PRODUCT_DIR):
	@mkdir -p $(PRODUCT_DIR)

# Create the intermediate objects directory
$(OBJECTS_DIR):
	@mkdir -p $(OBJECTS_DIR)

# Delete build products and intermediate objects
clean:
	@rm -rf $(BUILD_DIR)
