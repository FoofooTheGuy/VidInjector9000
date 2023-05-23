cmake_minimum_required(VERSION 3.3)

# Project
project(VidInjector9002)
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
