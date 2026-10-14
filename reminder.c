#include <stdio.h>
#include "budget.h"

void addReminder(int userId)
{
    printf("Add Reminder for user %d\n", userId);
}

void viewReminders(int userId)
{
    printf("View Reminders for user %d\n", userId);
}

void markReminderCompleted(int userId)
{
    printf("Mark Reminder Completed for user %d\n", userId);
}

void checkUpcomingReminders(int userId)
{
    printf("Check Upcoming Reminders for user %d\n", userId);
}

void reminder_dashboard(int userId)
{
    int choice;

    while (1)
    {
        printf("\n===== REMINDER DASHBOARD =====\n");
        printf("1. Add Reminder\n");
        printf("2. View Reminders\n");
        printf("3. Mark Reminder Completed\n");
        printf("4. Check Upcoming Reminders\n");
        printf("5. Back\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                addReminder(userId);
                break;
            case 2:
                viewReminders(userId);
                break;
            case 3:
                markReminderCompleted(userId);
                break;
            case 4:
                checkUpcomingReminders(userId);
                break;
            case 5:
                return;
            default:
                printf("Invalid choice.\n");
        }
    }
}