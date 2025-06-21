#include "Account.hpp"

Account::Account(int accNum, const std::string& name, double initialBalance){
    accountNumber = accNum;
    holderName = name;
    balance = initialBalance;
}

Account::~Account() = default;

int Account::getAccountNumber() const{
    return accountNumber;
}

double Account::getBalance() const{
    return balance;
}

void Account::setHolderName(const std::string& name){
    holderName = name;
}
