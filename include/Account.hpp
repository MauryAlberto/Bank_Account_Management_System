#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

class Account{
protected:
    int accountNumber;
    std::string holderName;
    double balance;

public:
    Account(int accNum, const std::string& name, double initialBalance);
    virtual ~Account();

    virtual void deposit(double amount) = 0;
    virtual void withdraw(double amount) = 0;
    virtual void display() const = 0;
    virtual void saveToFile() const = 0;
    virtual void loadFromFile() = 0;

    int getAccountNumber() const;
    double getBalance() const;
    void setHolderName(const std::string& name);
};