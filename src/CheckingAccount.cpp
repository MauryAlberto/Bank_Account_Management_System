#include "CheckingAccount.hpp"

void CheckingAccount::deposit(double amount) {
    balance += amount;
    saveToFile();
}

void CheckingAccount::withdraw(double amount) {
    if(balance + overdraftLimit - amount >= 0){
        balance -= amount;
        saveToFile();
    }else{
        throw std::runtime_error("Overdraft limit exceeded.");
    }
}

void CheckingAccount::display() const {
    std::cout << "CHECKING " << accountNumber << " " << holderName << " " << balance << " " << overdraftLimit << "\n";
}

void CheckingAccount::saveToFile() const {
    std::ifstream File("../accounts.dat");
    std::ofstream tempFile("../accounts_temp.dat", std::ios::trunc);

    if(!File.is_open()){
        std::cerr << "Unable to open accounts.dat\n";
        return;
    }else if(!tempFile.is_open()){
        std::cerr << "Unable to open account_temp.dat\n";
    }

    std::string line;
    std::string safeName = holderName;
    std::stringstream updatedLine;
    bool updated = false;

    std::replace(safeName.begin(), safeName.end(), ' ', '_');
    updatedLine << "CHECKING " << accountNumber << " " << safeName << " " << balance << " " << overdraftLimit;

    while(std::getline(File, line)){
        std::stringstream ss(line);
        std::string accountType;
        int accNum;

        ss >> accountType >> accNum;

        if(accountType == "CHECKING" && accNum == accountNumber){
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

    std::cout << "Account data for checking account #" << accountNumber << " saved successfully.\n";
}

void CheckingAccount::loadFromFile() {
    std::ifstream File("../accounts.dat");

    if(!File.is_open()){
        std::cerr << "Unable to open accounts.dat\n";
        return;
    }

    std::string line;
    while(std::getline(File, line)){
        std::stringstream ss(line);
        std::string accountType, name;
        int accNum, overDraft;
        double bal;

        ss >> accountType >> accNum >> name >> bal >> overDraft;

        if(accountType == "CHECKING" && accNum == accountNumber){
            std::replace(name.begin(), name.end(), '_', ' ');

            holderName = name;
            balance = bal;
            overdraftLimit = overDraft;

            std::cout << "Account data for checking account #" << accountNumber << " loaded successfully.\n";
            return;
        }
    }

    std::cerr << "Checking Account #" << accountNumber << " does not exist.\n";
    return;
}
