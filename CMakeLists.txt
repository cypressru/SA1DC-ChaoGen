cmake_minimum_required(VERSION 3.13)

set(KOS_BASE /opt/toolchains/dc/kos)
set(KOS_CC_BASE /opt/toolchains/dc/sh-elf)
set(KOS_SUBARCH pristine)
set(KOS_PORTS ${KOS_BASE}/../kos-ports)

include(${KOS_BASE}/utils/cmake/dreamcast.toolchain.cmake)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(untitled C)

set(CMAKE_C_STANDARD 23)

add_executable(untitled main.c)
target_link_libraries(untitled raylib GL png parallax mp3 m)

kos_add_romdisk(untitled romdisk)
