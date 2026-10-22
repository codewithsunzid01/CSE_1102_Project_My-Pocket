#include "budget.h"
#include <math.h>

static int nextTransactionId(void)
{
    int maxId = 0;
    for (int i = 0; i < transactionCount; ++i) if (transactions[i].transactionId > maxId) maxId = transactions[i].transactionId;
    return maxId + 1;
}

void addTransaction(int userId, TransactionType type, CategoryType category, double amount, const char *date, const char *description)
{
    Transaction *transaction;
    int userIndex = findUserById(userId);
    char currentDate[MAX_DATE_LEN];
    char month[MAX_MONTH_LEN];

    if (userIndex < 0) { puts("Cannot add transaction: user not found."); return; }
    if (!isfinite(amount) || amount <= 0.0) { puts("Cannot add a non-positive transaction."); return; }
    if (transactionCount >= MAX_TRANSACTIONS) { puts("Transaction storage is full; no transaction was recorded."); return; }

    transaction = &transactions[transactionCount];
    memset(transaction, 0, sizeof(*transaction));
    transaction->transactionId = nextTransactionId();
    transaction->userId = userId;
    transaction->type = type;
    transaction->category = category;
    transaction->amount = amount;
    getCurrentDate(currentDate);
    if (date != NULL && isValidDate(date)) (void)snprintf(transaction->date, sizeof(transaction->date), "%s", date);
    else (void)snprintf(transaction->date, sizeof(transaction->date), "%s", currentDate);
    if (description != NULL) (void)snprintf(transaction->description, sizeof(transaction->description), "%s", description);
    /* Store the type-specific note in the corresponding union member. */
    if (type == INCOME) {
        (void)snprintf(transaction->details.incomeSource, sizeof(transaction->details.incomeSource), "%s",
                       description != NULL ? description : "Income");
    } else if (type == EXPENSE) {
        (void)snprintf(transaction->details.expenseNote, sizeof(transaction->details.expenseNote), "%s",
                       description != NULL ? description : "Expense");
    } else {
        (void)snprintf(transaction->details.internalNote, sizeof(transaction->details.internalNote), "%s",
                       description != NULL ? description : "Internal transfer");
    }
    ++transactionCount;

    users[userIndex].monthly_income = calculateTotalIncome(userId);
    users[userIndex].total_expense = calculateTotalExpense(userId);
    (void)snprintf(month, sizeof(month), "%.7s", currentDate);
    for (int i = 0; i < budgetCount; ++i) {
        if (budgets[i].userId == userId && strcmp(budgets[i].month, month) == 0) {
            budgets[i].spent = users[userIndex].total_expense;
            budgets[i].remaining = budgets[i].amount - budgets[i].spent;
        }
    }
    saveAllData();
}

void addExpense(int userId)
{
    int index = findUserById(userId);
    CategoryType category;
    double amount;
    char *description;
    char date[MAX_DATE_LEN];
    if (index < 0) { puts("User not found."); return; }
    if (transactionCount >= MAX_TRANSACTIONS) { puts("Transaction storage is full."); return; }

    puts("\n========== ADD EXPENSE ==========");
    category = promptCategory();
    amount = readDouble("Enter expense amount (BDT): ", 0.01, 1000000000000.0);
    description = readLineDynamic("Description (e.g. lunch, bus fare, books): ");
    if (description == NULL || description[0] == '\0') {
        free(description);
        puts("Description is required; expense cancelled.");
        return;
    }
    getCurrentDate(date);
    users[index].balance -= amount;
    addTransaction(userId, EXPENSE, category, amount, date, description);
    printf("Expense saved. Balance is now %.2f BDT.\n", users[index].balance);
    free(description);
    checkBudgetAlert(userId);
}


void editExpense(int userId)
{
    int selectedId, transactionIndex = -1, userIndex = findUserById(userId);
    double oldAmount, newAmount;
    CategoryType newCategory;
    char *newDateInput = NULL, *newDescription = NULL;
    char currentDate[MAX_DATE_LEN], currentMonth[MAX_MONTH_LEN];

    if (userIndex < 0) { puts("User not found."); return; }

    puts("\n========== EDIT EXPENSE ==========");
    puts("Your recorded expenses:");
    printf("%-5s %-12s %-14s %-12s %s\n", "ID", "Date", "Category", "Amount", "Description");
    for (int i = 0; i < transactionCount; ++i) {
        const Transaction *t = &transactions[i];
        if (t->userId == userId && t->type == EXPENSE) {
            printf("%-5d %-12s %-14s %10.2f  %s\n", t->transactionId, t->date,
                   categoryToString(t->category), t->amount, t->description);
        }
    }

    selectedId = readInt("Enter the ID of the expense to edit (0 to cancel): ", 0, 2147483647);
    if (selectedId == 0) { puts("Edit cancelled."); return; }

    for (int i = 0; i < transactionCount; ++i) {
        if (transactions[i].transactionId == selectedId &&
            transactions[i].userId == userId && transactions[i].type == EXPENSE) {
            transactionIndex = i;
            break;
        }
    }
    if (transactionIndex < 0) {
        puts("Expense ID not found in your account. No changes made.");
        return;
    }

    printf("Current amount: %.2f BDT\n", transactions[transactionIndex].amount);
    oldAmount = transactions[transactionIndex].amount;
    newAmount = readDouble("Enter corrected amount (BDT): ", 0.01, 1000000000000.0);
    if (!isfinite(newAmount)) { puts("Invalid amount. No changes made."); return; }
    newCategory = promptCategory();

    printf("Current date: %s (press Enter to keep it)\n", transactions[transactionIndex].date);
    newDateInput = readLineDynamic("Enter new date (YYYY-MM-DD) or press Enter to keep: ");
    if (newDateInput == NULL) return;
    if (newDateInput[0] != '\0' && !isValidDate(newDateInput)) {
        puts("Invalid date. No changes made.");
        free(newDateInput);
        return;
    }

    printf("Current description: %s\n", transactions[transactionIndex].description);
    newDescription = readLineDynamic("Enter new description or press Enter to keep: ");
    if (newDescription == NULL) { free(newDateInput); return; }
    /* Correct the live balance by reversing the old expense and applying the new one. */
    users[userIndex].balance += oldAmount - newAmount;
    transactions[transactionIndex].amount = newAmount;
    transactions[transactionIndex].category = newCategory;
    if (newDateInput[0] != '\0') {
        (void)snprintf(transactions[transactionIndex].date,
                       sizeof(transactions[transactionIndex].date), "%s", newDateInput);
    }
    if (newDescription[0] != '\0') {
        (void)snprintf(transactions[transactionIndex].description,
                       sizeof(transactions[transactionIndex].description), "%s", newDescription);
    }
    {
        char noteCopy[MAX_DESC_LEN];
        (void)snprintf(noteCopy, sizeof(noteCopy), "%s", transactions[transactionIndex].description);
        memcpy(transactions[transactionIndex].details.expenseNote, noteCopy, sizeof(noteCopy));
    }

    getCurrentDate(currentDate);
    (void)snprintf(currentMonth, sizeof(currentMonth), "%.7s", currentDate);
    users[userIndex].monthly_income = calculateTotalIncome(userId);
    users[userIndex].total_expense = calculateTotalExpense(userId);
    for (int i = 0; i < budgetCount; ++i) {
        if (budgets[i].userId == userId && strcmp(budgets[i].month, currentMonth) == 0) {
            budgets[i].spent = users[userIndex].total_expense;
            budgets[i].remaining = budgets[i].amount - budgets[i].spent;
        }
    }
    saveAllData();
    printf("Expense #%d updated successfully. New amount: %.2f BDT. Current balance: %.2f BDT.\n",
           selectedId, newAmount, users[userIndex].balance);
    free(newDateInput);
    free(newDescription);
    checkBudgetAlert(userId);
}

void viewTransactions(int userId)
{
    int found = 0;
    printf("\n========== TRANSACTION HISTORY ==========");
    printf("\n%-5s %-12s %-20s %-14s %-12s %s\n", "ID", "Date", "Type", "Category", "Amount", "Description");
    for (int i = 0; i < transactionCount; ++i) {
        const Transaction *t = &transactions[i];
        if (t->userId != userId) continue;
        printf("%-5d %-12s %-20s %-14s %10.2f  %s\n", t->transactionId, t->date,
               transactionTypeToString(t->type), categoryToString(t->category), t->amount, t->description);
        found = 1;
    }
    if (!found) puts("\nNo transactions recorded for this account yet.");
}

void viewTransactionsByCategory(int userId, CategoryType category)
{
    int found = 0;
    printf("\nExpenses in category: %s\n", categoryToString(category));
    for (int i = 0; i < transactionCount; ++i) {
        const Transaction *t = &transactions[i];
        if (t->userId == userId && t->type == EXPENSE && t->category == category) {
            printf("#%d | %s | %.2f BDT | %s\n", t->transactionId, t->date, t->amount, t->description);
            found = 1;
        }
    }
    if (!found) puts("No expense found in this category.");
}
