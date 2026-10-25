#include "budget.h"
#include <math.h>

void checkBudgetAlert(int userId)
{
    int index = findUserById(userId);
    double expense, ratio;
    if (index < 0) { puts("User not found."); return; }
    if (users[index].monthly_budget <= 0.0) {
        puts("Budget alert: you have not set a monthly budget yet.");
        return;
    }
    expense = calculateTotalExpense(userId);
    ratio = expense / users[index].monthly_budget;
    if (ratio >= 1.0) {
        printf("BUDGET ALERT: you have exceeded your budget by %.2f BDT.\n", expense - users[index].monthly_budget);
    } else if (ratio >= 0.8) {
        printf("Budget warning: %.1f%% used; only %.2f BDT remains.\n", ratio * 100.0, users[index].monthly_budget - expense);
    } else {
        printf("Budget status: %.1f%% used; %.2f BDT remains.\n", ratio * 100.0, users[index].monthly_budget - expense);
    }
}

double calculateFutureBalance(int userId)
{
    int index = findUserById(userId);
    if (index < 0) return 0.0;
    return users[index].balance - calculateUpcomingExpenseTotal(userId);
}

void showFinancialPlanning(int userId)
{
    int index = findUserById(userId);
    int choice;
    if (index < 0) { puts("User not found."); return; }
    for (;;) {
        double income = calculateTotalIncome(userId);
        double expense = calculateTotalExpense(userId);
        puts("\n=============== FINANCIAL PLANNING ===============");
        printf("Monthly net amount: %.2f BDT\n", income - expense);
        printf("Available balance: %.2f BDT\n", users[index].balance);
        printf("Upcoming recurring expenses: %.2f BDT\n", calculateUpcomingExpenseTotal(userId));
        printf("Projected balance after listed upcoming expenses: %.2f BDT\n", calculateFutureBalance(userId));
        printf("Monthly savings target: %.2f BDT\n", users[index].savings_goal);
        if (users[index].savings_goal > 0.0) {
            double gap = users[index].savings_goal - (income - expense);
            if (gap <= 0.0) puts("Monthly savings target reached!");
            else printf("Still needed this month: %.2f BDT\n", gap);
        }
        puts("1. View upcoming expenses\n2. Check budget alert\n3. Back");
        choice = readInt("Choose: ", 1, 3);
        if (choice == 1) showUpcomingExpenses(userId);
        else if (choice == 2) checkBudgetAlert(userId);
        else return;
    }
}

