# Bank Account Management System (Modern C++)

A robust and extensible **Bank Account Management System** written in **Modern C++17**, showcasing real-world software design using **Object-Oriented Programming**, **Singleton Design Pattern**, **Smart Pointers**, **Redis Persistence**, **Server-Client Architecture**, and **Dynamic Polymorphism**.

## Project Overview

This project provides a bank account management system with:
- Server-side account management with Redis persistence
- Client applications for account operations
- Support for Savings and Checking accounts
- Network communication between components

---

## Features

### Account Operations
- Create `Savings` or `Checking` accounts
- Deposit and withdraw funds
- Modify account details (name, balance, interest rate, overdraft limit)
- View a specific account or list all accounts
- Close (delete) accounts
- Apply interest to savings accounts
- Enforce overdraft limits on checking accounts

### Data Persistence
- All account data is stored in Redis
- Automatic synchronization between server and database
- Efficient data serialization/deserialization

### Modern C++ Features
- Function templates for type-safe JSON validation
- Smart pointers for automatic memory management
- constexpr for compile-time checks
- std::thread for handling multiple clients
- std::chrono for adding delays

### Server-Client Architecture
- Dedicated server process managing all accounts
- Multiple client applications can connect
- Network communication via TCP/IP
- Thread-safe operations

---

## Project Structure

```
Bank_Account_Management_System/
│
├── client/ # Client application code
│ ├── main.cpp # Client entry point
│ └── CMakeLists.txt
│
├── server/ # Server application code
│ ├── BankServer.cpp # Server implementation
│ └── CMakeLists.txt
│
├── src/ # Core logic
│ ├── Account.cpp
│ ├── Bank.cpp # Main banking logic
│ ├── CheckingAccount.cpp
│ ├── CMakeLists.txt
│ ├── Network.cpp # Network communication
│ ├── RedisCache.cpp # Redis integration
│ └── SavingsAccount.cpp
│
├── include/ # Header files
│
├── .gitignore
├── CMakeLists.txt
└── README.md # Project documentation
```

---

## Concepts & Techniques Used

### Object-Oriented Programming (OOP)
- Abstract base class `Account` with **pure virtual functions**
- Derived classes `SavingsAccount` and `CheckingAccount` implement polymorphic behavior
- Encapsulation of account logic inside respective classes
- Use of **virtual destructors** for proper cleanup

### Singleton Design Pattern Implmentation
- Single instance existence
- Global access point
- Thread-safe initialization (via static local variable)
- Controlled instantiation

### Dynamic Polymorphism
- Base class pointers allow storing `SavingsAccount` and `CheckingAccount` in the same container
- Uses `dynamic_cast` for safely downcasting during type-specific operations

### STL Container
- `std::vector<std::unique_ptr<Account>>` stores all account objects
- Efficient insertion, removal, and iteration over accounts

### Smart Pointers
- Uses `std::unique_ptr<Account>` to manage dynamic memory safely
- Ensures **RAII** (Resource Acquisition Is Initialization) — no manual `delete` needed
- Prevents memory leaks and ownership ambiguity

### Redis Integration
- All data persisted in Redis database
- Automatic synchronization
- Efficient serializartion/deserialization

### Server-Client Architecture
- TCP/IP network communication
- Thread-safe operations
- JSON-based protocol
- Separation of concerns

### JSON Handling
- Type-safe validation with templates
- Error handling for malformed input
- Efficient parsing and serialization

---

## Sample Code Highlights

### Function Template for JSON Validation

```cpp
template <typename T>
bool validateJsonField(const json& obj, const std::string& key, json& msg, T& out){
    std::stringstream ss;

    if(!obj.contains(key)){
        ss << "Missing field '" << key << "'.\n";
        msg["status"] = "failed: ";
        msg["message"] = ss.str();
        return false;
    }

    if constexpr(std::is_same_v<T, int>){
        if(!obj[key].is_number_integer()){
            ss << "Invalid type for key: " << key << ". Expected " << getTypeName<T>() << ".\n";
            msg["status"] = "failed: ";
            msg["message"] = ss.str();
            return false;
        }
    }
    //... other type checks
    out = obj[key].get<T>();
    return true;
}
```

### Smart Pointer Usage

```cpp
std::vector<std::unique_ptr<Account>> accounts;
std::unique_ptr<Account> newAcc;
newAcc = std::make_unique<SavingsAccount>(accNum, name, balance, rate);
accounts.push_back(std::move(newAcc));
```

### Redis Integration

```cpp
RedisCache::getInstance().saveAccount(*acc);
auto account = Redis::Cache::getInstance().loadAccount(accNum);
```

### Bank Singleton Implementation
```cpp
class Bank {
public:
    static Bank& getInstance() {
        static Bank instance;  // Thread-safe initialization
        return instance;
    }
private:
    Bank() = default;                   // Private constructor
    Bank(const Bank&) = delete;         // Delete copy constructor
    Bank& operator=(const Bank&) = delete; // Delete assignment operator
};
```

### RedisCache Singleton Implementation
```cpp
class RedisCache {
public:
    static RedisCache& getInstance() {
        static RedisCache instance;  // Thread-safe initialization
        return instance;
    }
private:
    RedisCache();                   // Private constructor
    RedisCache(const RedisCache&) = delete; // Delete copy constructor
    RedisCache& operator=(const RedisCache&) = delete; // Delete assignment operator
};
```

---

## Running the Program

### Requirements
- C++17 or higher compiler
- CMake 
- Redis server
- redis-plus-plus
- nlohmann/json

### Build & Run

```bash
# Clone the repository
git clone https://github.com/MauryAlberto/Bank_Account_Management_System.git
cd Bank_Account_Management_System

# Build
mkdir build && cd build
cmake ..
make

# Start Redis
redis-server

# Run server (in separate terminal within build)
./server/bank_server

# Run client (in another terminal within build)
./client/bank_client
```

---

## What You'll Learn

By studying or extending this project, you'll gain experience with:

- Real-world class design
- Data persistence with Redis
- Modern C++ memory management (smart pointers)
- Clean separation of concerns
- Applying OOP principles in a practical application

---

## Ideas for Extension

- Add login/authentication system per account
- Add transaction history logging
- Implement a GUI using Qt or ImGui
- Add unit tests using GoogleTest
- Add SSL/TSL encryption

---
