#pragma once

#include "RedisCache.hpp"
#include "SavingsAccount.hpp"
#include "CheckingAccount.hpp"
#include <algorithm>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <limits>
#include <typeinfo>

class Bank{
public:
    static Bank& getInstance(){ // static accessor
        static Bank instance;
        return instance;
    }

    void createAccount();
    void deposit();
    void withdraw();
    void displayAccount();
    void displayAllAccounts();
    void closeAccount();
    void modifyAccount();

    void saveAllAccounts();
    void loadAllAccounts();
    void exportAllAccountsToFile();

    void applyInterestChoice();

    bool accountExists(int accNum);
    Account* findAccount(int accNum);
private:
    std::vector<std::unique_ptr<Account>> accounts;

    Bank() = default; // private constructor
    Bank(const Bank&) = delete; // delete copy constructor
    Bank& operator=(const Bank&) = delete; // delete copy assignment operator
};