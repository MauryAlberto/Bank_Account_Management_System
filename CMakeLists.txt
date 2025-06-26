cmake_minimum_required(VERSION 3.10)
project(BankSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find redis++ and hiredis
find_package(PkgConfig REQUIRED)
pkg_check_modules(HIREDIS REQUIRED hiredis)
pkg_check_modules(REDISPP REQUIRED redis++)

# Include directories
include_directories(
    include
    ${HIREDIS_INCLUDE_DIRS}
    ${REDISPP_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    src/Account.cpp
    src/Bank.cpp
    src/CheckingAccount.cpp
    src/RedisCache.cpp
    src/SavingsAccount.cpp
    src/main.cpp
)

# Define the executable
add_executable(bank_system ${SOURCES})

# Link libraries
target_link_libraries(bank_system
    ${HIREDIS_LIBRARIES}
    ${REDISPP_LIBRARIES}
)
