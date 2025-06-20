#pragma once

#include "Account.hpp"

class SavingsAccount : public Account{
private:
    double interestRate;

public:
    SavingsAccount(int accNum, const std::string& name, double balance, double rate) : Account(accNum, name, balance), interestRate(rate) {};

    virtual void deposit(double amount) override;
    virtual void withdraw(double amount) override;
    virtual void display() const override;
    virtual void saveToFile(std::ofstream& out) const override;
    virtual void loadFromFile(std::ifstream& in) override;

    void applyInterest();
};
