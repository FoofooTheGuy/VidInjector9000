cmake_minimum_required(VERSION 3.15)

# Project
project(VidInjector9002)
option(ENABLE_STATIC_BUILD "" ON)

if (ENABLE_STATIC_BUILD)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(STATUS "Enabling static build")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE INTERNAL "" FORCE)
    endif()
endif()

find_package(xtd REQUIRED)

file(GLOB VidInjector9002_SRC
	"src/*.cpp"
    "src/*.hpp"
    "src/*.h"
)

file(GLOB nnc_SRC
	"src/nnc/exheader_syscall_tab.inc"
	"src/nnc/*.c"
    "src/nnc/*.h"
    "src/nnc/nnc/*.h"
    "src/nnc/mbedtls/*.c"
    "src/nnc/mbedtls/mbedtls/*.h"
)

add_sources(
  ${VidInjector9002_SRC}
  ${nnc_SRC}
)
target_type(GUI_APPLICATION)

# Install
install_component()
