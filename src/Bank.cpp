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

    if (!obj.contains(key)){
        ss << "Missing field '" << key << "'";
        msg["status"] = "failed: ";
        msg["message"] = ss.str();
        return false;
    }

    if constexpr (std::is_same_v<T, int>) {
        const std::string strVal = obj[key].get<std::string>();
            
        // Check if the string is a valid integer
        if (strVal.empty() || strVal.find_first_not_of("-0123456789") != std::string::npos){
            ss << "Invalid integer format for '" << key << "'. Must contain only digits.";
            msg["status"] = "failed: ";
            msg["message"] = ss.str();
            return false;
        }

        try {
            size_t pos = 0;
            long num = std::stol(strVal, &pos); // Using stol to detect overflow
            
            // Ensure the entire string was parsed (no trailing chars)
            if (pos != strVal.length()){
                ss << "Invalid integer format for '" << key << "'. Trailing characters detected.";
                msg["status"] = "failed: ";
                msg["message"] = ss.str();
                return false;
            }

            // Check for int range
            if (num < std::numeric_limits<int>::min() || num > std::numeric_limits<int>::max()){
                ss << "Integer value for '" << key << "' is out of range.";
                msg["status"] = "failed: ";
                msg["message"] = ss.str();
                return false;
            }

            out = static_cast<int>(num);
            return true;
        }
        catch (const std::invalid_argument&){
            ss << "Invalid integer format for '" << key << "'";
            msg["status"] = "failed: ";
            msg["message"] = ss.str();
            return false;
        }
        catch (const std::out_of_range&){
            ss << "Integer value for '" << key << "' is out of range.";
            msg["status"] = "failed: ";
            msg["message"] = ss.str();
            return false;
        }
    }else if constexpr (std::is_same_v<T, double>){
        const std::string strVal = obj[key].get<std::string>();

        try {
            size_t pos = 0;
            double num = std::stod(strVal, &pos);

            // Ensure the entire string was parsed (no trailing chars)
            if (pos != strVal.length()){
                ss << "Invalid double format for '" << key << "'. Trailing characters detected.";
                msg["status"] = "failed: ";
                msg["message"] = ss.str();
                return false;
            }

            out = num;
            return true;
        }
        catch (const std::invalid_argument&){
            ss << "Invalid double format for '" << key << "'";
            msg["status"] = "failed: ";
            msg["message"] = ss.str();
            return false;
        }
        catch (const std::out_of_range&){
            ss << "Double value for '" << key << "' is out of range.";
            msg["status"] = "failed: ";
            msg["message"] = ss.str();
            return false;
        }

        ss << "Invalid type for '" << key << "'. Expected double.";
        msg["status"] = "failed: ";
        msg["message"] = ss.str();
        return false;
    }else if constexpr (std::is_same_v<T, std::string>){
        out = obj[key].get<T>();
        return true;
    }else{
        ss << "Unsupported type for '" << key << "'.";
        msg["status"] = "failed: ";
        msg["message"] = ss.str();
        return false;
    }
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

    if(!validateJsonField(accJson, "amount", msg, amount)){
        return msg;
    }

    Account* acc = findAccount(accNum);
    if(acc == nullptr){
        std::stringstream ss;
        ss << "Account #" << accNum << " not found.";
        msg["status"] = "failed: ";
        msg["message"] = ss.str();
        return msg;
    }

    if(amount < 0.0){
        std::stringstream ss;
        ss << "Cannot deposit negative amount.";
        msg["status"] = "failed: ";
        msg["message"] = ss.str();
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
        ss << "Account #" << accNum << " not found.";
        msg["status"] = "failed: ";
        msg["message"] = ss.str();
        return msg;
    }

    if(amount < 0.0){
        std::stringstream ss;
        ss << "Cannot withdraw negative amount.";
        msg["status"] = "failed: ";
        msg["message"] = ss.str();
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
        ss << "Account #" << accNum << " not found.";
        msg["status"] = "failed: ";
        msg["message"] = ss.str();
        return msg;
    }
    
    msg = acc->display();
    return msg;
}

/* json Bank::displayAllAccounts() const {
    json allAccounts = json::array();
    for(const auto& acc : accounts){
        allAccounts.push_back(acc->display());
    }
    return allAccounts;
} */

json Bank::displayAllAccounts() const {
    json result;
    result["accounts"] = json::array();
    
    for(const auto& acc : accounts) {
        if(!acc) continue;
        
        json accData;
        accData["accountType"] = acc->getAccountType(); // "SAVINGS" or "CHECKING"
        accData["accountNumber"] = acc->getAccountNumber();
        accData["holderName"] = acc->getHolderName();
        accData["balance"] = acc->getBalance();
        
        if(acc->getAccountType() == "SAVINGS") {
            accData["interestRate"] = dynamic_cast<SavingsAccount*>(acc.get())->getInterestRate();
        } else {
            accData["overdraftLimit"] = dynamic_cast<CheckingAccount*>(acc.get())->getOverDraftLimit();
        }
        
        result["accounts"].push_back(accData);
    }
    
    result["status"] = "success";
    result["message"] = "Accounts retrieved";
    return result;
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
        ss << "Acount #" << accNum << " closed succesfully.";
        msg["status"] = "success: ";
        msg["message"] = ss.str();
        return msg;
    }else{
        ss << "Account #" << accNum << " not found.";
        msg["status"] = "failed: ";
        msg["message"] = ss.str();
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
        ss << "Account #" << accNum << " not found.";
        msg["status"] = "failed: ";
        msg["message"] = ss.str();
        return msg;
    }

    std::string newName;
    if(validateJsonField(accJson, "holderName", msg, newName)){
        if(newName != "0"){
            acc->setHolderName(newName);
        }
    }else{
        return msg;
    }

    double newBalance;
    if(validateJsonField(accJson, "balance", msg, newBalance)){
        if(newBalance != 0.0){
            acc->setBalance(newBalance);
        }
    }else{
        return msg;
    }

    if(acc->getAccountType() == "SAVINGS"){
        double newInterestRate;
        auto* savings = dynamic_cast<SavingsAccount*>(acc);
        if(validateJsonField(accJson, "interestRate", msg, newInterestRate)){
            if(newInterestRate != 0.0){
                savings->setInterestRate(newInterestRate);
            }
        }else{
            return msg;
        }
    }else if(acc->getAccountType() == "CHECKING"){
        int newOverdraftLimit;
        auto* checking = dynamic_cast<CheckingAccount*>(acc);
        if(validateJsonField(accJson, "overdraftLimit", msg, newOverdraftLimit)){
            if(newOverdraftLimit != 0){
                checking->setOverDraftLimit(newOverdraftLimit);
            }
        }else{
            return msg;
        }
    }else{
        ss << "Unknown account type.";
        msg["status"] = "failed: ";
        msg["message"] = ss.str();
        return msg;
    }

    RedisCache::getInstance().saveAccount(*acc);
    ss << "Account #" << acc->getAccountNumber() << " updated successfully.";
    msg["status"] = "success: ";
    msg["message"] = ss.str();
    return msg;
}

json Bank::saveAllAccounts() const {
    std::stringstream ss;
    json msg;
    for(const auto& acc : accounts){
        RedisCache::getInstance().saveAccount(*acc);
    }

    ss << accounts.size() << " account(s) saved to Redis successfully.";
    msg["status"] = "success: ";
    msg["message"] = ss.str();
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

    ss << accounts.size() << " account(s) loaded from Redis sucessfully.";
    msg["status"] = "success: ";
    msg["message"] = ss.str();
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
    std::string choice;
    std::stringstream ss;

    if(!validateJsonField(accJson, "choice", msg, choice)){
        return msg;
    }

    if(choice == "ONE"){
        int accNum;
        if(!validateJsonField(accJson, "accountNumber", msg, accNum)){
            return msg;
        }

        Account* acc = findAccount(accNum);
        if(acc == nullptr){
            ss << "Account #" << accNum << " not found.";
            msg["status"] = "failed: ";
            msg["message"] = ss.str();
            return msg;
        }

        if(acc->getAccountType() == "SAVINGS"){
            auto* savings = dynamic_cast<SavingsAccount*>(acc);
            msg = savings->applyInterest();
            RedisCache::getInstance().saveAccount(*acc);
        }else{
            ss << "This is not a savings account.";
            msg["status"] = "failed: ";
            msg["message"] = ss.str();
            return msg;
        }
    }else if(choice == "ALL"){
        int count = 0;
        for(const auto& acc : accounts){
            if(acc->getAccountType() == "SAVINGS"){
                auto* savings = dynamic_cast<SavingsAccount*>(acc.get());
                savings->applyInterest();
                RedisCache::getInstance().saveAccount(*acc);
                ++count;
            }
        }
        ss << "Interest applied to " << count << " savings account(s).";
        msg["status"] = "success: ";
        msg["message"] = ss.str();
    }else{
        ss << "Invalid target: Must be 'ONE' or 'ALL'.";
        msg["status"] = "failed: ";
        msg["message"] = ss.str();
        return msg;
    }

    return msg;
}

json Bank::exportAllAccountsToFile() const {
    std::stringstream ss;
    json msg;
    
    std::ofstream file("accounts_export.json", std::ios::trunc);
    
    if(!file.is_open()){
        ss << "Failed to open JSON export file.";
        msg["status"] = "failed: ";
        msg["message"] = ss.str();
        return msg;
    }

    json allAccounts = json::array();
    for(const auto& acc : accounts){
        allAccounts.push_back(acc->toJson());
    }

    file << allAccounts.dump(4);
    ss << accounts.size() << " account(s) exported successfully.";
    msg["status"] = "success: ";
    msg["message"] = ss.str();
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
        ss << "Account #" << accNum << " already exists.";
        msg["status"] = "failed: ";
        msg["message"] = ss.str();
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
        ss << "Unkown account type. Account creation failed.";
        msg["status"] = "failed: ";
        msg["message"] = ss.str();
        return msg;
    }

    accounts.push_back(std::move(newAcc));
    RedisCache::getInstance().saveAccount(*accounts.back());

    ss << "Account #" << accNum << " created successfully.";
    msg["status"] = "success: ";
    msg["message"] = ss.str();
    return msg;
}

json Bank::deleteAllAccounts(){
    std::stringstream ss;
    json msg;
    size_t accTotal= accounts.size();
    
    for(auto& acc : accounts){
        RedisCache::getInstance().deleteAccount(acc->getAccountNumber());
    }

    ss << accTotal << " account(s) deleted.";
    msg["status"] = "success: ";
    msg["message"] = ss.str();
    return msg;
}