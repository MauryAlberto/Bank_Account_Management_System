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

    void createAccountFromJson(const json& accJson);
    void deposit(const json& accJson);
    void withdraw(const json& accJson);
    void displayAccount(const json& accJson);
    void displayAllAccounts();
    void closeAccount(const json& accJson);
    void modifyAccount(const json& accJson);

    void saveAllAccounts();
    void loadAllAccounts();
    void exportAllAccountsToFile();

    void applyInterestChoice(const json& accJson);

    bool accountExists(int accNum);
    Account* findAccount(int accNum);
private:
    std::vector<std::unique_ptr<Account>> accounts;

    Bank() = default; // private constructor
    Bank(const Bank&) = delete; // delete copy constructor
    Bank& operator=(const Bank&) = delete; // delete copy assignment operator
};