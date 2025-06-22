#include "Bank.hpp"

bool Bank::accountExists(int accNum){
    for(auto& acc : accounts){
        if(acc->getAccountNumber() == accNum){
            return true;
        }
    }
    return false;
}

void Bank::createAccount(){
    int typeChoice, accNum;
    std::string name;
    double balance;
    std::cout << "1. Savings Account\n2. Checking Account\nSelect type: ";
    std::cin >> typeChoice;
    std::cin.ignore();

    if(typeChoice != 1 && typeChoice != 2){
        std::cerr << "Invalid account type.\n";
        return;
    }

    std::cout << "Enter Account Number: ";
    std::cin >> accNum;
    std::cin.ignore();

    // call accountExists() here to check if account already exists
    if(accountExists(accNum)){
        std::cerr << "Account #" << accNum << " already exists.\n";
        return;
    }

    std::cout << "Enter Holder Name: ";
    std::getline(std::cin, name);
    
    std::cout << "Enter Initial Balance: ";
    std::cin >> balance;

    if(typeChoice == 1){
        double rate;
        std::cout << "Enter Interest Rate (e.g., 0.02 for 2%): ";
        std::cin >> rate;
        accounts.push_back(std::make_unique<SavingsAccount>(accNum, name, balance, rate));
    }else if(typeChoice == 2){
        int overDraft;
        std::cout << "Enter Overdraft limit: ";
        std::cin >> overDraft;
        accounts.push_back(std::make_unique<CheckingAccount>(accNum, name, balance, overDraft));
    }

    std::cout << (typeChoice == 1 ? "Savings" : "Checking") << " account created successfully.\n";
}

void Bank::deposit(){
    int accNum;
    double amount;

    std::cout << "Enter Account Number: ";
    std::cin >> accNum;

    Account* acc = findAccount(accNum);
    if(acc != nullptr){
        std::cout << "Enter Deposit Amount: ";
        std::cin >> amount;

        acc->deposit(amount);
        std::cout << "New Balance: $" << acc->getBalance() << std::endl;
    }
}

void Bank::withdraw(){
    int accNum;
    double amount;

    std::cout << "Enter Account Number: ";
    std::cin >> accNum;

    Account* acc = findAccount(accNum);
    if(acc != nullptr){
        std::cout << "Enter Withdraw Amount: ";
        std::cin >> amount;

        acc->withdraw(amount);
        std::cout << "New Balance: $" << acc->getBalance() << std::endl;
    }
}

void Bank::displayAccount(){
    int accNum;

    std::cout << "Enter Account Number: ";
    std::cin >> accNum;

    Account* acc = findAccount(accNum);
    if(acc != nullptr){
        acc->display();
    }
}

void Bank::displayAllAccounts(){
    for(auto& acc : accounts){
        acc->display();
    }
}

void Bank::closeAccount(){

}

void Bank::modifyAccount(){

}

void Bank::saveAllAccounts(){

}

void Bank::loadAllAccounts(){

}

Account* Bank::findAccount(int accNum){
    for(auto& acc : accounts){
        if(acc->getAccountNumber() == accNum){
            return acc.get();
        }
    }

    std::cerr << "Acount #" << accNum << " does not exist.\n";
    return nullptr;
}