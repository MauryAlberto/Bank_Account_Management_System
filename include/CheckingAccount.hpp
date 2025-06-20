#pragma once

#include "Account.hpp"

class CheckingAccount : public Account{
private:
    double overdraftLimit;
public:
    CheckingAccount(int accNum, const std::string& name, double balance, double limit) : Account(accNum, name, balance), overdraftLimit(limit) {};
    
    void deposit(double amount) override;
    void withdraw(double amount) override;
    void display() const override;
    void saveToFile(std::ofstream& out) const override;
    void loadFromFile(std::ifstream& in) override;
};
