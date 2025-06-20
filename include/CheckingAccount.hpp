#pragma once

#include "Account.hpp"

class CheckingAccount : public Account{
private:
    double overdraftLimit;
public:
    CheckingAccount(int accNum, const std::string& name, double balance, double limit) : Account(accNum, name, balance), overdraftLimit(limit) {};
    
    void withdraw(double amount) override;
    void display() const override;
};
