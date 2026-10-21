# MyPocket — Student Budget Management System

A console-based C11 project for managing student income, budgets, expenses, reminders, recurring expenses, financial summaries, search, and group expense sharing.

## Requirements

- GCC (or another C11-compatible compiler)
- GNU Make (make or mingw32-make on some Windows installations)

## Build and run

From the MyPocket project directory:

sh
make


The default target compiles the application and starts the interactive menu. To compile without starting it:

sh
make build


Other commands:

sh
make clean
./mypocket --help
./mypocket --version
./mypocket --stats


On Windows, the executable is `mypocket.exe`; use `mingw32-make` if that is the Make executable installed with MinGW. Depending on the shell, launch the executable as `mypocket.exe --help`.

## Features

- Multi-user registration and login using email
- Password-strength checks
- Monthly/additional income recording
- Monthly budget creation, update, remaining amount, and alerts
- Expense entry with category and description
- Transaction history, category filtering, and token-based search
- Reminder creation, completion, overdue/upcoming reminder checks
- Recurring expenses and upcoming expense projection
- Financial summary, sector-wise expense report, and month-to-month review
- Financial planning and projected balance
- Group events, participants, expense splitting, and event settlement transfers
- File persistence under `data/` across program restarts
- Dynamic input allocation, validation, structures, unions, enums, recursion, bitwise status flags, multiple source files, and a Makefile

## Data storage

The program stores local text files in `data/` (`users.txt`, `transactions.txt`, `budgets.txt`, `events.txt`, `fundings.txt`, `reminders.txt`, and `recurring.txt`). These files use a versioned hexadecimal text encoding so nested structures and unions can be restored exactly; they are text files, but their record lines are machine-oriented rather than directly readable field-by-field. Keep the `data/` folder on your computer to retain data. The program does not create a `.gitignore` file; do not add the personal `data/` files to Git commits. Passwords are stored as plain text inside the encoded records for this educational demo; do not use real or reused passwords.

## Notes

- Event participants must already have registered accounts; add them by user ID.
- A recorded group expense is recorded as an expense transaction for the user who paid the full amount. Shares are saved in the event for settlement calculations.
- This is an educational console project, not a production-grade financial application.

### Editing an expense

Open **Add expense / view transactions** from the dashboard, then choose **Edit an expense**. Select the expense ID from your own transaction list and enter the corrected amount, category, date, and description. Press Enter at the date or description prompt to keep the existing value. The balance and current-month budget totals are recalculated and saved.


## Implementation notes
- Transactions retain a general description and also use a `TransactionDetails` union member selected by transaction type (income, expense, or internal transfer), to demonstrate transaction-specific union data.
- Group-expense and fund-transfer actions check transaction capacity before changing account balances, preventing balance changes when the corresponding records cannot be stored.
- Passwords are stored in plain text in this educational lab project; do not use real passwords or sensitive personal data.

## Structured Programming Lab Topic Coverage

The proposal's 17 topics are represented in the code as follows:

1. **Variables and data types** — user IDs, choices, dates, amounts, counters, and flags.
2. **Conditional statements (`if/else`)** — authentication, input validation, budgets, and event/fund checks.
3. **`switch` statement** — main dashboard navigation and transaction-type conversion.
4. **Loops** — repeated menus and record processing.
5. **Iteration** — transaction totals, category totals, event participants, and file records.
6. **Arrays** — bounded collections of users, transactions, budgets, events, reminders, and shares.
7. **Strings** — names, email addresses, descriptions, categories, and ISO-formatted dates.
8. **String tokenization** — `strtok`-based keyword search in `src/search.c`.
9. **Structures and `enum`** — user, transaction, budget, event, funding, reminder, and recurring-expense records.
10. **Pointers** — passing records and buffers to functions; dynamic input buffers.
11. **Functions** — modules split across authentication, budget, transactions, events, reminders, search, and summaries.
12. **Recursion** — recursive monthly transaction summation in `src/summary.c`.
13. **File handling** — text-file save/load under `data/` using a versioned hex encoding.
14. **Date processing** — date validation, comparison, day arithmetic, reminders, and recurring expenses.
15. **Header file** — `includefile/budget.h`.
16. **Multiple `.c` files** — implementation split across `src/` modules.
17. **Makefile** — compiles all source modules with C11 warnings enabled.

Additional demonstration: the `TransactionDetails` union stores the type-specific note for income, expense, or internal-transfer transactions. `argc/argv`, dynamic allocation (`malloc/realloc/free`), and bitwise status flags are also included.

## Verification status

See `TESTING_NOTES.md` for the features exercised and the environments not available for testing. The source has been built with GCC and strict warning flags. A native Windows/MinGW build must still be run on a Windows setup before submission; it has not been represented as verified here.

### Keeping data only on your computer

- MyPocket creates `data/` automatically when it saves data.
- User records are stored locally in `.txt` files under `data/`.
- No `.gitignore` file is included, as requested. If you use Git, do not stage or commit the `data/` folder; keep it local on your computer.
- The `.txt` files use a versioned hexadecimal text encoding to preserve all C structure fields, including nested event data. They can be opened in Notepad, but record lines are not intended to be edited manually.
- The previous `.dat` binary files are not automatically imported. Start with fresh `.txt` storage or keep a backup of any old `.dat` files before switching.
