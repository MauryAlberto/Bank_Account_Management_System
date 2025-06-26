#include "CheckingAccount.hpp"

void CheckingAccount::deposit(double amount) {
    balance += amount;
}

void CheckingAccount::withdraw(double amount) {
    if(balance + overdraftLimit - amount >= 0){
        balance -= amount;
        std::cout << "New Balance: $" << balance << std::endl;
    }else{
        std::cerr << "Overdraft limit exceeded.\n";
        return;
    }
}

void CheckingAccount::display() const {
    std::cout << "CHECKING " << accountNumber << " " << holderName << " " << balance << " " << overdraftLimit << "\n";
}

void CheckingAccount::setOverDraftLimit(int newLimit) {
    overdraftLimit = newLimit;
}

std::string CheckingAccount::getHolderName() const {
    return holderName;
}

std::string CheckingAccount::getAccountType() const {
    return "CHECKING";
}

int CheckingAccount::getOverDraftLimit() const{
    return overdraftLimit;
}

std::string CheckingAccount::serialize() const {
    std::stringstream ss;
    std::string safeName = holderName;
    std::replace(safeName.begin(), safeName.end(), ' ', '_');
    ss << "CHECKING " << accountNumber << " " << safeName << " " << balance << " " << overdraftLimit;
    return ss.str();
}