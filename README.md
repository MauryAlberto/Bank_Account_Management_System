# Bank Account Management System (Modern C++)

A robust and extensible console-based **Bank Account Management System** written in **Modern C++17**, showcasing real-world software design using **Object-Oriented Programming**, **smart pointers**, **file I/O with persistence**, and **dynamic polymorphism**.

## Project Overview

This project allows users to create, manage, and persist bank accounts (Savings and Checking). The system supports operations such as deposits, withdrawals, modifying account details, applying interest, and saving/loading accounts from disk.

---

## Features

### Account Operations
- Create `Savings` or `Checking` accounts
- Deposit and withdraw funds
- Modify account details (name, interest rate, overdraft limit)
- View a specific account or list all accounts
- Close (delete) accounts
- Apply interest to savings accounts
- Enforce overdraft limits on checking accounts

### Data Persistence
- All account data is stored in a file `accounts.dat`
- On startup, the file is parsed to rebuild accounts in memory
- Data is saved with proper serialization/deserialization logic

### Clean Console Interface
- User-friendly menu-driven interface
- Input validation and informative error messages

---

## Project Structure

```
Bank_Account_Management_System/
│
├── include/                # Header files for all classes
│   ├── Account.hpp
│   ├── Bank.hpp
│   ├── CheckingAccount.hpp
│   └── SavingsAccount.hpp
│
├── src/                    # Source files
│   ├── Account.cpp
│   ├── Bank.cpp
│   ├── CheckingAccount.cpp
│   ├── main.cpp
│   └── SavingsAccount.cpp
│
├── .gitignore
├── accounts.dat            # Persistent account data
├── Makefile                # Build system
└── README.md               # Project documentation
```

---

## Concepts & Techniques Used

### Object-Oriented Programming (OOP)
- Abstract base class `Account` with **pure virtual functions**
- Derived classes `SavingsAccount` and `CheckingAccount` implement polymorphic behavior
- Encapsulation of account logic inside respective classes
- Use of **virtual destructors** for proper cleanup

### Smart Pointers
- Uses `std::unique_ptr<Account>` to manage dynamic memory safely
- Ensures **RAII** (Resource Acquisition Is Initialization) — no manual `delete` needed
- Prevents memory leaks and ownership ambiguity

### File I/O & Persistence
- Uses `std::ifstream` and `std::ofstream` to read/write to `accounts.dat`
- Implements **manual serialization/deserialization**
- Reconstructs full program state on launch

### Dynamic Polymorphism
- Base class pointers allow storing `SavingsAccount` and `CheckingAccount` in the same container
- Uses `dynamic_cast` for safely downcasting during type-specific operations

### STL Containers
- `std::vector<std::unique_ptr<Account>>` stores all account objects
- Efficient insertion, removal, and iteration over accounts

### File Parsing
- Uses `std::stringstream` to parse account data
- Converts strings into appropriate types
- Handles safe formatting (e.g., replacing underscores in names)

### Time Complexity Optimized Saving
- All accounts are saved in **O(n)** time using a temporary file + `rename()`
- Avoids inefficient repeated file rewrites

### Input Validation
- Checks for valid account types, positive balances, valid account numbers
- Ensures robust user interaction

---

## Sample Data Format (`accounts.dat`)

```txt
SAVINGS 1001 John_Doe 5000.00 0.02
CHECKING 1002 Alice_Smith 2000.00 1000
```

> Spaces in names are replaced with underscores for safe parsing.

---

## Running the Program

### Requirements
- C++17 or higher
- Make

### Build & Run

```bash
make        # Build the project
make run    # Execute the program
```

---

## Sample Code Snippets

### Virtual Function in Base Class

```cpp
virtual void saveToFile() const = 0;
```

### Smart Pointer Usage

```cpp
std::vector<std::unique_ptr<Account>> accounts;
accounts.push_back(std::make_unique<SavingsAccount>(...));
```

### File Parsing Example

```cpp
std::stringstream ss(line);
std::string type, name;
int accNum;
double balance;
ss >> type >> accNum >> name >> balance;
```

---

## What You'll Learn

By studying or extending this project, you'll gain experience with:

- Real-world class design
- Data persistence with manual serialization
- Modern C++ memory management (smart pointers)
- Clean separation of concerns
- Applying OOP principles in a practical application
- Handling file parsing, validation, and edge cases

---

## Ideas for Extension

- Add login/authentication system per account
- Add transaction history logging
- Use JSON or SQLite for structured data storage
- Implement a GUI using Qt or ImGui
- Add unit tests using GoogleTest
- Integrate Redis for fast read/write operations
- Implement multithreading and socket programming of multi-user access to the Bank

---
