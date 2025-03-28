//
// Created by Kirill Tumoian on 28.03.2025.
//
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h> // For unit tests
#include <time.h>  // For date/time

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
int getValidInt();
double getValidDouble();


// Define struct BankAccount before using it anywhere
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

// Test Checking Pin
void test_checkPin() {
    struct BankAccount account = {123, "Test User", 100.0, 1234, false};
    assert(checkPin(&account, 1234) == 1);
    assert(checkPin(&account, 0000) == 0);
}

bool checkBlocked(struct BankAccount *account) {
    return account->blocked;
}

// Test blocked status check
void test_checkBlocked() {
    struct BankAccount account = {123, "Test User", 100.0, 1234, false};
    assert(checkBlocked(&account) == 0);
    account.blocked = true;
    assert(checkBlocked(&account) == 1);
}

const char* withdraw(struct BankAccount *account, double amount) {
    if (amount <= 0) {
        return "Invalid withdrawal amount!";
    }
    // Ensure the withdrawal amount is a multiple of 5.
    if ((int)amount % 10 != 0) {
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

// Test withdrawal function
void test_withdraw() {
    struct BankAccount account = {123, "Test User", 100.0, 1234, false};

    // Negative amount should fail.
    const char* result = withdraw(&account, -10);
    assert(strcmp(result, "Invalid withdrawal amount!") == 0);

    // Withdrawal amount that is not a multiple of 5 should fail.
    result = withdraw(&account, 7);
    assert(strcmp(result, "Amount must be a multiple of 5, 10 or 20!") == 0);

    result = withdraw(&account, 45);
    assert(strcmp(result, "Amount must be a multiple of 5, 10 or 20!") == 0);

    // Attempting to withdraw more than the balance.
    result = withdraw(&account, 110);
    assert(strcmp(result, "Insufficient funds!") == 0);

    // Valid withdrawal.
    result = withdraw(&account, 50);
    // Check that the message indicates success.
    assert(strstr(result, "Withdrawal successful!") != 0);
    // Verify that the account balance has been updated.
    assert(account.balance == 50.0);

    result = withdraw(&account, 50);
    // Check that the message indicates success.
    assert(strstr(result, "Withdrawal successful!") != 0);
    // Verify that the account balance has been updated.
    assert(account.balance == 0.0);

    result = withdraw(&account, 20);
    // Check that the message indicates success.
    assert(strstr(result, "Insufficient funds!") != 0);
    // Verify that the account balance has been updated.
    assert(account.balance == 0.0);
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

// Test deposit function
void test_deposit() {
    struct BankAccount account = {123, "Test User", 100.0, 1234, false};

    // Negative deposit should fail.
    const char* result = deposit(&account, -10);
    assert(strcmp(result, "Invalid deposit amount!") == 0);

    // Valid deposit.
    result = deposit(&account, 50);
    assert(strstr(result, "Deposit successful!") != 0);
    assert(account.balance == 150.0);

    result = deposit(&account, 1000);
    assert(strstr(result, "Deposit successful!") != 0);
    assert(account.balance == 1150.0);
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

// Test changing PIN
void test_changePin() {
    struct BankAccount account = {123, "Test User", 100.0, 1234, false};

    // Mismatched new PINs.
    const char* result = changePin(&account, 1111, 2222);
    assert(strcmp(result, "Error: PINs do not match!") == 0);

    // New PIN not exactly 4 digits.
    result = changePin(&account, 99, 99);
    assert(strcmp(result, "Error: PIN must be exactly 4 digits!") == 0);

    result = changePin(&account, 222222, 222222);
    assert(strcmp(result, "Error: PIN must be exactly 4 digits!") == 0);

    // Valid PIN change.
    result = changePin(&account, 4321, 4321);
    assert(strcmp(result, "PIN successfully changed!") == 0);
    assert(account.pinCode == 4321);
}

const char* showBalance(struct BankAccount *account) {
    static char msg[100];
    snprintf(msg, sizeof(msg), "Your current balance is: £%.2f", account->balance);
    return msg;
}

// Test balance display
void test_showBalance() {
    struct BankAccount account = {123, "Test User", 100.0, 1234, false};
    const char* result = showBalance(&account);
    // Check that the string contains the correct formatted balance.
    assert(strstr(result, "£100.00") != 0);
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

// Test finding an account
void test_findAccount() {
    struct BankAccount accounts[2] = {
            {1, "Kirill", 100.0, 1111, false},
            {2, "Madiyar", 200.0, 2222, false}
    };

    struct BankAccount* acc = findAccount(accounts, 2, 1);
    assert(acc != NULL);
    assert(acc->accountNumber == 1);
    acc = findAccount(accounts, 2, 3);
    assert(acc == NULL);
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


int main() {
    // Running the unit test functions
    test_checkPin();
    test_checkBlocked();
    test_withdraw();
    test_deposit();
    test_changePin();
    test_showBalance();
    test_findAccount();
    printf("\nAll unit tests passed successfully! Let's head to the ATM Machine itself;)\n\n");

    int accountCount;
    // Load accounts once at the beginning
    struct BankAccount *accounts = loadAccountsFromCSV("accounts.csv", &accountCount);
    if (accountCount == 0) {
        printf("No accounts loaded. Exiting.\n");
        return 1;
    }
    while (true) {
        struct BankAccount *account = NULL;
        int pinAttempts;
        // Card selection
        printf("\nWelcome to the ATM Machine created by Kirill!\n"
               "Select a card (e.g., 1 for Card 1, 2 for Card 2). Enter 0 to Quit the Program:\n>>> ");
        int selectedCard = getValidInt();
        if (selectedCard == 0) {
            printf("Exiting program. Thanks for using the ATM!\n");
            // Save updated accounts before exiting.
            saveAccountsToCSV("accounts.csv", accounts, accountCount);
            exit(0);
        }
        account = findAccount(accounts, accountCount, selectedCard);
        if (account == NULL) {
            printf("Invalid card selection.\n");
            continue;
        }
        if (checkBlocked(account)) {
            printf("This card is blocked. Please contact the bank.\n");
            continue;
        }
        // PIN verification
        pinAttempts = 0;
        bool pinVerified = false;
        while (pinAttempts < 3 && !pinVerified) {
            printf("Enter PIN (exactly 4 digits):\n>>> ");
            int pin = getValidInt();
            if (checkPin(account, pin)) {
                pinVerified = true;
            } else {
                pinAttempts++;
                printf("Incorrect PIN. Attempts left: %d\n", 3 - pinAttempts);
            }
        }
        if (!pinVerified) {
            account->blocked = true;
            logTransaction(account->accountNumber, "Card Retained", 0, 0);
            printf("Card has been retained due to too many incorrect attempts. Please contact the bank.\n");
            continue;
        }

        // Main transaction loop for the logged-in card
        int exitChoice = 0;
        while (!exitChoice) {
            printf("\n--- ATM Menu ---\n");
            printf("1. Change PIN\n");
            printf("2. Check Balance\n");
            printf("3. Withdraw\n");
            printf("4. Deposit\n");
            printf("5. Eject Card (return to card selection)\n");
            printf("6. Quit the ATM\n");
            printf("Select an option:\n>>> ");
            int choice = getValidInt();

            double originalBalance = account->balance;
            const char *result;
            switch (choice) {
                case 1: {
                    printf("Enter new PIN:\n>>> ");
                    int newPin1 = getValidInt();
                    printf("Re-enter new PIN:\n>>> ");
                    int newPin2 = getValidInt();
                    result = changePin(account, newPin1, newPin2);
                    printf("%s\n", result);
                    logTransaction(account->accountNumber, "Change PIN", 0, 0);
                    break;
                }
                case 2: {
                    result = showBalance(account);
                    printf("%s\n", result);
                    logTransaction(account->accountNumber, "Check Balance", originalBalance, account->balance);
                    break;
                }
                case 3: {
                    printf("Enter amount to withdraw:\n>>> ");
                    double amount = getValidDouble();
                    result = withdraw(account, amount);
                    printf("%s\n", result);
                    if (strstr(result, "successful") != NULL) {
                        logTransaction(account->accountNumber, "Withdrawal", originalBalance, account->balance);
                        displayReceipt(account->accountHolder, "Withdrawal", originalBalance, account->balance);
                    }
                    break;
                }
                case 4: {
                    printf("Enter amount to deposit:\n>>> ");
                    double amount = getValidDouble();
                    result = deposit(account, amount);
                    printf("%s\n", result);
                    if (strstr(result, "successful") != NULL) {
                        logTransaction(account->accountNumber, "Deposit", originalBalance, account->balance);
                        displayReceipt(account->accountHolder, "Deposit", originalBalance, account->balance);
                    }
                    break;
                }
                case 5:
                    exitChoice = 1;
                    printf("Card ejected. Returning to card selection...\n");
                    break;
                case 6:
                    printf("Exiting program. Please take your card. Thanks for using the ATM!\n");
                    // Save updated accounts before exiting.
                    saveAccountsToCSV("accounts.csv", accounts, accountCount);
                    exit(0);
                default:
                    printf("Invalid option. Try again.\n");
            }
        }
    }
}

