# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++2a -Wall -Wextra -pedantic

# Directories
OBJDIR = $(PROJ_DIR)/obj
BINDIR = $(PROJ_DIR)/bin

# Target executable
TARGET = $(BINDIR)/$(TARGET_NAME)

# Source files
SOURCES = $(wildcard $(PROJ_DIR)/*.cpp)

# Object files
OBJECTS = $(patsubst $(PROJ_DIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

# Default target
all: $(TARGET)

# Compile source files into object files
$(OBJDIR)/%.o: $(PROJ_DIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link the target executable
$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Clean up generated files
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Phony targets
.PHONY: all clean
