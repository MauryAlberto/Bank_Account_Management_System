#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <mutex>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Account{
public:
    Account(int accNum, const std::string& name, double initialBalance);
    virtual ~Account();
    
    virtual json withdraw(double amount) = 0;
    virtual json display() const = 0;
    virtual json toJson() const = 0;
    virtual std::string getHolderName() const = 0;
    virtual std::string getAccountType() const = 0;

    json deposit(double amount);
    int getAccountNumber() const;
    double getBalance() const;
    void setHolderName(const std::string& name);
    void setBalance(double newBalance);
protected:
    std::mutex mtx;
    int accountNumber;
    std::string holderName;
    double balance;
};