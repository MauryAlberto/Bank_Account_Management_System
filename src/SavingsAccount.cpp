#include "SavingsAccount.hpp"

void SavingsAccount::deposit(double amount) {
    balance += amount;
    saveToFile();
}

void SavingsAccount::withdraw(double amount) {
    if(balance - amount >= 0){
        balance -= amount;
        saveToFile();
    }else{
        throw std::runtime_error("Insufficient funds in savings account.");
    }
}

void SavingsAccount::display() const {
    std::cout << "SAVINGS " << accountNumber << " " << holderName << " " << balance << " " << interestRate << "\n";
}

void SavingsAccount::saveToFile() const {
    std::ifstream File("../accounts.dat");
    std::ofstream tempFile("../accounts_temp.dat", std::ios::trunc);

    if(!File.is_open()){
        std::cerr << "Unable to open accounts.dat\n";
        return;
    }else if(!tempFile.is_open()){
        std::cerr << "Unable to open accounts_temp.dat\n";
        return;
    }

    std::string line;
    std::string safeName = holderName;
    std::stringstream updatedLine;
    bool updated = false;

    std::replace(safeName.begin(), safeName.end(), ' ', '_');
    updatedLine << "SAVINGS " << accountNumber << " " << safeName << " " << balance << " " << interestRate;

    while(std::getline(File, line)){
        std::stringstream ss(line);
        std::string accountType;
        int accNum;
        
        ss >> accountType >> accNum;

        if(accountType == "SAVINGS" && accNum == this->accountNumber){
            tempFile << updatedLine.str() << "\n";
            updated = true;
        }else{
            tempFile << line << "\n";
        }
    }

    if(!updated){
        tempFile << updatedLine.str() << "\n";
    }

    File.close();
    tempFile.close();

    std::remove("../accounts.dat");
    std::rename("../accounts_temp.dat", "../accounts.dat");

    std::cout << "Account data for account #" << accountNumber << " saved successfully.\n";
}

void SavingsAccount::loadFromFile() {
    std::ifstream File("../accounts.dat");

    if(!File.is_open()){
        std::cerr << "Unable to open accounts.dat\n";
        return;
    }

    std::string line;
    while(std::getline(File, line)){
        std::stringstream ss(line);
        std::string accountType, name;
        int accNum;
        double bal, rate;

        ss >> accountType >> accNum >> name >> bal >> rate;

        if(accountType == "SAVINGS" && accNum == this->accountNumber){
            std::replace(name.begin(), name.end(), '_', ' ');

            this->holderName = name;
            this->balance = bal;
            this->interestRate = rate;
            
            std::cout << "Account data for account #" << accountNumber << " loaded successfully.\n";
            return;
        }
    }

    std::cerr << "Account #" << accountNumber << " does not exist.\n";
    return;
}

void SavingsAccount::applyInterest(){
    double interest = balance * interestRate;
    balance += interest;

    std::cout << "Interest of $" << interest << " applied to account #" << accountNumber << ".\n";
    std::cout << "New balance: $" << balance << "\n";

    saveToFile();
}