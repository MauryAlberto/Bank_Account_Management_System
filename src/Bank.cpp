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
        }
    }

    return input;
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

    /* std::cout << "Enter Account Number: ";
    std::cin >> accNum;
    std::cin.ignore(); */
    accNum = getValidatedInput<int>("Enter Account Number: ");

    // call accountExists() here to check if account already exists
    if(accountExists(accNum)){
        std::cerr << "Account #" << accNum << " already exists.\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    /* std::cout << "Enter Holder Name: ";
    std::getline(std::cin, name); */
    name = getValidatedInput<std::string>("Enter Holder Name: ");
    
    /* std::cout << "Enter Initial Balance: ";
    std::cin >> balance; */
    balance = getValidatedInput<double>("Enter Initial Balance: ");
    if(balance <= 0){
        std::cerr << "Must enter initial balance greater than 0.\n";
        return;
    }

    if(typeChoice == 1){
        double rate;
        /* std::cout << "Enter Interest Rate (e.g., 0.02 for 2%): ";
        std::cin >> rate; */
        rate = getValidatedInput<double>("Enter Interest Rate (e.g., 0.02 for 2%): ");
        accounts.push_back(std::make_unique<SavingsAccount>(accNum, name, balance, rate));
    }else if(typeChoice == 2){
        int overDraft;
        /* std::cout << "Enter Overdraft limit: ";
        std::cin >> overDraft; */
        overDraft = getValidatedInput<int>("Enter Overdraft Limit: ");
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

        acc->withdraw(amount);
        std::cout << "New Balance: $" << acc->getBalance() << std::endl;
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
        std::cout << "Closing account $" << accNum << "...\n";
        accounts.erase(it); // Unique pointer is deleted here
        saveAllAccounts(); // update the persistent file
        std::cout << "Account closed successfully.\n";
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
    std::this_thread::sleep_for(std::chrono::seconds(2));
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
            newName = getValidatedInput<std::string>("Enter new holder name: ");
            acc->setHolderName(newName);
            break;
        }
        case 2:{
            double newBalance;
            newBalance= getValidatedInput<double>("Enter a new balance:");
            acc->setBalance(newBalance);
            break;
        }
        case 3:{
            auto* savings = dynamic_cast<SavingsAccount*>(acc);
            if(savings){
                double newRate;
                newRate = getValidatedInput<double>("Enter a new interest rate:");
                savings->setInterestRate(newRate);
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
                newLimit= getValidatedInput<int>("Enter new overdraft limit: ");
                checking->setOverDraftLimit(newLimit);
            }else{
                std::cerr << "This is not a checking account.\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }
        default:{
            std::cerr << "Invalid.\n";
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