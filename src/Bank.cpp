#include "Bank.hpp"

template<typename T>
std::string getTypeName();

template<>
std::string getTypeName<int>() { return "integer"; }

template<>
std::string getTypeName<double>() { return "double"; }

template<>
std::string getTypeName<std::string>() { return "string"; }

template <typename T>
bool validateJsonField(const json& obj, const std::string& key, T& out){
    if(!obj.contains(key)){
        std::cerr << "Missing field '" << key << "'.\n";
        return false;
    }

    if constexpr(std::is_same_v<T, int>){
        if(!obj[key].is_number_integer()){
            std::cerr << "Invalid type for key: " << key << ". Expected " << getTypeName<T>() << ".\n";
            return false;
        }
    }else if constexpr(std::is_same_v<T, double>){
        if(!obj[key].is_number()){
            std::cerr << "Invalid type for key: " << key << ". Expected " << getTypeName<T>() << ".\n";
            return false;
        }
    }else if constexpr(std::is_same_v<T, std::string>){
        if(!obj[key].is_string()){
            std::cerr << "Invalid type for key: " << key << ". Expected " << getTypeName<T>() << ".\n";
            return false;
        }
    }else{
        std::cerr << "Unsupported type check for key '" << key << "'.\n";
        return false;
    }

    out = obj[key].get<T>();
    return true;
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
    for(const auto& acc : accounts){
        if(acc->getAccountNumber() == accNum){
            return true;
        }
    }
    return false;
}

void Bank::deposit(const json& accJson){
    int accNum;
    double amount;

    if(!validateJsonField(accJson, "accountNumber", accNum)){
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    Account* acc = findAccount(accNum);
    if(acc != nullptr){
        if(amount < 0.0){
            std::cerr << "Error: Cannot deposit negative amount.\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return;
        }
        acc->deposit(amount);
        RedisCache::getInstance().saveAccount(*acc);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void Bank::withdraw(const json& accJson){
    int accNum;
    double amount;

    if(!validateJsonField(accJson, "accountNumber", accNum)){
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    Account* acc = findAccount(accNum);
    if(acc != nullptr){
        if(amount < 0.0){
            std::cerr << "Error: Cannot withdraw negative amount.\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return;
        }
        acc->withdraw(amount);
        RedisCache::getInstance().saveAccount(*acc);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void Bank::displayAccount(const json& accJson){
    int accNum;
    if(!validateJsonField(accJson, "accountNumber", accNum)){
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

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

void Bank::closeAccount(const json& accJson){
    if(!accJson.contains("accountNumber") || !accJson["accountNumber"].is_number_integer()){
        std::cerr << "Invalid or missing 'accountNumber'. Expected integer.\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    int accNum = accJson["accountNumber"];

    auto it = std::find_if(accounts.begin(), accounts.end(), 
                [accNum](const std::unique_ptr<Account>& acc){
                    return acc->getAccountNumber() == accNum;
                });
    
    if(it != accounts.end()){
        std::cout << "Closing account #" << accNum << "...\n";
        accounts.erase(it); // unique pointer is deleted here
        RedisCache::getInstance().deleteAccount(accNum); // delete account from Redis
        std::cout << "Account closed successfully.\n";
        saveAllAccounts(); // update Redis
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }else{
        std::cerr << "Account #" << accNum << " not found.\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void Bank::modifyAccount(const json& accJson){
    if(!accJson.contains("accountNumber") || !accJson["accountNumber"].is_number_integer()){
        std::cerr << "Invalid or missing 'accountNumber'. Expected integer.\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    int accNum = accJson["accountNumber"];
    Account* acc = findAccount(accNum);
    
    if(acc == nullptr){
        std::cerr << "Account #" << accNum << " not found.\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    std::cout << "Account #" << accNum << " found.\n";
    acc->display();
    std::this_thread::sleep_for(std::chrono::seconds(2));

    if(accJson.contains("holderName") && accJson.is_string()){
        acc->setHolderName(accJson["holderName"]);
    }else if(accJson.contains("holderName")){
        std::cerr << "Invalid type for 'holderName'. Expected string.\n";
        return;
    }

    if(accJson.contains("balance") && accJson["balance"].is_number()){
        acc->setBalance(accJson["balance"]);
    }else if(accJson.contains("balance")){
        std::cerr << "Invalid type for 'balance'. Expected number.\n";
        return;
    }

    if(auto* savings = dynamic_cast<SavingsAccount*>(acc)){
        if(accJson.contains("interestRate")){
            if(accJson["interestRate"].is_number()){
                savings->setInterestRate(accJson["interestRate"]);
            }else{
                std::cerr << "Invalid type for 'interestRate'. Expected number.\n";
                return;
            }
        }
    }else if(auto* checking = dynamic_cast<CheckingAccount*>(acc)){
        if(accJson.contains("overdraftLimit")){
            if(accJson["overdraftLimit"].is_number_integer()){
                checking->setOverDraftLimit(accJson["overdraftLimit"]);
            }else{
                std::cerr << "Invalid type for 'overdraftLimit'. Expected integer.\n";
                return;
            }
        }
    }else{
        std::cerr << "Error: Unknown account type.\n";
        return;
    }

    RedisCache::getInstance().saveAccount(*acc);
    std::cout << "Account #" << acc->getAccountNumber() << " updated successfully.\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void Bank::saveAllAccounts(){
    for(const auto& acc : accounts){
        RedisCache::getInstance().saveAccount(*acc);
    }

    std::cout << accounts.size() << " account(s) saved to Redis successfully.\n";
}

void Bank::loadAllAccounts(){
    accounts.clear();
    
    std::vector<std::string> keys = RedisCache::getInstance().getAllAccountKeys();

    for(const auto& key : keys){
        int accNum = std::stoi(key.substr(key.find(":") + 1));
        std::unique_ptr<Account> acc = RedisCache::getInstance().loadAccount(accNum);
        if(acc){
            accounts.push_back(std::move(acc));
        }
    }

    std::cout << accounts.size() << " account(s) loaded from Redis sucessfully.\n";
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

void Bank::applyInterestChoice(const json& accJson){
    std::string target;
    if(!validateJsonField(accJson, "target", target)){
        std::cerr << "Invalid or missing 'target'. Expected string.\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    if(target == "ONE"){
        int accNum;
        if(!validateJsonField(accJson, "accountNumber", accNum)){
            std::cerr << "Invalid or missing 'accountNumber' for ONE target.\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return;
        }

        Account* acc = findAccount(accNum);
        if(acc == nullptr){
            std::cerr << "Account #" << accNum << " not found.\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return;
        }

        auto* savings = dynamic_cast<SavingsAccount*>(acc);
        if(savings){
            savings->applyInterest();
            RedisCache::getInstance().saveAccount(*acc);
        }else{
            std::cerr << "This is not a savings account.\n";
        }
    }else if(target == "ALL"){
        int count = 0;
        for(const auto& acc : accounts){
            auto* savings = dynamic_cast<SavingsAccount*>(acc.get());
            if(savings){
                savings->applyInterest();
                RedisCache::getInstance().saveAccount(*acc);
                ++count;
            }
        }

        std::cout << "Interest applied to " << count << " savings account(s).\n";
    }else{
        std::cerr << "Invalid target: Must be 'ONE' or 'ALL'.\n";
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void Bank::exportAllAccountsToFile(){
    std::ofstream file("accounts_export.json", std::ios::trunc);
    
    if(!file.is_open()){
        std::cerr << "Failed to open JSON export file.\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    json allAccounts = json::array();
    for(const auto& acc : accounts){
        allAccounts.push_back(acc->toJson());
    }

    file << allAccounts.dump(4);
    std::cout << accounts.size() << " account(s) exported successfully.\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void Bank::createAccountFromJson(const json& acc){
    std::string name, accountType;
    int accNum;
    double balance;

    if(!validateJsonField(acc, "accountNumber", accNum) ||
       !validateJsonField(acc, "balance", balance) ||
       !validateJsonField(acc, "holderName", name) ||
       !validateJsonField(acc, "accountType", accountType)){
       std::this_thread::sleep_for(std::chrono::seconds(2));
       return;
    }

    if(accountExists(accNum)){
        std::cerr << "Account #" << accNum << " already exists.\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    std::unique_ptr<Account> newAcc;

    if(accountType == "SAVINGS"){
        double rate;
        if(!validateJsonField(acc, "interestRate", rate)){
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return;
        }
        newAcc = std::make_unique<SavingsAccount>(accNum, name, balance, rate);
    }else if(accountType == "CHECKING"){
        int overdraft;
        if(!validateJsonField(acc, "overdraftLimit", overdraft)){
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return;
        }
        newAcc = std::make_unique<CheckingAccount>(accNum, name, balance, overdraft);
    }else{
        std::cerr << "Unkown account type. Account creation failed.\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    accounts.push_back(std::move(newAcc));
    RedisCache::getInstance().saveAccount(*accounts.back());

    std::cout << "Account #" << accNum << " created successfully.\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}
