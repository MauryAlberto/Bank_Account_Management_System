#pragma once

#include "Account.hpp"

class SavingsAccount : public Account{
private:
    double interestRate;

public:
    SavingsAccount(int accNum, const std::string& name, double balance, double rate) : Account(accNum, name, balance), interestRate(rate) {};

    void deposit(double amount) override;
    void withdraw(double amount) override;
    void display() const override;
    void saveToFile() const override;
    void loadFromFile() override;

    void applyInterest();
};
