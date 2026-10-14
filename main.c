#include <stdio.h>
#include "budget.h"

void showMainMenu(void)
{
    printf("\n========== MY POCKET ==========\n");
    printf("1. Register\n");
    printf("2. Login\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");
}

void user_menu(int userId)
{
    int choice;

    while (1)
    {
        printf("\n========== DASHBOARD ==========\n");
        printf("Welcome User ID: %d\n", userId);
        printf("========== USER FINANCE MENU ==========\n");
        printf("1. Income\n");
        printf("2. Budget\n");
        printf("3. Transactions\n");
        printf("4. Search & Tokenization\n");
        printf("5. Reminder\n");
        printf("6. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                printf("Income module will be added later.\n");
                break;

            case 2:
                printf("Budget module will be added later.\n");
                break;

            case 3:
                printf("Transaction module will be added later.\n");
                break;

            case 4:
                search_and_tokenization_demo();
                break;

            case 5:
                reminder_dashboard(userId);
                break;

            case 6:
                printf("Logging out...\n");
                return;

            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}

int main(void)
{
    int choice;
    int userId;

    while (1)
    {
        showMainMenu();
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                userId = registerUser();
                if (userId != -1)
                {
                    printf("Registration successful! Redirecting to dashboard...\n");
                    user_menu(userId);
                }
                break;

            case 2:
                userId = loginUser();
                if (userId != -1)
                {
                    printf("Login successful! Redirecting to dashboard...\n");
                    user_menu(userId);
                }
                else
                {
                    printf("Invalid email or password.\n");
                }
                break;

            case 3:
                printf("Exiting program...\n");
                return 0;

            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}
