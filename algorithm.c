//
// Created by Kirill Tumoian on 25.03.2025.
//
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>  // For date/time


struct BankAccount {
    int accountNumber;
    char accountHolder[50];
    double balance;
    int pinCode;
    bool blocked;
};

bool checkPin(struct BankAccount *account, int enteredPin) {
    if (enteredPin == account->pinCode) {
        return true;
    }
    return false;
}

bool checkBlocked(struct BankAccount *account) {
    return account->blocked;
}

const char* withdraw(struct BankAccount *account, double amount) {
    if (amount <= 0) {
        return "Invalid withdrawal amount!";
    }
    // Ensure the withdrawal amount is a multiple of 5.
    if ((int)amount % 5 != 0) {
        return "Amount must be a multiple of 5, 10 or 20!";
    }
    if (account->balance >= amount) {
        account->balance -= amount;
        static char msg[100];
        snprintf(msg, 100, "Withdrawal successful! New balance: £%.2f", account->balance);
        return msg;
    }
    return "Insufficient funds!";
}

const char* deposit(struct BankAccount *account, double amount) {
    if (amount <= 0) {
        return "Invalid deposit amount!";
    }
    account->balance += amount;
    static char msg[100];
    snprintf(msg, sizeof(msg), "Deposit successful! New balance: £%.2f", account->balance);
    return msg;
}

const char* changePin(struct BankAccount *account, int newPin1, int newPin2) {
    if (newPin1 != newPin2) {
        return "Error: PINs do not match!";
    }
    if (newPin1 < 1000 || newPin1 > 9999) { // Ensure exactly 4 digits
        return "Error: PIN must be exactly 4 digits!";
    }
    account->pinCode = newPin1;
    return "PIN successfully changed!";
}

const char* showBalance(struct BankAccount *account) {
    static char msg[100];
    snprintf(msg, sizeof(msg), "Your current balance is: £%.2f", account->balance);
    return msg;
}

// Logging function that appends the transaction details to "log.txt"
void logTransaction(int accountNumber, const char *transactionType, double originalBalance, double newBalance) {
    FILE *logFile = fopen("log.txt", "a");
    if (logFile != NULL) {
        fprintf(logFile, "Account %d - %s: Original Balance = £%.2f, New Balance = £%.2f\n",
                accountNumber, transactionType, originalBalance, newBalance);
        fclose(logFile);
    } else {
        printf("Error: Could not open log file.\n");
    }
}

// Function to optionally display a receipt on the screen
void displayReceipt(const char *accountHolder, const char *transactionType, double originalBalance, double newBalance) {
    char choice;
    // Get the current date/time
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char dateTime[26];
    strftime(dateTime, 26, "%Y-%m-%d %H:%M:%S", tm_info);  // Format as: YYYY-MM-DD HH:MM:SS
    // Ask if user wants the receipt
    while (1) {
        printf("Do you want a receipt? (y/n):\n>>> ");
        scanf(" %c", &choice);  // Read a single character, ignore leading whitespace
        // Validate the input
        if (choice == 'y' || choice == 'Y' || choice == 'n' || choice == 'N') {
            break;  // Exit loop if valid input
        } else {
            // Clear invalid input
            while (getchar() != '\n');  // Clear the buffer of any extra characters
            printf("Invalid input! Please enter 'y' for yes or 'n' for no.\n");
        }
    }
    // Proceed if the input is valid
    if (choice == 'y' || choice == 'Y') {
        // Print the receipt
        printf("\n----- ATM RECEIPT -----\n");
        printf("Date/Time: %s\n", dateTime);
        printf("Account Holder: %s\n", accountHolder);
        printf("----------------------\n");
        printf("Transaction: %-12s\n", transactionType);
        printf("Original Balance: £%10.2f\n", originalBalance);
        printf("New Balance:      £%10.2f\n", newBalance);
        printf("----------------------\n");
        printf("Thank you for using our ATM!\n");
        printf("----------------------\n");
    }
}


// This function reads the CSV file and fills a static array of BankAccount.
// It returns a pointer to that array and sets *accountCount to the number of accounts read.
struct BankAccount* loadAccountsFromCSV(const char *filename, int *accountCount) {
    int numberOfAccounts = 2;
    struct BankAccount *accountList = malloc(numberOfAccounts * sizeof(struct BankAccount));
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open %s\n", filename);
        *accountCount = 0;
        return accountList;
    }
    char line[256];
    // Skip the header line.
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        *accountCount = 0;
        return accountList;
    }
    *accountCount = 0;
    while (fgets(line, sizeof(line), file) != NULL && *accountCount < numberOfAccounts) {
        int accNum, pin, blockedInt;
        double balance;
        char name[50];
        // Parse the CSV line.
        if (sscanf(line, "%d,%49[^,],%lf,%d,%d", &accNum, name, &balance, &pin, &blockedInt) == 5) {
            accountList[*accountCount].accountNumber = accNum;
            strcpy(accountList[*accountCount].accountHolder, name);
            accountList[*accountCount].balance = balance;
            accountList[*accountCount].pinCode = pin;
            accountList[*accountCount].blocked = (blockedInt != 0);
            (*accountCount)++;
        }
    }
    fclose(file);
    return accountList;
}


struct BankAccount* findAccount(struct BankAccount *accounts, int counter, int accountNumber) {
    for (int i = 0; i < counter; i++) {
        if (accounts[i].accountNumber == accountNumber) {
            return &accounts[i];
        }
    }
    return NULL;
}

void saveAccountsToCSV(const char *filename, struct BankAccount *accounts, int accountCount) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not open %s for writing.\n", filename);
        return;
    }
    // Write the CSV header
    fprintf(file, "AccountNumber,AccountHolder,Balance,PinCode,Blocked\n");
    // Write each account's details
    for (int i = 0; i < accountCount; i++) {
        fprintf(file, "%d,%s,%.2f,%d,%d\n",
                accounts[i].accountNumber,
                accounts[i].accountHolder,
                accounts[i].balance,
                accounts[i].pinCode,
                accounts[i].blocked ? 1 : 0);
    }
    fclose(file);
}

// Helper function to safely read an integer
int getValidInt() {
    int num;
    char ch;
    while (scanf("%d", &num) != 1) {
        while ((ch = getchar()) != '\n' && ch != EOF);  // Clear buffer
        printf("Invalid input. Please try again:\n>>> ");
    }
    return num;
}

// Helper function to safely read a double
double getValidDouble() {
    double num;
    char ch;
    while (scanf("%lf", &num) != 1) {
        while ((ch = getchar()) != '\n' && ch != EOF);
        printf("Invalid input. Please try again:\n>>> ");
    }
    return num;
}
