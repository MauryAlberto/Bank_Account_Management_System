#pragma once

#include "Account.hpp"

class SavingsAccount : public Account{
public:
    SavingsAccount(int accNum, const std::string& name, double balance, double rate) : Account(accNum, name, balance), interestRate(rate) {};

    json withdraw(double amount) override;
    json display() const override;
    json toJson() const override;
    std::string getHolderName() const override;
    std::string getAccountType() const override;

    json applyInterest();
    void setInterestRate(double newRate);
    double getInterestRate() const;
private:
    double interestRate;
};
