---
# ATM Simulator

A C-based ATM simulator featuring both a command-line and a GTK-based graphical user interface (GUI). This project implements core ATM functionalities such as card selection, PIN verification, balance inquiry, deposit, withdrawal, and PIN change, along with transaction logging and receipt generation.

## Features

- **Dual Interface:**  
  - **Command-Line:** A straightforward text-based interface.
  - **Graphical UI:** A GUI built with GTK for an enhanced user experience.
  
- **Core Functionalities:**  
  - Pre-initialized cards with unique PINs and balances.
  - Three-attempt PIN verification and automatic card blocking.
  - Deposit and withdrawal operations with validation (e.g., withdrawal multiples).
  - Change PIN functionality with error checking.
  - Transaction logging to a text file (`log.txt`).
  - Optional on-screen receipt printing for each transaction.

- **Testing:**  
  - Unit tests using the C standard library's `assert` are provided to ensure the reliability of the ATM functions.

## Project Structure

- **main.c**  
  Implements the command-line version of the ATM simulator.
  
- **algorithm.c / algorithm.h**  
  Contains the core ATM functions (e.g., `checkPin()`, `withdraw()`, `deposit()`, `changePin()`, etc.) and data structures.
  
- **gui.c**  
  Implements the GTK-based GUI for the ATM simulator, handling user interactions and screen navigation.
  
- **test.c**  
  Provides unit tests for the ATM functions to support reliable, error-free operation.

## Text-Based Menu

The command-line version of the ATM operates through a structured text-based menu system, allowing users to interact with the ATM using numerical selections. The flow is as follows:

1. **Card Selection:** The user selects a card from a predefined list.
2. **PIN Verification:** The user enters a PIN, with a maximum of three attempts before the card is blocked.
3. **Main Menu:** Once authenticated, the user is presented with the following options:
   - **1. Check Balance** – Displays the current account balance.
   - **2. Deposit** – Allows the user to enter an amount to deposit.
   - **3. Withdraw** – Enables withdrawals with validation (e.g., ensuring the amount is a multiple of a specified value).
   - **4. Change PIN** – The user can change their PIN, with verification of the old PIN before setting a new one.
   - **5. Eject Card** - The user can eject the card and return to card selection, while the data of his account is updated.
   - **6. Exit** – Logs the user out and terminates the session.
4. **Transaction Logging:** Every transaction is recorded in `log.txt`, providing a history of deposits, withdrawals, and PIN changes.
5. **Session Termination:** The user can exit at any time, ensuring data integrity and security.

This menu-driven approach provides a simple yet effective way to interact with the ATM simulator without requiring a graphical interface.

### **GUI Features**
- **Card Selection Screen:** Users can select a card from a dropdown list.
- **PIN Entry:** A secure PIN entry screen with masked input and error tracking.
- **Main Menu:** A structured interface with buttons for each ATM function.
- **Transaction Screens:**
  - **Balance Inquiry:** Displays the current account balance in a message box.
  - **Deposit & Withdrawal:** Interactive input fields with validation.
  - **PIN Change:** A step-by-step interface for changing the PIN.
- **Receipt Generation:** An on-screen receipt summarizing each transaction.
- **Logging:** All actions are recorded in `log.txt` for tracking.
- **Session Management:** Users can eject the card or exit at any time.

### **GUI Demonstration**
Click the video below to see the **ATM Simulator in action**:

📺 [GUI_Explanation.mp4](GUI_Explanation.mp4)

## Requirements

- **Compiler:** GCC (or any standard C compiler)
- **Libraries:**  
  - GTK 4 development libraries (for the GUI)
  - Standard C library

## Testing

Both unit and integration testing are integrated to ensure the functionality of all core features. Run the unit tests provided in `test.c` to verify that each ATM function behaves as expected.

## License

This project is licensed under the GNU GENERAL PUBLIC LICENSE License.

## Author

Kirill Tumoian

---

