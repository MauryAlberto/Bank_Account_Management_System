#include "Bank.hpp"

void displayMenu(){
    std::cout << "\n=== Bank Account Management System ===\n";
    std::cout << "1. Create Account\n";
    std::cout << "2. Deposit\n";
    std::cout << "3. Withdraw\n";
    std::cout << "4. Display Account\n";
    std::cout << "5. Display All Accounts\n";
    std::cout << "6. Modify Account\n";
    std::cout << "7. Close Account\n";
    std::cout << "8. Save All Accounts\n";
    std::cout << "9. Exit\n";
    std::cout << "Select an option: ";
}

int main(){
    Bank bank;
    bank.loadAllAccounts();

    int choice;
    do{
        displayMenu();
        std::cin >> choice;

        switch(choice){
            case 1: bank.createAccount(); break;

            case 2: bank.deposit(); break;

            case 3: bank.withdraw(); break;

            case 4: bank.displayAccount(); break;

            case 5: bank.displayAllAccounts(); break;

            case 6: bank.modifyAccount(); break;

            case 7: bank.closeAccount(); break;

            case 8: bank.saveAllAccounts(); std::cout << "Accounts saved successfully.\n"; break;

            case 9: bank.saveAllAccounts(); std::cout << "Goodbye!\n"; break;

            default: std::cout << "Invalid option. Try again.\n";
        }        
    }while(choice != 9);

    return 0;
}