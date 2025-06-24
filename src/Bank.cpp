#include "Bank.hpp"

template <typename T>
T getValidatedInput(const std::string& prompt){
    std::string input;
    T value;

    while(true){
        std::cout << prompt;
        std::getline(std::cin, input);

        std::stringstream ss(input);
        ss >> value;

        if(!ss.fail() && ss.eof()){
            break;
        }else{
            std::cerr << "Invalid input. Please enter a value of type " << typeid(T).name() << ".\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }

    return value;
}

template <>
std::string getValidatedInput<std::string>(const std::string& prompt){
    std::string input;

    while(true){
        std::cout << prompt;
        std::getline(std::cin, input);

        if(!input.empty()){
            break;
        }else{
            std::cerr << "Invalid input. Please enter a non-empty string.\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }

    return input;
}

bool containsNumber(const std::string& str) {
    for (char ch : str) {
        if (std::isdigit(ch)) {
            return true;
        }
    }
    return false;
}

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
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    accNum = getValidatedInput<int>("Enter Account Number: ");

    // call accountExists() here to check if account already exists
    if(accountExists(accNum)){
        std::cerr << "Account #" << accNum << " already exists.\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    name = getValidatedInput<std::string>("Enter Holder Name: ");
    while(containsNumber(name) || name.size() > 50){
        std::cerr << "Error: Invalid holder name.\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        name = getValidatedInput<std::string>("Enter Holder Name: ");
    }

    balance = getValidatedInput<double>("Enter Initial Balance: ");
    while(balance <= 0.0){
        std::cerr << "Error: Initial balance must be greater than 0.\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        balance = getValidatedInput<double>("Enter Initial Balance: ");
    }

    if(typeChoice == 1){
        double rate;
        /* std::cout << "Enter Interest Rate (e.g., 0.02 for 2%): ";
        std::cin >> rate; */
        rate = getValidatedInput<double>("Enter Interest Rate (e.g., 0.02 for 2% | max interest rate is 0.055): ");
        while(rate < 0 || rate > 0.055){
            std::cerr << "Error: Invalid interest rate.\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
            rate = getValidatedInput<double>("Enter Interest Rate (e.g., 0.02 for 2% | max interest rate is 0.055): ");
        }
        accounts.push_back(std::make_unique<SavingsAccount>(accNum, name, balance, rate));
    }else if(typeChoice == 2){
        int overDraft;
        overDraft = getValidatedInput<int>("Enter Overdraft Limit ($5000 max limit): ");
        while(overDraft < 0 || overDraft > 5000){
            std::cerr << "Error: Invalid overdraft limit.\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
            overDraft = getValidatedInput<int>("Enter Overdraft Limit ($5000 max limit): ");
        }
        accounts.push_back(std::make_unique<CheckingAccount>(accNum, name, balance, overDraft));
    }

    std::cout << (typeChoice == 1 ? "Savings" : "Checking") << " account created successfully.\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    accounts.back()->saveToFile();
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void Bank::deposit(){
    int accNum;
    double amount;

    accNum = getValidatedInput<int>("Enter Account Number: ");

    Account* acc = findAccount(accNum);
    if(acc != nullptr){
        amount = getValidatedInput<double>("Enter Deposit Amount: ");
        while(amount < 0.0){
            std::cerr << "Error: Cannot deposit negative amount.\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
            amount = getValidatedInput<double>("Enter Deposit Amount: ");
        }
        acc->deposit(amount);
        std::cout << "New Balance: $" << acc->getBalance() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void Bank::withdraw(){
    int accNum;
    double amount;

    accNum = getValidatedInput<int>("Enter Account Number: ");

    Account* acc = findAccount(accNum);
    if(acc != nullptr){
        amount = getValidatedInput<double>("Enter Withdraw Amount: ");
        while(amount < 0.0){
            std::cerr << "Error: Cannot withdraw negative amount.\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
            amount = getValidatedInput<double>("Enter Withdraw Amount: ");
        }
        acc->withdraw(amount);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void Bank::displayAccount(){
    int accNum;

    accNum = getValidatedInput<int>("Enter Account Number: ");

    Account* acc = findAccount(accNum);
    if(acc != nullptr){
        acc->display();
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void Bank::displayAllAccounts(){
    for(auto& acc : accounts){
        acc->display();
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void Bank::closeAccount(){
    int accNum;
    accNum = getValidatedInput<int>("Enter account number to close: ");

    auto it = std::find_if(accounts.begin(), accounts.end(), 
                [accNum](const std::unique_ptr<Account>& acc){
                    return acc->getAccountNumber() == accNum;
                });
    
    if(it != accounts.end()){
        std::cout << "Closing account #" << accNum << "...\n";
        accounts.erase(it); // Unique pointer is deleted here
        std::cout << "Account closed successfully.\n";
        saveAllAccounts(); // update the persistent file
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }else{
        std::cerr << "Account #" << accNum << " not found.\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void Bank::modifyAccount(){
    int accNum;
    accNum = getValidatedInput<int>("Enter account number to modify: ");

    Account* acc = findAccount(accNum);
    if(acc == nullptr){
        std::cerr << "Account not found.\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    std::cout << "Account found:\n";
    acc->display();
    std::this_thread::sleep_for(std::chrono::seconds(2));

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
            newName = getValidatedInput<std::string>("Enter New Holder Name: ");
            acc->setHolderName(newName);
            break;
        }
        case 2:{
            double newBalance;
            newBalance= getValidatedInput<double>("Enter New Balance:");
            while(newBalance <= 0.0){
                std::cerr << "Error: New balance must be greater than 0.\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
                newBalance = getValidatedInput<double>("Enter New Balance: ");
            }
            acc->setBalance(newBalance);
            break;
        }
        case 3:{
            auto* savings = dynamic_cast<SavingsAccount*>(acc);
            if(savings){
                double newRate;
                newRate = getValidatedInput<double>("Enter Interest Rate (e.g., 0.02 for 2% | max interest rate is 0.055): ");
                while(newRate < 0){
                    std::cerr << "Error: Invalid interest rate.\n";
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    newRate = getValidatedInput<double>("Enter Interest Rate (e.g., 0.02 for 2% | max interest rate is 0.055): ");
                }
                savings->setInterestRate(newRate);
                break;
            }else{
                std::cerr << "This is not a savings account.\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
                return;
            }
            
        }
        case 4:{
            auto* checking = dynamic_cast<CheckingAccount*>(acc);
            if(checking){
                int newLimit;
                newLimit= getValidatedInput<int>("Enter new overdraft limit ($5000 max limit): ");
                while(newLimit < 0 || newLimit > 5000){
                    std::cerr << "Error: Invalid overdraft limit.\n";
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    newLimit = getValidatedInput<int>("Enter new overdraft limit ($5000 max limit): ");
                }
                checking->setOverDraftLimit(newLimit);
                break;
            }else{
                std::cerr << "This is not a checking account.\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
                return;
            }
        }
        default:{
            std::cerr << "Invalid choice.\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return;
        }
    }

    acc->saveToFile();
    std::cout << "Account updated successfully.\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void Bank::saveAllAccounts(){
    std::ofstream File("accounts_temp.dat");

    if(!File.is_open()){
        std::cerr << "Unable to open accounts_temp.dat\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
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
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void Bank::loadAllAccounts(){
    std::ifstream File("accounts.dat");

    if(!File.is_open()){
        std::cerr << "Unable to open accounts.dat\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
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
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

Account* Bank::findAccount(int accNum){
    for(auto& acc : accounts){
        if(acc->getAccountNumber() == accNum){
            return acc.get();
        }
    }

    std::cerr << "Acount #" << accNum << " does not exist.\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return nullptr;
}

void Bank::applyInterestChoice(){
    std::string input;
    std::cout << "Apply interest to ONE account or ALL accounts? (Enter ONE or ALL): ";
    std::getline(std::cin, input);

    if(input == "ONE"){
        int accNum;
        std::cout << "Enter Account Number: ";
        std::cin >> accNum;
        Account* acc = findAccount(accNum);
        if(acc == nullptr){
            std::cerr << "Account not found.\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return;
        }

        auto* savings = dynamic_cast<SavingsAccount*>(acc);
        if(savings){
            savings->applyInterest();
        }else{
            std::cerr << "This is not a savings account.\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return;
        }
    }else if(input == "ALL"){
        for(const auto& acc : accounts){
            auto* savings = dynamic_cast<SavingsAccount*>(acc.get());
            if(savings){
                savings->applyInterest();
            }
        }
    }else{
        std::cerr << "Invalid choice.\n";
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
}