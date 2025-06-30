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

    json createAccountFromJson(const json& accJson);
    json deposit(const json& accJson);
    json withdraw(const json& accJson);
    json displayAccount(const json& accJson);
    json closeAccount(const json& accJson);
    json modifyAccount(const json& accJson);
    json applyInterestChoice(const json& accJson);

    json saveAllAccounts() const;
    json displayAllAccounts() const;
    json exportAllAccountsToFile() const;  
    json loadAllAccounts();
    json deleteAllAccounts();

    bool accountExists(int accNum) const;
    Account* findAccount(int accNum) const;
private:
    std::vector<std::unique_ptr<Account>> accounts;

    Bank() = default; // private constructor
    Bank(const Bank&) = delete; // delete copy constructor
    Bank& operator=(const Bank&) = delete; // delete copy assignment operator
};