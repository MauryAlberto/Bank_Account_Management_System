add_executable(bank_client main.cpp)
target_link_libraries(bank_client core ${BANK_ACCOUNT_SYSTEM_LIBS})
