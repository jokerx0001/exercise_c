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

# =============================================================================
# 依赖：vendor zlog（不依赖 brew，不拉远端）
# =============================================================================
# 你的 zlog.h 位于 third_party/zlog/src/zlog.h
set(ZLOG_SRC_DIR "${CMAKE_SOURCE_DIR}/third_party/zlog/src")
set(ZLOG_LIB "${ZLOG_SRC_DIR}/libzlog.a")

if(NOT EXISTS "${ZLOG_SRC_DIR}/zlog.h")
    message(FATAL_ERROR "zlog.h not found: ${ZLOG_SRC_DIR}/zlog.h")
endif()

if(NOT EXISTS "${ZLOG_LIB}")
    message(FATAL_ERROR "libzlog.a not found: ${ZLOG_LIB}. Please build vendor zlog first (e.g. make -C third_party/zlog/src).")
endif()

add_library(zlog STATIC IMPORTED GLOBAL)
set_target_properties(zlog PROPERTIES
    IMPORTED_LOCATION "${ZLOG_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${ZLOG_SRC_DIR}"
)

# 谁需要包含 zlog.h，就链接 zlog（头文件目录会自动传播）
target_link_libraries(exercise_core PRIVATE zlog)
target_link_libraries(exercise PRIVATE zlog)

# 运行时资源拷贝(与Makefile的resource目标类似)
add_custom_command(TARGET exercise POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/resources" "${CMAKE_BINARY_DIR}/resources"
    COMMENT "Copying resources..."
)

# 安装
install(TARGETS exercise RUNTIME DESTINATION bin)
