# Compiler & Flags
CC        = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g -O0 -std=gnu99
LDFLAGS   =

# Directories
SRCDIR    = src
BUILDDIR  = build
BINDIR    = bin

# Target
TARGET    = $(BINDIR)/noxe

# Source files (exclude cmd.c — removed in favour of Ctrl+S/Q)
SRCS      = $(filter-out $(SRCDIR)/cmd.c, $(wildcard $(SRCDIR)/*.c)) libmemory/arena.c
OBJS      = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(filter-out $(SRCDIR)/cmd.c, $(wildcard $(SRCDIR)/*.c))) \
            $(BUILDDIR)/libmemory/arena.o
DEPS      = $(OBJS:.o=.d) $(BUILDDIR)/test_undo.d

# Test target
TEST_SRCS = test_undo.c
TEST_OBJS = $(BUILDDIR)/test_undo.o $(filter-out $(BUILDDIR)/main.o, $(OBJS))
TEST_TARGET = $(BINDIR)/test_undo

# Phony targets
.PHONY: all clean run format dirs test

# Default target
all: dirs $(TARGET)

# Create directories
dirs:
	@mkdir -p $(BUILDDIR) $(BINDIR)

# Link executable
$(TARGET): $(OBJS)
	@echo "Linking $@..."
	@$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

# Build test
$(TEST_TARGET): $(TEST_OBJS)
	@echo "Linking test..."
	@$(CC) $(CFLAGS) $(TEST_OBJS) -o $@ $(LDFLAGS)
	@echo "Test build complete: $@"

# Compile test object
$(BUILDDIR)/test_undo.o: test_undo.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Compile objects with dependency tracking
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILDDIR)/libmemory/%.o: libmemory/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Include auto-generated dependencies
-include $(DEPS)

# Run the program
run: all
	@echo "Running $(TARGET)..."
	@$(TARGET)

# Clean build artifacts
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILDDIR) $(BINDIR)
	@echo "Clean complete!"

# Format source code with clang-format
format:
	@echo "Formatting source files..."
	@clang-format -i $(SRCDIR)/*.c $(SRCDIR)/*.h
	@echo "Format complete!"

# Check formatting without changing files
format-check:
	@echo "Checking format..."
	@clang-format --dry-run --Werror $(SRCDIR)/*.c $(SRCDIR)/*.h

# Static analysis with cppcheck (if installed)
check:
	@echo "Running cppcheck..."
	@cppcheck --enable=all --suppress=missingIncludeSystem $(SRCDIR)/

# Show help
help:
	@echo "Available targets:"
	@echo "  all          : Build the project (default)"
	@echo "  run          : Build and run"
	@echo "  test         : Build and run undo/redo test"
	@echo "  clean        : Remove build artifacts"
	@echo "  format       : Format source with clang-format"
	@echo "  format-check : Check formatting without changes"
	@echo "  check        : Static analysis with cppcheck"
	@echo "  help         : Show this help"

# Run test
test: $(TEST_TARGET)
	@echo "Running test..."
	@$(TEST_TARGET)
