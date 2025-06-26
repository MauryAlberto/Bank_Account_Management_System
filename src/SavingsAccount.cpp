#include "SavingsAccount.hpp"

void SavingsAccount::deposit(double amount) {
    balance += amount;
}

void SavingsAccount::withdraw(double amount) {
    if(balance - amount >= 0){
        balance -= amount;
        std::cout << "New Balance: $" << balance << std::endl;
    }else{
        std::cerr << "Insufficient funds in savings account.\n";
        return;
    }
}

void SavingsAccount::display() const {
    std::cout << "SAVINGS " << accountNumber << " " << holderName << " " << balance << " " << interestRate << "\n";
}

void SavingsAccount::applyInterest() {
    double interest = balance * interestRate;
    balance += interest;

    std::cout << "Interest of $" << interest << " applied to account #" << accountNumber << ".\n";
    std::cout << "New balance: $" << balance << "\n";
}

void SavingsAccount::setInterestRate(double newRate) {
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

std::string SavingsAccount::serialize() const {
    std::stringstream ss;
    std::string safeName = holderName;
    std::replace(safeName.begin(), safeName.end(), ' ', '_');
    ss << "SAVINGS " << accountNumber << " " << safeName << " " << balance << " " << interestRate;
    return ss.str();
}