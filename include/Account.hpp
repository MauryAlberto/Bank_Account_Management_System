#pragma once

#include <iostream>
#include <fstream>
#include <string>

class Account{
protected:
    int accountNumber;
    std::string holderName;
    double balance;

public:
    Account(int accNum, const std::string& name, double initialBalance);
    virtual ~Account() = default;

    virtual void deposit(double amount);
    virtual void withdraw(double amount);
    virtual void display() const;
    virtual void saveToFile(std::ofstream& out) const;
    virtual void loadFromFile(std::ifstream& in);

    int getAccountNumber() const;
    double getBalance() const;
    void setHolderName(const std::string& name);
};