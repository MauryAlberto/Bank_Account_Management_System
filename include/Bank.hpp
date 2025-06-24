#pragma once

#include "SavingsAccount.hpp"
#include "CheckingAccount.hpp"
#include <algorithm>
#include <vector>
#include <memory>
#include <chrono>

class Bank{
private:
    std::vector<std::unique_ptr<Account>> accounts;
    std::string dataFile = "accounts.dat";
public:
    void createAccount();
    void deposit();
    void withdraw();
    void displayAccount();
    void displayAllAccounts();
    void closeAccount();
    void modifyAccount();

    void saveAllAccounts();
    void loadAllAccounts();

    bool accountExists(int accNum);
    Account* findAccount(int accNum);
};