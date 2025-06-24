#pragma once

#include "Account.hpp"

class CheckingAccount : public Account{
private:
    int overdraftLimit;
public:
    CheckingAccount(int accNum, const std::string& name, double balance, int limit) : Account(accNum, name, balance), overdraftLimit(limit) {};
    
    void deposit(double amount) override;
    void withdraw(double amount) override;
    void display() const override;
    void saveToFile() const override;
    void loadFromFile() override;
    std::string getHolderName() const override;
    std::string getAccountType() const override;

    void setOverDraftLimit(int newLimit);
    int getOverDraftLimit();
};
