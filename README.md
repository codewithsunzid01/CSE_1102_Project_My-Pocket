
# MyPocket — Student Budget & Financial Planning System

MyPocket is a console-based C11 lab project for students to manage income, expenses, monthly budgets, savings, reminders, recurring costs, financial reports, and shared group expenses.

## Requirements

- GCC (or another C11-compatible compiler)
- GNU Make (`make` or `mingw32-make` with MinGW on Windows)

## Build and run

From the `MyPocket` project directory:

```sh
make build
./mypocket
```

On Windows PowerShell:

```powershell
mingw32-make build
.\mypocket.exe
```

Useful commands:

```sh
make clean
make test
./mypocket --help
./mypocket --version
./mypocket --stats
```

`make test` builds and runs the core regression tests inside an isolated sandbox under `build/`, so it does not overwrite your actual `data/` files.

## Features

- **Accounts:** registration, login, password-strength checks, and profile editing.
- **Income:** record additional/monthly income; view, edit, and delete financial records.
- **Budget:** create/update the current month's budget, see spending and remaining budget, receive alerts, and delete the current month's budget. A budget must be set only after income is recorded and cannot exceed the current month's recorded income.
- **Expenses:** add categorized expenses, view transaction history, filter by category, search by keywords, and edit/delete ordinary financial records.
- **Savings:** record deposits and withdrawals, view actual savings balance and goal progress, update/clear the goal, and edit/delete savings transactions. Deposits cannot exceed available balance and withdrawals cannot exceed saved funds.
- **Reminders/calendar:** add, view, complete, edit, and delete reminders; inspect overdue and upcoming reminders.
- **Recurring expenses:** add, view, edit, delete, and process recurring expenses; estimate upcoming costs.
- **Financial reports:** current-month summary, category-wise expenses, month-to-month comparison, projected balance, and CSV export for Excel/spreadsheets.
- **Group events:** create/edit/delete empty events, add/remove participants, record expenses with equal/custom shares, see who owes whom, and edit/delete event expenses and settlement transfers. Linked records are guarded to reduce accidental balance corruption.
- **Local persistence:** records are saved under `data/` in `.txt` files and reloaded at startup.

## Data storage and privacy

The program stores `users.txt`, `transactions.txt`, `budgets.txt`, `events.txt`, `fundings.txt`, `reminders.txt`, and `recurring.txt` under `data/`. These use the versioned `MYPocket-TEXT-HEX-v1` encoding: files are text, but the record lines are machine-oriented hexadecimal rather than field-by-field readable text. Do not edit these files manually.

No `.gitignore` file is included, as requested. **Do not stage or commit `data/` files to GitHub** because they may contain personal financial records. This educational project stores passwords in its local encoded records without cryptographic hashing; do not use real or reused passwords or sensitive personal data.

The previous `.dat` binary files are not automatically imported. Back up old data before switching storage formats.

## Important behavior

- A group-event expense is recorded as an expense transaction for the participant who paid the total amount; the event stores each participant's share separately.
- Settlement transfers update both users' balances and are represented by linked internal transactions. Transfer amounts are checked against the current settlement owed.
- Editing/deleting an event expense or transfer is restricted when linked settlement data could become inconsistent; correct/delete the dependent transfer first.
- Deleting a recurring schedule does not delete historical expenses that were already processed.
- Reminders in this console application do not send operating-system notifications while the app is closed.
- This is an educational console application, not a production financial product.

## Structured Programming Lab topic coverage

1. Variables and data types — IDs, choices, dates, amounts, counters, and flags.
2. Conditional statements — authentication, validation, budget checks, and settlement checks.
3. `switch` statements — menu navigation and transaction-type handling.
4. Loops — repeated menus and record processing.
5. Iteration — transaction/category totals, event participants, and file records.
6. Arrays — bounded user, transaction, budget, event, reminder, and share collections.
7. Strings — names, emails, descriptions, categories, and dates.
8. String tokenization — keyword-based transaction search.
9. Structures and enums — user, transaction, budget, event, funding, reminder, and recurring records.
10. Pointers — record/buffer handling and dynamic input.
11. Functions — modular feature implementation across multiple `.c` files.
12. Recursion — recursive monthly transaction summation.
13. File handling — local text-file persistence.
14. Date processing — validation, formatting, comparisons, reminders, and recurring expenses.
15. Header file — `includefile/budget.h`.
16. Multiple source files — modules under `src/`.
17. Makefile — build and test automation.

Additional concepts: `argc/argv`, `malloc/realloc/free`, unions, and bitwise status flags.

## Project layout

```text
MyPocket/
  Makefile
  README.md
  TESTING_NOTES.md
  includefile/budget.h
  src/*.c
  tests/test_core.c
  data/                 # created/used locally at runtime
```

See `TESTING_NOTES.md` for the checks actually performed and remaining environment limitations.
