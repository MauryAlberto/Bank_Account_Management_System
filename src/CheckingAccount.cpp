#include "CheckingAccount.hpp"

void CheckingAccount::deposit(double amount) {
    std::lock_guard<std::mutex> lock(mtx);
    balance += amount;
    std::cout << "New Balance: $" << balance << std::endl;
}

void CheckingAccount::withdraw(double amount) {
    std::lock_guard<std::mutex> lock(mtx);
    if(balance + overdraftLimit - amount >= 0){
        balance -= amount;
        std::cout << "New Balance: $" << balance << std::endl;
    }else{
        std::cerr << "Overdraft limit exceeded.\n";
    }
}

void CheckingAccount::display() const {
    std::cout << "CHECKING " << accountNumber << " " << holderName << " " << balance << " " << overdraftLimit << "\n";
}

void CheckingAccount::setOverDraftLimit(int newLimit) {
    std::lock_guard<std::mutex> lock(mtx);
    overdraftLimit = newLimit;
}

std::string CheckingAccount::getHolderName() const {
    return holderName;
}

std::string CheckingAccount::getAccountType() const {
    return "CHECKING";
}

int CheckingAccount::getOverDraftLimit() const {
    return overdraftLimit;
}

json CheckingAccount::toJson() const {
    return {
        {"accountType", "CHECKING"},
        {"accountNumber", accountNumber},
        {"holderName", holderName},
        {"balance", balance},
        {"overdraftLimit", overdraftLimit}
    };
}