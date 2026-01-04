# =============================================================================
# 项目配置
# =============================================================================
PROJECT_NAME = exercise
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2
CPPFLAGS = -I$(SRCDIR)/include
LDFLAGS = 
LIBS = -lm

# =============================================================================
# 目录结构
# =============================================================================
SRCDIR = src
INCLUDEDIR = $(SRCDIR)/include
RESOURCESDIR = resources
BUILDDIR = build
BINDIR = $(BUILDDIR)/bin
OBJDIR = $(BUILDDIR)/obj
DEPDIR = $(BUILDDIR)/dep

# =============================================================================
# 文件集合
# =============================================================================
# 查找所有源文件
SOURCES = $(shell find $(SRCDIR) -name '*.c' ! -name 'main.c')
MAIN_SOURCE = $(SRCDIR)/main.c

# 生成对应的对象文件路径
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
MAIN_OBJECT = $(OBJDIR)/main.o

# 依赖文件
DEPS = $(OBJECTS:$(OBJDIR)/%.o=$(DEPDIR)/%.d) $(MAIN_OBJECT:$(OBJDIR)/%.o=$(DEPDIR)/%.d)

# 可执行文件
TARGET = $(BINDIR)/$(PROJECT_NAME)

# =============================================================================
# 默认目标
# =============================================================================
.PHONY: all clean rebuild install help

all: $(TARGET)

# =============================================================================
# 编译规则
# =============================================================================

# 链接可执行文件
$(TARGET): $(OBJECTS) $(MAIN_OBJECT) | $(BINDIR)
	@echo "Linking executable..."
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	@echo "Build completed: $(TARGET)"

# 编译主文件
$(MAIN_OBJECT): $(MAIN_SOURCE) | $(OBJDIR)
	@echo "Compiling $<..."
	@mkdir -p $(dir $(DEPDIR)/main.d)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MP -MF $(DEPDIR)/main.d -c $< -o $@

# 编译源文件
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@echo "Compiling $<..."
	@mkdir -p $(dir $(DEPDIR)/$*.d)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MP -MF $(DEPDIR)/$*.d -c $< -o $@

# =============================================================================
# 目录创建
# =============================================================================
$(BINDIR) $(OBJDIR):
	@mkdir -p $@

# =============================================================================
# 资源文件处理
# =============================================================================
.PHONY: resources
resources:
	@echo "Copying resources..."
	@mkdir -p $(BINDIR)/../resources
	@cp -r $(RESOURCESDIR)/* $(BINDIR)/../resources/ 2>/dev/null || :

# =============================================================================
# 清理
# =============================================================================
clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILDDIR)
	@echo "Clean completed."

# =============================================================================
# 重新构建
# =============================================================================
rebuild: clean all

# =============================================================================
# 安装（可选）
# =============================================================================
PREFIX ?= /usr/local
INSTALL_BIN = $(PREFIX)/bin

install: $(TARGET)
	@echo "Installing $(PROJECT_NAME) to $(INSTALL_BIN)..."
	@install -d $(INSTALL_BIN)
	@install -m 755 $(TARGET) $(INSTALL_BIN)/$(PROJECT_NAME)
	@echo "Installation completed."

# =============================================================================
# 运行程序
# =============================================================================
.PHONY: run
run: $(TARGET) resources
	@echo "Running $(PROJECT_NAME)..."
	@$(TARGET)

# =============================================================================
# 包含依赖文件
# =============================================================================
-include $(DEPS)

# =============================================================================
# 帮助信息
# =============================================================================
help:
	@echo "Available targets:"
	@echo "  all      - Build the project (default)"
	@echo "  clean    - Remove all build files"
	@echo "  rebuild  - Clean and rebuild everything"
	@echo "  run      - Build and run the program"
	@echo "  install  - Install the program to system"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  CC       - C compiler (default: gcc)"
	@echo "  CFLAGS   - Compiler flags (default: -Wall -Wextra -std=c99 -g -O2)"
	@echo "  PREFIX   - Installation prefix (default: /usr/local)"

# =============================================================================
# 调试目标
# =============================================================================
.PHONY: debug-info
debug-info:
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "Main Object: $(MAIN_OBJECT)"
	@echo "Dependencies: $(DEPS)"
	@echo "Target: $(TARGET)"