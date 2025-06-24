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
    int accNum;
    std::cout << "Enter account number to close: ";
    std::cin >> accNum;

    auto it = std::find_if(accounts.begin(), accounts.end(), 
                [accNum](const std::unique_ptr<Account>& acc){
                    return acc->getAccountNumber() == accNum;
                });
    
    if(it != accounts.end()){
        std::cout << "Closing account $" << accNum << "...\n";
        accounts.erase(it); // Unique pointer is deleted here
        saveAllAccounts(); // update the persistent file
        std::cout << "Account close successfully.\n";
    }else{
        std::cerr << "Account #" << accNum << " not found.\n";
    }
}

void Bank::modifyAccount(){
    int accNum;
    std::cout << "Enter account number to modify: ";
    std::cin >> accNum;

    Account* acc = findAccount(accNum);
    if(acc == nullptr){
        std::cerr << "Account not found.\n";
    }

    std::cout << "Account found:\n";
    acc->display();

    std::cout << "\nChoose what to modify:\n";
    std::cout << "1. Holder Name\n";
    std::cout << "2. Balance\n";
    std::cout << "3. Interest Rate (Savings Only)\n";
    std::cout << "4. Overdraft Limit (Checking Only)\n";
    std::cout << "Enter choice: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore();

    switch(choice){
        case 1:{
            std::string newName;
            std::cout << "Enter new holder name: ";
            std::getline(std::cin, newName);
            acc->setHolderName(newName);
            break;
        }
        case 2:{
            double newBalance;
            std::cout << "Enter a new balance: ";
            std::cin >> newBalance;
            acc->setBalance(newBalance);
            break;
        }
        case 3:{
            auto* savings = dynamic_cast<SavingsAccount*>(acc);
            if(savings){
                double newRate;
                std::cout << "Enter new interest rate: ";
                std::cin >> newRate;
                savings->setInterestRate(newRate);
            }else{
                std::cerr << "This is not a savings account.\n";
            }
            break;
        }
        case 4:{
            auto* checking = dynamic_cast<CheckingAccount*>(acc);
            if(checking){
                int newLimit;
                std::cout << "Enter new overdraft limit: ";
                std::cin >> newLimit;
                checking->setOverDraftLimit(newLimit);
            }else{
                std::cerr << "This is not a checking account.\n";
            }
        }
        default:{
            std::cerr << "Invalid.\n";
            return;
        }
    }

    acc->saveToFile();
    std::cout << "Account updated successfully.\n";
}

void Bank::saveAllAccounts(){
    std::ofstream File("accounts_temp.dat");

    if(!File.is_open()){
        std::cerr << "Unable to open accounts_temp.dat\n";
        return;
    }

    for(const auto& acc : accounts){
        std::string name = acc->getHolderName();
        std::replace(name.begin(), name.end(), ' ', '_');

        File << acc->getAccountType() << " " << acc->getAccountNumber()
             << " " << name << " " << acc->getBalance() << " ";
        
        if(acc->getAccountType() == "SAVINGS"){
            auto* savings = dynamic_cast<SavingsAccount*>(acc.get());
            if(savings){
                File << savings->getInterestRate();
            }
        }else if(acc->getAccountType() == "CHECKING"){
            auto* checking = dynamic_cast<CheckingAccount*>(acc.get());
            if(checking){
                File << checking->getOverDraftLimit();
            }
        }

        File << "\n";
    }

    File.close();
    std::remove("accounts.dat");
    std::rename("accounts_temp.dat", "accounts.dat");

    std::cout << "All accounts saved succesfully.\n";
}

void Bank::loadAllAccounts(){
    std::ifstream File("accounts.dat");

    if(!File.is_open()){
        std::cerr << "Unable to open accounts.dat\n";
        return;
    }

    accounts.clear();
    std::string line;
    while(std::getline(File, line)){
        std::stringstream ss(line);
        std::string name, accountType;
        int accNum;
        double balance;

        ss >> accountType >> accNum >> name >> balance;
        std::replace(name.begin(), name.end(), '_', ' ');

        if(accountType == "SAVINGS"){
            double rate;
            ss >> rate;
            accounts.push_back(std::make_unique<SavingsAccount>(accNum, name, balance, rate));
        }else if(accountType == "CHECKING"){
            int overDraft;
            ss >> overDraft;
            accounts.push_back(std::make_unique<CheckingAccount>(accNum, name, balance, overDraft));
        }else{
            std::cerr << "Unknown account type: " << accountType << "\n";
            return;
        }
    }

    File.close();
    std::cout << "All files loaded succesffully.\n";
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