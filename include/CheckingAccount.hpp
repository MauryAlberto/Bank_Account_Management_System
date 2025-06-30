#pragma once

#include "Account.hpp"

class CheckingAccount : public Account{
public:
    CheckingAccount(int accNum, const std::string& name, double balance, int limit) : Account(accNum, name, balance), overdraftLimit(limit) {};
    
    json withdraw(double amount) override;
    json display() const override;
    json toJson() const override;
    std::string getHolderName() const override;
    std::string getAccountType() const override;

    void setOverDraftLimit(int newLimit);
    int getOverDraftLimit() const;
private:
    int overdraftLimit;
};
