#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algorithm.h"


int main() {
    int accountCount;
    // Load accounts once at the beginning
    struct BankAccount *accounts = loadAccountsFromCSV("accounts.csv", &accountCount);
    if (accountCount == 0) {
        printf("No accounts loaded. Exiting.\n");
        return 1;
    }
    while (true) {
        int selectedCard;
        struct BankAccount *account = NULL;
        int pin, pinAttempts;
        int choice;
        double amount;

        // Card selection
        printf("\nWelcome to the ATM Machine created by Kirill!\n"
               "Select a card (e.g., 1 for Card 1, 2 for Card 2). Enter 0 to Quit the Program:\n>>> ");
        scanf("%d", &selectedCard);
        if (selectedCard == 0) {
            printf("Exiting program. Thanks for using the ATM!.\n");
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
            scanf("%d", &pin);
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
            scanf("%d", &choice);

            double originalBalance = account->balance;
            const char *result;
            switch (choice) {
                case 1: {
                    int newPin1, newPin2;
                    printf("Enter new PIN:\n>>> ");
                    scanf("%d", &newPin1);
                    printf("Re-enter new PIN:\n>>> ");
                    scanf("%d", &newPin2);
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
                    scanf("%lf", &amount);
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
                    scanf("%lf", &amount);
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
