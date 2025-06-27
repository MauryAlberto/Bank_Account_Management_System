
# Create the core library
add_library(core STATIC
    Bank.cpp
    Account.cpp
    CheckingAccount.cpp
    SavingsAccount.cpp
    RedisCache.cpp
    Network.cpp
)

# Include directories for the library
target_include_directories(core PUBLIC ../include)
target_include_directories(core PRIVATE ${HIREDIS_INCLUDE_DIRS})

# Link libraries to core
target_link_libraries(core ${REDIS_PLUS_PLUS_LIB})
target_link_libraries(core ${HIREDIS_LIBRARIES})

# Set compile flags
target_compile_options(core PRIVATE ${HIREDIS_CFLAGS_OTHER})