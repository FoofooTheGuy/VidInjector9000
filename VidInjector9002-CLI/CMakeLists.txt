cmake_minimum_required(VERSION 3.15)
project(VidInjector9002-CLI)

set(ENABLE_TESTING CACHE BOOL OFF FORCE)
set(ENABLE_PROGRAMS CACHE BOOL OFF FORCE)
set(INSTALL_MBEDTLS_HEADERS CACHE BOOL OFF FORCE)

add_subdirectory(nnc EXCLUDE_FROM_ALL)

link_libraries(MbedTLS::mbedcrypto nnc)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
endif()

file(GLOB MY_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${MY_SRC})

if (BUILD_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
    enable_testing()
    add_subdirectory(test)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Application")
