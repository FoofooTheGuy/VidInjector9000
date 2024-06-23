cmake_minimum_required(VERSION 3.15)

include(cmake/helpers.cmake)

project(nnc VERSION 1.0.0.0 LANGUAGES C CXX)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# TODO: provide FindMbedTLS ?
find_package(MbedTLS)

if (NOT MbedTLS_FOUND)
    set(ENABLE_TESTING CACHE BOOL OFF FORCE)
    set(ENABLE_PROGRAMS CACHE BOOL OFF FORCE)
    set(INSTALL_MBEDTLS_HEADERS CACHE BOOL OFF FORCE)

    add_subdirectory(extern/mbedtls EXCLUDE_FROM_ALL)
endif()

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.c")

file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hh")

set(PROJECT_FILES ${PROJECT_SOURCES} ${PROJECT_HEADERS})

add_library(${PROJECT_NAME} ${PROJECT_FILES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC MbedTLS::mbedcrypto)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if (BUILD_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
    enable_testing()
    add_subdirectory(test)
endif()

create_target_directory_groups(${PROJECT_NAME})
