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

void Account::setBalance(double newBalance){
    balance = newBalance;
}

json Account::deposit(double amount){
    std::lock_guard<std::mutex> lock(mtx);
    std::stringstream ss;
    json msg;
    balance += amount;
    ss << "New balance of $" << balance;
    msg = {
        {"status", "success: "},
        {"message", ss.str()}
    };
    return msg;
}
