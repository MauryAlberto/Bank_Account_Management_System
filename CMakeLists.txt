cmake_minimum_required(VERSION 3.10)
project(BankingSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Find hiredis
pkg_check_modules(HIREDIS REQUIRED hiredis)

# Find redis++
find_library(REDIS_PLUS_PLUS_LIB redis++)
if(NOT REDIS_PLUS_PLUS_LIB)
    message(FATAL_ERROR "redis++ library not found")
endif()

# Add subdirectories
add_subdirectory(src)
add_subdirectory(server)
add_subdirectory(client)