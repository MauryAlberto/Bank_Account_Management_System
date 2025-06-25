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