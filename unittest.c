//
// Created by Kirill Tumoian on 27.03.2025.
//
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "algorithm.h"

// Test PIN verification
void test_checkPin() {
    struct BankAccount account = {123, "Test User", 100.0, 1234, false};
    assert(checkPin(&account, 1234) == 1);
    assert(checkPin(&account, 0000) == 0);
}

// Test blocked status check
void test_checkBlocked() {
    struct BankAccount account = {123, "Test User", 100.0, 1234, false};
    assert(checkBlocked(&account) == 0);
    account.blocked = true;
    assert(checkBlocked(&account) == 1);
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

    result = withdraw(&account, 42);
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

// Test balance display
void test_showBalance() {
    struct BankAccount account = {123, "Test User", 100.0, 1234, false};
    const char* result = showBalance(&account);
    // Check that the string contains the correct formatted balance.
    assert(strstr(result, "£100.00") != 0);
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

int main() {
    test_checkPin();
    test_checkBlocked();
    test_withdraw();
    test_deposit();
    test_changePin();
    test_showBalance();
    test_findAccount();

    printf("All unit tests passed successfully! ;)\n");
    return 0;
}
