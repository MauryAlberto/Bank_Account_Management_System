#include "CheckingAccount.hpp"

json CheckingAccount::withdraw(double amount) {
    std::lock_guard<std::mutex> lock(mtx);
    std::stringstream ss;
    json msg;
    if(balance + overdraftLimit - amount >= 0){
        balance -= amount;
        ss << "New balance of $" << balance << "\n";
        msg = {
            {"status", "success: "},
            {"message", ss.str()}
        };
        return msg;
    }else{
        ss << "Overdraft limit exceeded.\n";
        msg = {
            {"status", "success"},
            {"message", ss.str()}
        };
        return msg;
    }   
}

json CheckingAccount::display() const {
    std::stringstream ss;
    json msg;
    ss << "CHECKING " << accountNumber << " " << holderName << " " << balance << " " << overdraftLimit;
    msg = {
        {"status", "success: "},
        {"message", ss.str()}
    };
    return msg;
}

void CheckingAccount::setOverDraftLimit(int newLimit) {
    std::lock_guard<std::mutex> lock(mtx);
    overdraftLimit = newLimit;
}

std::string CheckingAccount::getHolderName() const {
    return holderName;
}

std::string CheckingAccount::getAccountType() const {
    return "CHECKING";
}

int CheckingAccount::getOverDraftLimit() const {
    return overdraftLimit;
}

json CheckingAccount::toJson() const {
    return {
        {"accountType", "CHECKING"},
        {"accountNumber", accountNumber},
        {"holderName", holderName},
        {"balance", balance},
        {"overdraftLimit", overdraftLimit}
    };
}