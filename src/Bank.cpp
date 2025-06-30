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
bool validateJsonField(const json& obj, const std::string& key, json& msg, T& out){
    std::stringstream ss;

    if(!obj.contains(key)){
        ss << "Missing field '" << key << "'.\n";
        msg = {
            {"status", "failed: "},
            {"message", ss.str()}
        };
        return false;
    }

    if constexpr(std::is_same_v<T, int>){
        if(!obj[key].is_number_integer()){
            ss << "Invalid type for key: " << key << ". Expected " << getTypeName<T>() << ".\n";
            msg = {
                {"status", "failed: "},
                {"message", ss.str()}
            };
            return false;
        }
    }else if constexpr(std::is_same_v<T, double>){
        if(!obj[key].is_number()){
            ss << "Invalid type for key: " << key << ". Expected " << getTypeName<T>() << ".\n";
            msg = {
                {"status", "failed: "},
                {"message", ss.str()}
            };
            return false;
        }
    }else if constexpr(std::is_same_v<T, std::string>){
        if(!obj[key].is_string()){
            ss << "Invalid type for key: " << key << ". Expected " << getTypeName<T>() << ".\n";
            msg = {
                {"status", "failed: "},
                {"message", ss.str()}
            };
            return false;
        }
    }else{
        ss << "Unsupported type check for key '" << key << "'.\n";
        msg = {
            {"status", "failed: "},
            {"message", ss.str()}
        };
            return false;
    }

    out = obj[key].get<T>();
    return true;
}

bool Bank::accountExists(int accNum) const {
    for(const auto& acc : accounts){
        if(acc->getAccountNumber() == accNum){
            return true;
        }
    }
    return false;
}

json Bank::deposit(const json& accJson){
    json msg;
    int accNum;
    double amount;

    if(!validateJsonField(accJson, "accountNumber", msg, accNum)){
        return msg;
    }

    Account* acc = findAccount(accNum);
    if(acc == nullptr){
        std::stringstream ss;
        ss << "Account #" << accNum << " not found.\n";
        msg = {
            {"status", "failed: "},
            {"message", ss.str()}
        };
        return msg;
    }

    if(amount < 0.0){
        std::stringstream ss;
        ss << "Cannot deposit negative amount.\n";
        msg = {
            {"status", "failed: "},
            {"message", ss.str()}
        };
        return msg;
    }

    msg = acc->deposit(amount);
    RedisCache::getInstance().saveAccount(*acc);
    return msg;
}

json Bank::withdraw(const json& accJson){
    json msg;
    int accNum;
    double amount;

    if(!validateJsonField(accJson, "accountNumber", msg, accNum)){
        return msg;
    }

    Account* acc = findAccount(accNum);
    if(acc == nullptr){
        std::stringstream ss;
        ss << "Account #" << accNum << " not found.\n";
        msg = {
            {"status", "failed: "},
            {"message", ss.str()}
        };
        return msg;
    }

    if(amount < 0.0){
        std::stringstream ss;
        ss << "Cannot withdraw negative amount.\n";
        msg = {
            {"status", "failed: "},
            {"message", ss.str()}
        };
        return msg;
    }

    msg = acc->withdraw(amount);
    RedisCache::getInstance().saveAccount(*acc);
    return msg;
}

json Bank::displayAccount(const json& accJson){
    json msg;
    int accNum;
    if(!validateJsonField(accJson, "accountNumber", msg, accNum)){
        return msg;
    }

    Account* acc = findAccount(accNum);
    if(acc == nullptr){
        std::stringstream ss;
        ss << "Account #" << accNum << " not found.\n";
        msg = {
            {"status", "failed: "},
            {"message", ss.str()}
        };
        return msg;
    }
    
    msg = acc->display();
    return msg;
}

json Bank::displayAllAccounts() const {
    json allAccounts = json::array();
    for(const auto& acc : accounts){
        allAccounts.push_back(acc->display());
    }
    return allAccounts;
}

json Bank::closeAccount(const json& accJson){
    std::stringstream ss;
    json msg;
    int accNum;

    if(!validateJsonField(accJson, "accountNumber", msg, accNum)){
        return msg;
    }

    auto it = std::find_if(accounts.begin(), accounts.end(), 
                [accNum](const std::unique_ptr<Account>& acc){
                    return acc->getAccountNumber() == accNum;
                });
    
    if(it != accounts.end()){
        accounts.erase(it); // unique pointer is deleted here
        RedisCache::getInstance().deleteAccount(accNum); // delete account from Redis
        saveAllAccounts(); // update Redis
        ss << "Acount #" << accNum << " closed succesfully.\n";
        msg = {
            {"status", "success: "},
            {"message", ss.str()}
        };
        return msg;
    }else{
        ss << "Account #" << accNum << " not found.\n";
        msg = {
            {"stauts", "failed: "},
            {"message", ss.str()}
        };
        return msg;
    }
}

json Bank::modifyAccount(const json& accJson){
    std::stringstream ss;
    json msg;
    int accNum;
    if(!validateJsonField(accJson, "accountNumber", msg, accNum)){
        return msg;
    }

    Account* acc = findAccount(accNum);
    if(acc == nullptr){
        ss << "Account #" << accNum << " not found.\n";
        msg = {
            {"stautus", "failed: "},
            {"message", ss.str()}
        };
        return msg;
    }

    std::string newName;
    if(validateJsonField(accJson, "holderName", msg, newName)){
        acc->setHolderName(newName);
    }else{
        return msg;
    }

    double newBalance;
    if(validateJsonField(accJson, "balance", msg, newBalance)){
        acc->setBalance(newBalance);
    }else{
        return msg;
    }

    if(acc->getAccountType() == "SAVINGS"){
        double interestRate;
        auto* savings = dynamic_cast<SavingsAccount*>(acc);
        if(validateJsonField(accJson, "interestRate", msg, interestRate)){
            savings->setInterestRate(interestRate);
        }else{
            return msg;
        }
    }else if(acc->getAccountType() == "CHECKING"){
        int newOverdraftLimit;
        auto* checking = dynamic_cast<CheckingAccount*>(acc);
        if(validateJsonField(accJson, "overdraftLimit", msg, newOverdraftLimit)){
            checking->setOverDraftLimit(newOverdraftLimit);
        }else{
            return msg;
        }
    }else{
        ss << "Unknown account type.\n";
        msg = {
            {"status", "failed: "},
            {"message", ss.str()}
        };
        return msg;
    }

    RedisCache::getInstance().saveAccount(*acc);
   ss << "Account #" << acc->getAccountNumber() << " updated successfully.\n";
   msg = {
        {"status", "success: "},
        {"message", ss.str()}
   };
   return msg;
}

json Bank::saveAllAccounts() const {
    std::stringstream ss;
    json msg;
    for(const auto& acc : accounts){
        RedisCache::getInstance().saveAccount(*acc);
    }

    ss << accounts.size() << " account(s) saved to Redis successfully.\n";
    msg = {
        {"status", "success: "},
        {"message", ss.str()}
    };

    return msg;
}

json Bank::loadAllAccounts(){
    std::stringstream ss;
    json msg;
    accounts.clear();
    
    std::vector<std::string> keys = RedisCache::getInstance().getAllAccountKeys();

    for(const auto& key : keys){
        int accNum = std::stoi(key.substr(key.find(":") + 1));
        std::unique_ptr<Account> acc = RedisCache::getInstance().loadAccount(accNum);
        if(acc){
            accounts.push_back(std::move(acc));
        }
    }

    ss << accounts.size() << " account(s) loaded from Redis sucessfully.\n";
    msg = {
        {"status", "success: "},
        {"message", ss.str()}
    };
    return msg;
}

Account* Bank::findAccount(int accNum) const {
    for(const auto& acc : accounts){
        if(acc->getAccountNumber() == accNum){
            return acc.get();
        }
    }
    return nullptr;
}

json Bank::applyInterestChoice(const json& accJson){
    json msg;
    std::string target;
    std::stringstream ss;

    if(!validateJsonField(accJson, "target", msg, target)){
        return msg;
    }

    if(target == "ONE"){
        int accNum;
        if(!validateJsonField(accJson, "accountNumber", msg, accNum)){
            return msg;
        }

        Account* acc = findAccount(accNum);
        if(acc == nullptr){
            ss << "Account #" << accNum << " not found.\n";
            msg = {
                {"status", "failed: "},
                {"message", ss.str()}
            };
            return msg;
        }

        if(acc->getAccountType() == "SAVINGS"){
            auto* savings = dynamic_cast<SavingsAccount*>(acc);
            msg = savings->applyInterest();
            RedisCache::getInstance().saveAccount(*acc);
        }else{
            ss << "This is not a savings account.\n";
            msg = {
                {"status", "failed: "},
                {"message", ss.str()}
            };
            return msg;
        }
    }else if(target == "ALL"){
        int count = 0;
        for(const auto& acc : accounts){
            if(acc->getAccountType() == "SAVINGS"){
                auto* savings = dynamic_cast<SavingsAccount*>(acc.get());
                savings->applyInterest();
                RedisCache::getInstance().saveAccount(*acc);
                ++count;
            }
        }
        ss << "Interest applied to " << count << " savings account(s).\n";
        msg = {
            {"status", "success: "},
            {"message", ss.str()}
        };
    }else{
        ss << "Invalid target: Must be 'ONE' or 'ALL'.\n";
        msg = {
            {"status", "failed: "},
            {"message", ss.str()}
        };
        return msg;
    }

    return msg;
}

json Bank::exportAllAccountsToFile() const {
    std::stringstream ss;
    json msg;
    
    std::ofstream file("accounts_export.json", std::ios::trunc);
    
    if(!file.is_open()){
        ss << "Failed to open JSON export file.\n";
        msg = {
            {"status", "failed: "},
            {"message", ss.str()}
        };
        return msg;
    }

    json allAccounts = json::array();
    for(const auto& acc : accounts){
        allAccounts.push_back(acc->toJson());
    }

    file << allAccounts.dump(4);
    ss << accounts.size() << " account(s) exported successfully.\n";
    msg = {
        {"status", "success: "},
        {"message", ss.str()}
    };
    return msg;
}

json Bank::createAccountFromJson(const json& acc){
    std::string name, accountType;
    int accNum;
    double balance;
    std::stringstream ss;
    json msg;

    if(!validateJsonField(acc, "accountNumber", msg, accNum) ||
       !validateJsonField(acc, "balance", msg, balance) ||
       !validateJsonField(acc, "holderName", msg, name) ||
       !validateJsonField(acc, "accountType", msg, accountType)){
       return msg;
    }

    if(accountExists(accNum)){
        ss << "Account #" << accNum << " already exists.\n";
        msg = {
            {"status", "failed:" },
            {"message", ss.str()}
        };
        return msg;
    }

    std::unique_ptr<Account> newAcc;

    if(accountType == "SAVINGS"){
        double rate;
        if(!validateJsonField(acc, "interestRate", msg, rate)){
            return msg;
        }
        newAcc = std::make_unique<SavingsAccount>(accNum, name, balance, rate);
    }else if(accountType == "CHECKING"){
        int overdraft;
        if(!validateJsonField(acc, "overdraftLimit", msg, overdraft)){
            return msg;
        }
        newAcc = std::make_unique<CheckingAccount>(accNum, name, balance, overdraft);
    }else{
        ss << "Unkown account type. Account creation failed.\n";
        msg = {
            {"status", "failed: "},
            {"message", ss.str()}
        };
        return msg;
    }

    accounts.push_back(std::move(newAcc));
    RedisCache::getInstance().saveAccount(*accounts.back());

    ss << "Account #" << accNum << " created successfully.\n";
    msg = {
        {"status", "success: "},
        {"message", ss.str()}
    };
    return msg;
}

json Bank::deleteAllAccounts(){
    std::stringstream ss;
    json msg;
    size_t accTotal= accounts.size();
    
    for(auto& acc : accounts){
        RedisCache::getInstance().deleteAccount(acc->getAccountNumber());
    }

    ss << accTotal << " account(s) deleted.\n";
    msg = {
        {"status", "success: "},
        {"messsage", ss.str()}
    };
    return msg;
}