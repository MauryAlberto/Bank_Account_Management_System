#pragma once

#include "Account.hpp"

class CheckingAccount : public Account{
public:
    CheckingAccount(int accNum, const std::string& name, double balance, int limit) : Account(accNum, name, balance), overdraftLimit(limit) {};
    
    void deposit(double amount) override;
    void withdraw(double amount) override;
    void display() const override;
    std::string serialize() const override;
    std::string getHolderName() const override;
    std::string getAccountType() const override;

    void setOverDraftLimit(int newLimit);
    int getOverDraftLimit() const;
private:
    int overdraftLimit;
};
