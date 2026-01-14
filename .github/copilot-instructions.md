# Copilot Instructions for AI Agents

## Project Overview
This is a C project organized for modular development and maintainability. The main source code is in `src/`, with headers in `src/include/`. The project is built using both a `Makefile` and VS Code C/C++ build tasks. The structure supports both command-line and IDE-based workflows.

## Key Directories & Files
- `src/main.c`: Entry point for the application.
- `src/exercise/exercise.c` & `src/include/exercise/exercise.h`: Core module implementation and interface.
- `Makefile`: Standard build automation; supports custom targets.
- `.vscode/launch.json`: VS Code debugging configuration.
- `build/`: Output directory for build artifacts and intermediate files.
- `exercise.xcodeproj/`: Xcode project files (for macOS development).

## Build & Run Workflows
- **VS Code**: Use the default build task `C/C++: clang 生成活动文件` to compile the active file. Output is placed in the same directory as the source.
- **Makefile**: Run `make` from the project root for full builds. Custom targets may exist for cleaning or testing.
- **Xcode**: Open `exercise.xcodeproj` for IDE-based development on macOS.

## Project Conventions
- Source files are grouped by module under `src/exercise/` and headers under `src/include/exercise/`.
- All new modules should follow this pattern: `src/exercise/<module>.c` and `src/include/exercise/<module>.h`.
- Debugging is configured via `.vscode/launch.json` and Xcode workspace settings.
- Build artifacts and logs are stored under `build/`.

## Integration Points
- No external dependencies detected; project is self-contained C code.
- For macOS, Xcode integration is supported via project files in `exercise.xcodeproj/`.

## Examples
- To add a new module, create `src/exercise/newmodule.c` and `src/include/exercise/newmodule.h`, then update the `Makefile` and include the header in `main.c`.
- To debug, use VS Code's launch configuration or Xcode's debugger.

## Tips for AI Agents
- Always update both source and header files when adding new functionality.
- Respect the directory structure for maintainability.
- Use the provided build systems (`Makefile`, VS Code tasks, or Xcode) for compilation and debugging.
- Reference `main.c` for application entry and integration points.

## Language Preference
- 默认用中文回答，与本项目相关的所有说明、代码注释与交互请优先使用中文。
- 如确需英文（例如需要与英文 API 文档一致），请在同一回复中同时提供中文说明。

---
If any section is unclear or missing, please provide feedback for further refinement.