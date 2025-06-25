#pragma once

#include <sw/redis++/redis++.h>
#include <memory>
#include "SavingsAccount.hpp"
#include "CheckingAccount.hpp"

class RedisCache{
public:
    static RedisCache& getInstance(){
        static RedisCache instance;
        return instance;
    }

    void saveAccount(const Account& acc);
    std::unique_ptr<Account> loadAccount(int accNum);
    void deleteAccount(int accNum);
private:
    RedisCache();
    RedisCache(const RedisCache&) = delete;
    RedisCache& operator=(const RedisCache&) = delete;

    sw::redis::Redis redis;
};