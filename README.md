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
│   ├── main.cpp # Client entry point
│   └── CMakeLists.txt
│
├── server/ # Server application code
│   ├── BankServer.cpp # Server implementation
│   └── CMakeLists.txt
│
├── src/ # Core logic
│   ├── Account.cpp
│   ├── Bank.cpp # Main banking logic
│   ├── CheckingAccount.cpp
│   ├── CMakeLists.txt
│   ├── Network.cpp # Network communication
│   ├── RedisCache.cpp # Redis integration
│   └── SavingsAccount.cpp
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

### Singleton Design Pattern Implementation

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
- Efficient serialization/deserialization

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

```cpp
// Example: Function Template for JSON Validation

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

## Command-Line Interface (CLI)

Once the client connects to the server, it enters a CLI prompt that accepts structured commands. Each command is translated into a JSON request and sent to the server for processing.

You will see a prompt like:

```
127.0.0.1:6000>
```

### Command List

| Command              | Purpose                                    | Usage Example                        | Sample Output                      |
| -------------------- | ------------------------------------------ | ------------------------------------ | ---------------------------------- |
| `CREATE`             | Create a new account (Savings or Checking) | `CREATE SAVINGS 101 Bob 1000 0.03`   | `success: Account # created`       |
| `DISPLAY_ALL`        | Show all accounts in the system            | `DISPLAY_ALL`                        | Formatted list of accounts         |
| `DISPLAY_ONE`        | Show one specific account                  | `DISPLAY_ONE 101`                    | Details for account 101            |
| `DEPOSIT`            | Add funds to an account                    | `DEPOSIT 101 200`                    | `success: New balance of $`        |
| `WITHDRAW`           | Withdraw funds from an account             | `WITHDRAW 101 100`                   | `success: New balance of $`        |
| `TRANSFER`           | Transfer funds from an account             | `TRANSFER 101 100 $`                 | `success: Transfer successful`     |
| `MODIFY`             | Modify an existing account                 | `MODIFY 101 Bob 1500 0.04`           | `success: Account # updated`       |
| `DELETE`             | Delete an account by account number        | `DELETE 101`                         | `success: Account # closed`        |
| `DELETE_ALL`         | Deletes all accounts in the system         | `DELETE_ALL`                         | `success: All accounts deleted`    |
| `APPLY_INTEREST_ONE` | Apply interest to one savings account      | `APPLY_INTEREST_ONE 101`             | `success: New balance of $`        |
| `APPLY_INTEREST_ALL` | Apply interest to all savings accounts     | `APPLY_INTEREST_ALL`                 | `success: Interest applied to all` |
| `EXPORT_JSON`        | Export all account data as a JSON dump     | `EXPORT_JSON`                        | `success: All accounts exported`   |
| `EXIT`               | Closes the client connection               | `EXIT`                               | `success: Closing Bank`            |

### Example Session

```
127.0.0.1:6000> CREATE SAVINGS 101 Bob 1000 0.03
success: Account created

127.0.0.1:6000> DEPOSIT 101 200
success: Deposit successful

127.0.0.1:6000> DISPLAY_ONE 101
success: SAVINGS 101 Bob 1200 0.03
```
---

## What You'll Learn

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

