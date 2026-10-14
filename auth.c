#include <stdio.h>
#include <string.h>
#include <ctype.h> 
#include "budget.h"

/* Find user by email, return index if found, otherwise -1 */
int findUserByEmail(const char *email)
 {
    int i;
    for (i = 0; i < userCount; i++) {
        if (strcmp(users[i].email, email) == 0) {
            return i;
        }
    }
    return -1;
}

/* Find user by ID, return index if found, otherwise -1 */
int findUserById(int userId) {
    int i;
    for (i = 0; i < userCount; i++) {
        if (users[i].userId == userId) {
            return i;
        }
    }
    return -1;
}

/* Check email + password, return userId if valid, otherwise -1 */
int authenticateUser(const char *email, const char *password) {
    int index = findUserByEmail(email);

    if (index == -1) {
        return -1;
    }

    if (strcmp(users[index].password, password) == 0) {
        return users[index].userId;
    }

    return -1;
}

/* Check password strength */
int isStrongPassword(const char *password) {
    int hasUpper = 0, hasLower = 0, hasDigit = 0, hasSpecial = 0;
    int length = strlen(password);
    int i;

    if (length < 6) {
        return 0;
    }

    for (i = 0; password[i] != '\0'; i++) {
        if (isupper((unsigned char)password[i])) {
            hasUpper = 1;
        } else if (islower((unsigned char)password[i])) {
            hasLower = 1;
        } else if (isdigit((unsigned char)password[i])) {
            hasDigit = 1;
        } else {
            hasSpecial = 1;
        }
    }

    return hasUpper && hasLower && hasDigit && hasSpecial;
}

/* Register a new user */
     int registerUser(void ) {
    char name[MAX_NAME_LEN];
    char email[MAX_EMAIL_LEN];
    char password[MAX_PASSWORD_LEN];

    if (userCount >= MAX_USERS) {
        printf("\nUser limit reached. Cannot register more users.\n");
        return -1;
    }

    printf("\n========== USER REGISTRATION ==========\n");

    printf("Enter your name: ");
    scanf(" %[^\n]", name);

    printf("Enter your email: ");
    scanf("%s", email);

    if (findUserByEmail(email) != -1) {
        printf("This email is already registered.\n");
        return -1;
    }

    while (1) {
        printf("Create password: ");
        scanf("%s", password);

        if (strlen(password) < 6) {
            printf("Warning: Password must be at least 6 characters long.\n");
            continue;
        }

        if (!isStrongPassword(password)) {
            printf("Weak password!\n");
            printf("Use at least one uppercase letter, one lowercase letter, one digit, and one special character.\n");
            continue;
        }

        printf("Strong password accepted.\n");
        break;
    }

    users[userCount].userId = userCount + 1;
    strcpy(users[userCount].name, name);
    strcpy(users[userCount].email, email);
    strcpy(users[userCount].password, password);

    /* Initialize finance fields */
    users[userCount].monthly_income = 0.0;
    users[userCount].monthly_budget = 0.0;
    users[userCount].savings_goal = 0.0;
    users[userCount].total_expense = 0.0;
    users[userCount].balance = 0.0;

    userCount++;

    printf("Registration successful!\n");
    printf("Your User ID is: %d\n", users[userCount - 1].userId);
}

/* Login user and return userId if successful, otherwise -1 */
int loginUser(void) {
    char email[MAX_EMAIL_LEN];
    char password[MAX_PASSWORD_LEN];
    int userId;

    printf("\n============= USER LOGIN =============\n");

    printf("Enter your email: ");
    scanf("%s", email);

    printf("Enter your password: ");
    scanf("%s", password);

    userId = authenticateUser(email, password);

    if (userId == -1) {
        printf("Invalid email or password.\n");
        return -1;
    }

    printf("Login successful!\n");
    return userId;
}