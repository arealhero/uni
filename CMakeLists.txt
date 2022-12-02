cmake_minimum_required(VERSION 3.20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/Meta/CMake")

project(Uni)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INSTALL_PREFIX Dest)
set(CMAKE_INSTALL_INCLUDEDIR include)
set(CMAKE_INSTALL_LIBDIR lib)

include (utils)

include_directories (Libraries)

add_subdirectory (Libraries)
add_subdirectory (Applications)

add_subdirectory (Vendors)
add_subdirectory (Tests)
