cmake_minimum_required(VERSION 3.16)
project(exercise C)

# 设置C标准与编译数据库
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 将main.c以外的模块编成库
file(GLOB_RECURSE MODULE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.c"
)
list(REMOVE_ITEM MODULE_SOURCES "${CMAKE_SOURCE_DIR}/src/main.c")

add_library(exercise_core ${MODULE_SOURCES})
target_include_directories(exercise_core PUBLIC "${CMAKE_SOURCE_DIR}/src/include")
# 编译警告设置
target_compile_options(exercise_core PRIVATE -Wall -Wextra)

# 添加可执行文件
add_executable(exercise "${CMAKE_SOURCE_DIR}/src/main.c")
target_link_libraries(exercise PRIVATE exercise_core)

# 依赖库处理
include(ExternalProject)

# zlog
ExternalProject_Add(zlog_project
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/third_party/zlog"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND $(MAKE)
    INSTALL_COMMAND ""
    BUILD_IN_SOURCE 1
)
ExternalProject_Get_Property(zlog_project SOURCE_DIR)
add_library(zlog STATIC IMPORTED)
set_target_properties(zlog PROPERTIES
    IMPORTED_LOCATION "${SOURCE_DIR}/src/libzlog.a"
)
add_dependencies(zlog zlog_project)
# 让可执行与核心模块能找到 zlog.h（zlog 仓库有 include/ 目录；如无则改为 src/）
target_include_directories(exercise PUBLIC "${SOURCE_DIR}/include")
target_include_directories(exercise_core PUBLIC "${SOURCE_DIR}/include")
# 链接 zlog 库
target_link_libraries(exercise PRIVATE zlog)

# 运行时资源拷贝(与Makefile的resource目标类似)
add_custom_command(TARGET exercise POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/resources" "${CMAKE_BINARY_DIR}/resources"
    COMMENT "Copying resources..."
)

# 安装
install(TARGETS exercise RUNTIME DESTINATION bin)
