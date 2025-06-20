#pragma once

#include "Account.hpp"

class SavingsAccount : public Account{
private:
    double interestRate;

public:
    SavingsAccount(int accNum, const std::string& name, double balance, double rate) : Account(accNum, name, balance), interestRate(rate) {};

    void applyInterest();
    void display() const override;

};
