# exercise（C 练习工程）

这是一个按模块组织的 C 工程：源码在 `src/`，头文件在 `src/include/`，支持 `CMake`（推荐）与 `Makefile`（仅参考）。

## 目录结构

- `src/main.c`：程序入口
- `src/exercise/`：核心模块实现
- `src/include/`：工程头文件（按模块分目录）
- `resources/`：运行时资源（构建后会拷贝到 `build/resources`）
- `third_party/zlog/`：vendor 的 zlog（三方依赖，静态库链接）
- `.vscode/`：VS Code 工作区配置（尽量只放跨环境通用项）

## 依赖

- CMake ≥ 3.16（推荐走 CMake 流程）
- C 编译器：macOS 上建议 `clang`（Xcode Command Line Tools），Linux/Windows 亦可使用对应工具链
- `make`

## 编译与运行（推荐：CMake）

> 注意：本项目通过静态库链接 vendor 的 zlog。首次构建前需要先把 zlog 的 `libzlog.a` 编出来，否则 CMake 会报错。

### 1) 构建 third_party/zlog

```bash
make -C third_party/zlog/src
```

成功后应生成：`third_party/zlog/src/libzlog.a`。

### 2) 配置与编译

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```

- 可执行文件：`build/exercise`
- 资源目录：`build/resources`（由 CMake 在构建后自动拷贝）
- 编译数据库：`build/compile_commands.json`（用于 VS Code/clangd/C/C++ IntelliSense）

### 3) 运行

```bash
./build/exercise
```

## 编译与运行（可选：Makefile，仅参考）

> 仓库内的 `Makefile` 头部已注明：该文件仅用于参考学习，不保证长期正确。建议优先使用 CMake。

```bash
make
./build/bin/exercise
```

或：

```bash
make run
```

## VS Code 使用说明

### 推荐扩展

- C/C++：`ms-vscode.cpptools`
- CMake Tools：`ms-vscode.cmake-tools`

### 工作区（仓库）已包含的设置

仓库的 `.vscode/settings.json` 只保留跨环境通用项，例如：忽略 `build/`、C99、`compile_commands.json` 路径、includePath、格式化与文件关联。

### 你需要在 User Settings（用户级）加入的内容

由于不同机器/不同 OS 的编译器路径与终端 profile 差异很大，这类配置建议放到 User Settings（或 VS Code Profile）里，而不要提交到仓库。

在 **User Settings (JSON)** 中加入（按需调整路径/架构）：

```jsonc
{
  // 终端默认 profile：按 OS 拆分，避免写死某个平台
  "terminal.integrated.defaultProfile.osx": "zsh",
  "terminal.integrated.defaultProfile.windows": "PowerShell",
  "terminal.integrated.defaultProfile.linux": "bash",

  // C/C++：建议在用户级配置 compilerPath / intelliSenseMode
  // macOS 常见：/usr/bin/clang（Xcode CLT）或 /opt/homebrew/opt/llvm/bin/clang
  "C_Cpp.default.compilerPath": "/usr/bin/clang",

  // 根据你的机器架构选择：macos-clang-arm64（Apple Silicon）/ macos-clang-x64（Intel）
  "C_Cpp.default.intelliSenseMode": "macos-clang-arm64"
}
```

### 常见问题

- 如果 VS Code 提示找不到 `build/compile_commands.json`：先跑一次 CMake 配置/构建：

  ```bash
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
  cmake --build build -j
  ```

- 如果 CMake 报 `libzlog.a not found`：先执行：

  ```bash
  make -C third_party/zlog/src
  ```
