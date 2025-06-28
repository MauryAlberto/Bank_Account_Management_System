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
#include <mutex>

class Bank{
public:
    static Bank& getInstance(){ // static accessor
        static Bank instance;
        return instance;
    }

    bool createAccountFromJson(const json& accJson);
    bool deposit(const json& accJson);
    bool withdraw(const json& accJson);
    bool displayAccount(const json& accJson);
    void displayAllAccounts();
    bool closeAccount(const json& accJson);
    bool modifyAccount(const json& accJson);

    void saveAllAccounts();
    void loadAllAccounts();
    void exportAllAccountsToFile();

    bool applyInterestChoice(const json& accJson);

    bool accountExists(int accNum);
    Account* findAccount(int accNum);
private:
    std::vector<std::unique_ptr<Account>> accounts;

    Bank() = default; // private constructor
    Bank(const Bank&) = delete; // delete copy constructor
    Bank& operator=(const Bank&) = delete; // delete copy assignment operator
};