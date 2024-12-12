cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(nrf51822_elink42)

# 將 src 目錄下的所有 .c 檔案添加到構建中
file(GLOB SRC_FILES "src/*.c")

target_sources(app PRIVATE ${SRC_FILES})