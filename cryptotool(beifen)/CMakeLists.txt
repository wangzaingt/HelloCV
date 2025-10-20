cmake_minimum_required(VERSION 3.10)
project(CryptoTool)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件目录
include_directories(include)

# 添加可执行文件
file(GLOB SOURCES "src/*.cpp" "main.cpp")
add_executable(CryptoTool ${SOURCES})

# 在Windows上复制可执行文件到项目根目录
if(WIN32)
    add_custom_command(TARGET CryptoTool POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:CryptoTool> ${CMAKE_SOURCE_DIR}/CryptoTool.exe
        COMMENT "Copying CryptoTool to project directory"
    )
endif()
