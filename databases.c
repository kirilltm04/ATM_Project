#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

// Function to create the database, create the Accounts table, and insert two accounts.
void createDatabaseAndInsertAccounts() {
    sqlite3 *db;
    char *errMsg = 0;
    int rc = sqlite3_open("accounts.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    // SQL command to create the Accounts table if it doesn't already exist.
    const char *sqlCreate = "CREATE TABLE IF NOT EXISTS Accounts ("
                            "accountNumber INTEGER PRIMARY KEY, "
                            "accountHolder TEXT, "
                            "balance REAL, "
                            "pinCode INTEGER, "
                            "blocked INTEGER);";
    rc = sqlite3_exec(db, sqlCreate, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error creating table: %s\n", errMsg);
        sqlite3_free(errMsg);
    }

    // Insert the two accounts. "INSERT OR IGNORE" ensures we don't duplicate entries if they already exist.
    const char *sqlInsert1 = "INSERT OR IGNORE INTO Accounts (accountNumber, accountHolder, balance, pinCode, blocked) "
                             "VALUES (1, 'Kirill Tumoian', 1234.60, 1234, 0);";
    rc = sqlite3_exec(db, sqlInsert1, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error inserting account 1: %s\n", errMsg);
        sqlite3_free(errMsg);
    }

    const char *sqlInsert2 = "INSERT OR IGNORE INTO Accounts (accountNumber, accountHolder, balance, pinCode, blocked) "
                             "VALUES (2, 'Andrew Bradley', 848.50, 5678, 0);";
    rc = sqlite3_exec(db, sqlInsert2, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error inserting account 2: %s\n", errMsg);
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);
}

// Callback function to print each row returned by the SQL query.
int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

// Function to extract and print account data from the database.
void extractData() {
    sqlite3 *db;
    char *errMsg = 0;
    int rc = sqlite3_open("accounts.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    // SQL command to select all data from the Accounts table.
    const char *sqlSelect = "SELECT accountNumber, accountHolder, balance, pinCode, blocked FROM Accounts;";

    rc = sqlite3_exec(db, sqlSelect, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error selecting data: %s\n", errMsg);
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);
}

int try() {
    // Create the database and insert the two accounts.
    createDatabaseAndInsertAccounts();

    // Extract and display the account data.
    extractData();

    return 0;
}
