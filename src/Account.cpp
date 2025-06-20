#include "Account.hpp"

Account::Account(int accNum, const std::string& name, double initialBalance){
    this->accountNumber = accNum;
    this->holderName = name;
    this->balance = initialBalance;
}

Account::~Account() = default;

int Account::getAccountNumber() const{
    return accountNumber;
}

double Account::getBalance() const{
    return balance;
}

void Account::setHolderName(const std::string& name){
    this->holderName = name;
}
