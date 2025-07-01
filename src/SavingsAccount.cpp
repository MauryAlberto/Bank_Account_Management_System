#include "SavingsAccount.hpp"

json SavingsAccount::withdraw(double amount) {
    std::lock_guard<std::mutex> lock(mtx);
    std::stringstream ss;
    json msg;
    if(balance - amount >= 0){
        balance -= amount;
        ss << "New balance of $" << balance << "\n";
        msg = {
            {"status", "success: "},
            {"message", ss.str()}
        };
        return msg;
    }else{
        ss << "Insufficient funds in savings account.\n";
        msg = {
            {"status", "success: "},
            {"message", ss.str()}
        };
        return msg;
    }
}

json SavingsAccount::display() const {
    std::stringstream ss;
    json msg;
    ss << "SAVINGS " << accountNumber << " " << holderName << " " << balance << " " << interestRate;
    msg = {
        {"status", "success: "},
        {"message", ss.str()}
    };
    return msg;
}

json SavingsAccount::applyInterest() {
    std::lock_guard<std::mutex> lock(mtx);
    std::stringstream ss;
    json msg;
    double interest = balance * interestRate;
    balance += interest;
    ss << "New balance of $" << balance << "\n";
    msg = {
        {"status", "success: "},
        {"message", ss.str()}
    };
    return msg;
}

void SavingsAccount::setInterestRate(double newRate) {
    std::lock_guard<std::mutex> lock(mtx);
    interestRate = newRate;
}

std::string SavingsAccount::getHolderName() const {
    return holderName;
}

std::string SavingsAccount::getAccountType() const {
    return "SAVINGS";
}

double SavingsAccount::getInterestRate() const {
    return interestRate;
}

json SavingsAccount::toJson() const {
    return {
        {"accountType", "SAVINGS"},
        {"accountNumber", accountNumber},
        {"holderName", holderName},
        {"balance", balance},
        {"interestRate", interestRate}
    };
}