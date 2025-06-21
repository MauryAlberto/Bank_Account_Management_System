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