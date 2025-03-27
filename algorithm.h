#ifndef PROGRAMMING_ASSIGNMENT_ALGORITHM_H
#define PROGRAMMING_ASSIGNMENT_ALGORITHM_H

#include <stdbool.h>  // Required for bool type

// Define struct BankAccount before using it anywhere
struct BankAccount {
    int accountNumber;
    char accountHolder[50];
    double balance;
    int pinCode;
    bool blocked;
};

// Function prototypes
struct BankAccount* loadAccountsFromCSV(const char *filename, int *accountCount);
const char* withdraw(struct BankAccount *account, double amount);
const char* deposit(struct BankAccount *account, double amount);
bool checkPin(struct BankAccount *account, int enteredPin);  // PIN verification
bool checkBlocked(struct BankAccount *account);
const char* changePin(struct BankAccount *account, int newPin1, int newPin2);
const char* showBalance (struct BankAccount *account);
struct BankAccount* findAccount(struct BankAccount *accounts, int counter, int accountNumber);
void logTransaction(int accountNumber, const char *transactionType, double originalBalance, double newBalance);
void displayReceipt(const char *accountHolder, const char *transactionType, double originalBalance, double newBalance);
void saveAccountsToCSV(const char *filename, struct BankAccount *accounts, int accountCount);

#endif // PROGRAMMING_ASSIGNMENT_ALGORITHM_H
