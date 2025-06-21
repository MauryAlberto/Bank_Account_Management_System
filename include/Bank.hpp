#pragma once

#include "SavingsAccount.hpp"
#include "CheckingAccount.hpp"
#include <vector>
#include <memory>

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

    Account* findAccount(int accNum);
};