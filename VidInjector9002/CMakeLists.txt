cmake_minimum_required(VERSION 3.3)

# Project
project(VidInjector9002)
find_package(xtd REQUIRED)
add_sources(
  src/form1.h
  src/form1.cpp
)
target_type(GUI_APPLICATION)

# Install
install_component()
