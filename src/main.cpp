#include "Bank.hpp"

int main(){

    SavingsAccount acc1(100, "Maury Alberto", 1000, 0.02);
    CheckingAccount acc2(101, "First Middle Last", 2000, 100);

    acc1.display();
    acc2.display();

    acc1.saveToFile();
    acc2.saveToFile();

    acc1.deposit(100);
    acc2.deposit(100);

    acc1.display();
    acc2.display();

    SavingsAccount temp_acc1(100, "", 0, 0);
    CheckingAccount temp_acc2(101, "", 0, 0);
    
    temp_acc1.loadFromFile();
    temp_acc2.loadFromFile();

    temp_acc1.display();
    temp_acc2.display();

    std::cout << acc1.getAccountNumber() << std::endl;
    std::cout << acc2.getAccountNumber() << std::endl;

    acc1.applyInterest();
    acc2.withdraw(2000);

    acc1.display();
    acc2.display();

    acc1.setHolderName("John Doe");
    acc2.setHolderName("Another Name");

    acc1.display();
    acc2.display();

    acc1.withdraw(1000);
    acc2.withdraw(100);

    acc1.display();
    acc2.display();

    acc2.withdraw(100);
    acc2.display();

    std::cout << acc1.getBalance() << std::endl;
    std::cout << acc2.getBalance() << std::endl;

    /* acc2.withdraw(100); */ //exceeding over draft limit 
    /* acc2.display(); */

    /* acc1.withdraw(1000); */ // withdrawing more than the current balance

    /* Bank bank;
    bank.loadAllAccounts();

    int choice;
    do{
        // Display menu
        // Call corresponding bank method
    }while(choice != 0);

    bank.saveAllAccounts(); */

    return 0;
}