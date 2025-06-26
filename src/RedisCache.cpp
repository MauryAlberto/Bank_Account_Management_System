#include "RedisCache.hpp"

using namespace sw::redis;

RedisCache::RedisCache() : redis("tcp://127.0.0.1:6379") {}

void RedisCache::saveAccount(const Account& acc){
    std::string key = "account:" + std::to_string(acc.getAccountNumber());

    std::unordered_map<std::string, std::string> fields = {
        {"type", acc.getAccountType()},
        {"name", acc.getHolderName()},
        {"balance", std::to_string(acc.getBalance())}
    };

    if(acc.getAccountType() == "SAVINGS"){
        const auto* savings = dynamic_cast<const SavingsAccount*>(&acc);
        fields["interest"] = std::to_string(savings->getInterestRate());
    }else if(acc.getAccountType() == "CHECKING"){
        const auto* checking = dynamic_cast<const CheckingAccount*>(&acc);
        fields["overdraft"] = std::to_string(checking->getOverDraftLimit());
    }
    
    try{
        redis.hset(key, fields.begin(), fields.end());
        redis.sadd("accounts", std::to_string(acc.getAccountNumber()));
    }catch(const sw::redis::Error &err){
        std::cerr << "Redis Error: " << err.what() << std::endl;
    }
}

std::unique_ptr<Account> RedisCache::loadAccount(int accNum){
    std::string key = "account:" + std::to_string(accNum);

    if(!redis.exists(key)){
        std::cerr << "Account number #" << accNum << " does not exist.\n";
        return nullptr;
    }
    
    std::unordered_map<std::string, std::string> accountData;
    redis.hgetall(key, std::inserter(accountData, accountData.begin()));

    std::string name = accountData["name"];
    double balance = std::stod(accountData["balance"]);
    std::string type = accountData["type"];

    if(type == "SAVINGS"){
        double interestRate = std::stod(accountData["interest"]);
        return std::make_unique<SavingsAccount>(accNum, name, balance, interestRate);
    }else if(type == "CHECKING"){
        int overDraftLimit = std::stoi(accountData["overdraft"]);
        return std::make_unique<CheckingAccount>(accNum, name, balance, overDraftLimit);
    }else{
        std::cerr << "Unkown account type.\n";
        return nullptr;
    }
}

void RedisCache::deleteAccount(int accNum){
    std::string key = "account:" + std::to_string(accNum);
    redis.del(key);
    redis.srem("accounts", std::to_string(accNum));
}

std::vector<std::string> RedisCache::getAllAccountKeys(){
    std::vector<std::string> keys;
    sw::redis::Cursor cursor = 0;

    while(true){
        cursor = redis.scan(cursor, "account:*", 100, std::back_inserter(keys));
        if(cursor == 0){
            break;
        }
    }

    return keys;
}