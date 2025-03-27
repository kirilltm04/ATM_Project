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
