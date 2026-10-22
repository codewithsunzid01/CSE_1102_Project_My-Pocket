#include "budget.h"
#include <math.h>

static void currentMonth(char *month)
{
    char date[MAX_DATE_LEN];
    getCurrentDate(date);
    (void)snprintf(month, MAX_MONTH_LEN, "%.7s", date);
}

static int findCurrentBudget(int userId)
{
    char month[MAX_MONTH_LEN];
    int i;
    currentMonth(month);
    for (i = 0; i < budgetCount; ++i) {
        if (budgets[i].userId == userId && strcmp(budgets[i].month, month) == 0) return i;
    }
    return -1;
}

void addIncome(int userId, double amount)
{
    int index = findUserById(userId);
    char date[MAX_DATE_LEN];
    if (index < 0) { puts("User not found."); return; }
    if (!isfinite(amount) || amount <= 0.0) { puts("Income must be a positive amount."); return; }
    if (transactionCount >= MAX_TRANSACTIONS) { puts("Transaction storage is full."); return; }

    getCurrentDate(date);
    users[index].balance += amount;
    addTransaction(userId, INCOME, OTHERS, amount, date, "Monthly/Additional income");
    printf("Income of %.2f BDT added successfully. Current balance: %.2f BDT\n", amount, users[index].balance);
}

void manageIncome(int userId)
{
    int choice, index = findUserById(userId);
    if (index < 0) { puts("User not found."); return; }
    for (;;) {
        printf("\n========== INCOME MANAGEMENT ==========\n");
        printf("This month's recorded income: %.2f BDT\n", calculateTotalIncome(userId));
        printf("Current balance: %.2f BDT\n", users[index].balance);
        puts("1. Add income\n2. View financial summary\n3. Back");
        choice = readInt("Choose: ", 1, 3);
        if (choice == 1) {
            double amount = readDouble("Enter income amount (BDT): ", 0.01, 1000000000000.0);
            addIncome(userId, amount);
        } else if (choice == 2) {
            showFinancialSummary(userId);
        } else {
            return;
        }
    }
}

void setBudget(int userId, double amount)
{
    int index = findUserById(userId);
    int budgetIndex;
    char month[MAX_MONTH_LEN];
    if (index < 0) { puts("User not found."); return; }
    if (!isfinite(amount) || amount <= 0.0) { puts("Budget must be a positive amount."); return; }

    currentMonth(month);
    budgetIndex = findCurrentBudget(userId);
    if (budgetIndex < 0) {
        if (budgetCount >= MAX_BUDGETS) { puts("Budget storage is full."); return; }
        budgetIndex = budgetCount++;
        memset(&budgets[budgetIndex], 0, sizeof(budgets[budgetIndex]));
        budgets[budgetIndex].budgetId = 1;
        for (int i = 0; i < budgetIndex; ++i) {
            if (budgets[i].budgetId >= budgets[budgetIndex].budgetId) budgets[budgetIndex].budgetId = budgets[i].budgetId + 1;
        }
        budgets[budgetIndex].userId = userId;
        (void)snprintf(budgets[budgetIndex].month, sizeof(budgets[budgetIndex].month), "%s", month);
    }
    budgets[budgetIndex].amount = amount;
    budgets[budgetIndex].spent = calculateTotalExpense(userId);
    budgets[budgetIndex].remaining = amount - budgets[budgetIndex].spent;
    users[index].monthly_budget = amount;
    saveAllData();
    printf("Budget for %s set to %.2f BDT.\n", month, amount);
    checkBudgetAlert(userId);
}

double calculateRemainingBudget(int userId)
{
    int index = findCurrentBudget(userId);
    if (index < 0) return 0.0;
    budgets[index].spent = calculateTotalExpense(userId);
    budgets[index].remaining = budgets[index].amount - budgets[index].spent;
    return budgets[index].remaining;
}

void manageBudget(int userId)
{
    int choice, index = findUserById(userId), bindex;
    if (index < 0) { puts("User not found."); return; }
    for (;;) {
        puts("\n========== BUDGET MANAGEMENT ==========");
        puts("1. Set/update monthly budget\n2. View budget status\n3. Check budget alert\n4. Back");
        choice = readInt("Choose: ", 1, 4);
        if (choice == 1) {
            double amount = readDouble("Enter your total budget for this month (BDT): ", 0.01, 1000000000000.0);
            setBudget(userId, amount);
        } else if (choice == 2) {
            bindex = findCurrentBudget(userId);
            if (bindex < 0) {
                puts("No monthly budget set yet. Choose option 1 to set one.");
            } else {
                budgets[bindex].spent = calculateTotalExpense(userId);
                budgets[bindex].remaining = budgets[bindex].amount - budgets[bindex].spent;
                printf("Month: %s\nBudget: %.2f BDT\nSpent: %.2f BDT\nRemaining: %.2f BDT\n",
                       budgets[bindex].month, budgets[bindex].amount, budgets[bindex].spent, budgets[bindex].remaining);
                if (budgets[bindex].remaining < 0.0) printf("Overspent by %.2f BDT.\n", -budgets[bindex].remaining);
                saveAllData();
            }
        } else if (choice == 3) {
            checkBudgetAlert(userId);
        } else {
            return;
        }
    }
}
