#include "budget.h"
#include <limits.h>
#include <math.h>

static int nextRecurringId(void)
{
    int maximum = 0;
    for (int i = 0; i < recurringCount; ++i)
        if (recurringExpenses[i].recurringId > maximum) maximum = recurringExpenses[i].recurringId;
    return maximum + 1;
}

void addRecurringExpense(int userId)
{
    RecurringExpense *r;
    char *title, *date;
    CategoryType category;
    double amount;
    int interval;
    if (!isValidUser(userId)) { puts("User not found."); return; }
    if (recurringCount >= MAX_RECURRING) { puts("Recurring expense storage is full."); return; }
    title = readLineDynamic("Recurring expense title (e.g. rent, internet): ");
    if (title == NULL || title[0] == '\0' || strlen(title) >= MAX_TITLE_LEN) {
        free(title); puts("A non-empty title under 100 characters is required."); return;
    }
    category = promptCategory();
    amount = readDouble("Amount per occurrence (BDT): ", 0.01, 1000000000000.0);
    date = readLineDynamic("Next due date (YYYY-MM-DD): ");
    if (date == NULL || !isValidDate(date)) {
        free(title); free(date); puts("Invalid date. Use YYYY-MM-DD."); return;
    }
    interval = readInt("Repeat interval in days (1-365): ", 1, 365);
    r = &recurringExpenses[recurringCount++];
    memset(r, 0, sizeof(*r));
    r->recurringId = nextRecurringId();
    r->userId = userId;
    r->category = category;
    r->amount = amount;
    r->intervalDays = interval;
    (void)snprintf(r->title, sizeof(r->title), "%s", title);
    (void)snprintf(r->nextDate, sizeof(r->nextDate), "%s", date);
    saveAllData();
    printf("Recurring expense #%d saved. It repeats every %d day(s).\n", r->recurringId, interval);
    free(title); free(date);
}

void viewRecurringExpenses(int userId)
{
    int found = 0;
    puts("\n================ RECURRING EXPENSES ================");
    printf("%-5s %-12s %-12s %-10s %-7s %s\n", "ID", "Next due", "Amount", "Category", "Every", "Title");
    for (int i = 0; i < recurringCount; ++i) {
        const RecurringExpense *r = &recurringExpenses[i];
        if (r->userId != userId) continue;
        printf("%-5d %-12s %10.2f %-10s %-7d %s\n", r->recurringId, r->nextDate,
               r->amount, categoryToString(r->category), r->intervalDays, r->title);
        found = 1;
    }
    if (!found) puts("No recurring expenses have been added.");
}

void processRecurringExpenses(int userId)
{
    char today[MAX_DATE_LEN];
    int processed = 0;
    getCurrentDate(today);
    for (int i = 0; i < recurringCount; ++i) {
        RecurringExpense *r = &recurringExpenses[i];
        if (r->userId != userId || !isValidDate(r->nextDate)) continue;
        while (compareDates(r->nextDate, today) <= 0) {
            int userIndex = findUserById(userId);
            char description[MAX_DESC_LEN];
            if (userIndex < 0 || transactionCount >= MAX_TRANSACTIONS) {
                puts("Cannot process more recurring expenses: user not found or transaction storage is full.");
                saveAllData();
                return;
            }
            (void)snprintf(description, sizeof(description), "Recurring: %.130s", r->title);
            users[userIndex].balance -= r->amount;
            addTransaction(userId, EXPENSE, r->category, r->amount, r->nextDate, description);
            addDaysToDate(r->nextDate, r->intervalDays);
            ++processed;
            /* Defensive guard against invalid interval/date arithmetic. */
            if (!isValidDate(r->nextDate)) break;
        }
    }
    if (processed > 0) {
        saveAllData();
        printf("Processed %d due recurring expense(s).\n", processed);
        checkBudgetAlert(userId);
    }
}

void showUpcomingExpenses(int userId)
{
    int found = 0;
    puts("\n========== UPCOMING EXPENSES (NEXT 30 DAYS) ==========");
    for (int i = 0; i < recurringCount; ++i) {
        const RecurringExpense *r = &recurringExpenses[i];
        int days;
        if (r->userId != userId || !isValidDate(r->nextDate)) continue;
        days = daysUntilDate(r->nextDate);
        if (days >= 0 && days <= 30) {
            printf("%s | %s | %.2f BDT | %s\n", r->nextDate, r->title, r->amount,
                   days == 0 ? "due today" : "scheduled");
            found = 1;
        }
    }
    for (int i = 0; i < reminderCount; ++i) {
        const Reminder *r = &reminders[i];
        int days;
        if (r->userId != userId || r->completed || !isValidDate(r->date)) continue;
        days = daysUntilDate(r->date);
        if (days >= 0 && days <= 30) {
            printf("Reminder: %s | %s | %s\n", r->date, r->title, days == 0 ? "today" : "upcoming");
            found = 1;
        }
    }
    if (!found) puts("No recurring expenses or reminders due in the next 30 days.");
}

double calculateUpcomingExpenseTotal(int userId)
{
    double total = 0.0;
    for (int i = 0; i < recurringCount; ++i) {
        const RecurringExpense *r = &recurringExpenses[i];
        int days;
        if (r->userId != userId || !isValidDate(r->nextDate)) continue;
        days = daysUntilDate(r->nextDate);
        if (days >= 0 && days <= 30) total += r->amount;
    }
    return total;
}
