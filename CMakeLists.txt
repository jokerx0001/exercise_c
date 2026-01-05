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

# 运行时资源拷贝(与Makefile的resource目标类似)
add_custom_command(TARGET exercise POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/resources" "${CMAKE_BINARY_DIR}/resources"
    COMMENT "Copying resources..."
)

# 安装
install(TARGETS exercise RUNTIME DESTINATION bin)
