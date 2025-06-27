# Create the bank_server executable
add_executable(bank_server BankServer.cpp)

# Link against the core library
target_link_libraries(bank_server core)

# Link against redis++ and hiredis
target_link_libraries(bank_server ${REDIS_PLUS_PLUS_LIB})
target_link_libraries(bank_server ${HIREDIS_LIBRARIES})

# Include directories
target_include_directories(bank_server PRIVATE ${HIREDIS_INCLUDE_DIRS})

# Set compile flags
target_compile_options(bank_server PRIVATE ${HIREDIS_CFLAGS_OTHER})