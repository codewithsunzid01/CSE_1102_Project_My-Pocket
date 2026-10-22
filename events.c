#include "budget.h"
#include <math.h>
#include <limits.h>

static int nextEventId(void)
{
    int maxId = 0;
    for (int i = 0; i < eventCount; ++i)
        if (events[i].eventId > maxId) maxId = events[i].eventId;
    return maxId + 1;
}

static int nextExpenseId(void)
{
    int maxId = 0;
    for (int i = 0; i < eventCount; ++i)
        for (int j = 0; j < events[i].expenseCount; ++j)
            if (events[i].expenses[j].expenseId > maxId) maxId = events[i].expenses[j].expenseId;
    return maxId + 1;
}

int findEventById(int eventId)
{
    for (int i = 0; i < eventCount; ++i)
        if (events[i].eventId == eventId) return i;
    return -1;
}

static int participantIndex(const Event *event, int userId)
{
    for (int i = 0; i < event->participantCount; ++i)
        if (event->participants[i].userId == userId) return i;
    return -1;
}

void createEvent(int userId)
{
    char *name, *date;
    Event *event;
    if (!isValidUser(userId)) { puts("User not found."); return; }
    if (eventCount >= MAX_EVENTS) { puts("Event storage is full."); return; }
    name = readLineDynamic("Event name: ");
    if (name == NULL || name[0] == '\0' || strlen(name) >= MAX_TITLE_LEN) {
        free(name); puts("Event name is required (maximum 99 characters)."); return;
    }
    date = readLineDynamic("Event date (YYYY-MM-DD): ");
    if (date == NULL || !isValidDate(date)) {
        free(name); free(date); puts("Invalid date. Use YYYY-MM-DD."); return;
    }
    event = &events[eventCount++];
    memset(event, 0, sizeof(*event));
    event->eventId = nextEventId();
    event->createdBy = userId;
    (void)snprintf(event->eventName, sizeof(event->eventName), "%s", name);
    (void)snprintf(event->eventDate, sizeof(event->eventDate), "%s", date);
    event->participants[0].userId = userId;
    (void)snprintf(event->participants[0].name, sizeof(event->participants[0].name), "%s", users[findUserById(userId)].name);
    event->participantCount = 1;
    saveAllData();
    printf("Event #%d created. You have been added as the first participant.\n", event->eventId);
    free(name); free(date);
}

void viewEvents(int userId)
{
    int found = 0;
    puts("\n========== GROUP EVENTS ==========");
    for (int i = 0; i < eventCount; ++i) {
        if (participantIndex(&events[i], userId) < 0 && events[i].createdBy != userId) continue;
        printf("ID: %d | %s | Date: %s | Participants: %d | Expenses: %d\n",
               events[i].eventId, events[i].eventName, events[i].eventDate,
               events[i].participantCount, events[i].expenseCount);
        found = 1;
    }
    if (!found) puts("No group events found for your account.");
}

void addParticipantToEvent(int eventId)
{
    int index = findEventById(eventId), userId;
    Event *event;
    if (index < 0) { puts("Event not found."); return; }
    event = &events[index];
    if (event->participantCount >= MAX_PARTICIPANTS) { puts("Participant limit reached."); return; }
    userId = readInt("Enter registered user ID to add: ", 1, INT_MAX);
    if (!isValidUser(userId)) { puts("No registered user has that ID."); return; }
    if (participantIndex(event, userId) >= 0) { puts("That user is already participating."); return; }
    event->participants[event->participantCount].userId = userId;
    (void)snprintf(event->participants[event->participantCount].name,
                   sizeof(event->participants[event->participantCount].name), "%s", users[findUserById(userId)].name);
    ++event->participantCount;
    saveAllData();
    puts("Participant added successfully.");
}

void splitExpenseEqual(Event *event, EventExpense *expense)
{
    if (event == NULL || expense == NULL || event->participantCount <= 0) return;
    expense->splitType = EQUAL_SPLIT;
    expense->participantCount = event->participantCount;
    for (int i = 0; i < event->participantCount; ++i) {
        expense->shares[i].userId = event->participants[i].userId;
        expense->shares[i].shareAmount = expense->totalAmount / event->participantCount;
    }
}

void splitExpenseCustom(Event *event, EventExpense *expense)
{
    double total = 0.0;
    if (event == NULL || expense == NULL) return;
    expense->splitType = CUSTOM_SPLIT;
    expense->participantCount = event->participantCount;
    for (int i = 0; i < event->participantCount; ++i) {
        double share = readDouble("Share amount for this participant (BDT): ", 0.0, expense->totalAmount);
        if (!isfinite(share) || share < 0.0 || share > expense->totalAmount) {
            puts("Invalid or incomplete share input. Expense cancelled.");
            expense->participantCount = 0;
            return;
        }
        expense->shares[i].userId = event->participants[i].userId;
        expense->shares[i].shareAmount = share;
        total += share;
    }
    if (!isfinite(total) || fabs(total - expense->totalAmount) > 0.01) {
        puts("Custom shares must add up to the total expense. Expense cancelled.");
        expense->participantCount = 0;
    }
}

void addEventExpense(int eventId)
{
    int index = findEventById(eventId), payer, split;
    char *title, date[MAX_DATE_LEN];
    double amount;
    Event *event;
    EventExpense *expense;
    if (index < 0) { puts("Event not found."); return; }
    event = &events[index];
    if (event->expenseCount >= MAX_EVENT_EXPENSES) { puts("This event has reached its expense limit."); return; }
    if (transactionCount >= MAX_TRANSACTIONS) { puts("Transaction storage is full; event expense was not recorded."); return; }
    if (event->participantCount <= 0) { puts("Add participants before adding expenses."); return; }
    title = readLineDynamic("Expense description: ");
    if (title == NULL || title[0] == '\0' || strlen(title) >= MAX_TITLE_LEN) {
        free(title); puts("Description is required."); return;
    }
    amount = readDouble("Total expense amount (BDT): ", 0.01, 1000000000000.0);
    puts("Who paid? Enter a participant's user ID:");
    for (int i = 0; i < event->participantCount; ++i)
        printf("%d - %s (ID %d)\n", i + 1, event->participants[i].name, event->participants[i].userId);
    payer = readInt("Payer user ID: ", 1, INT_MAX);
    if (participantIndex(event, payer) < 0) { free(title); puts("Payer must be an event participant."); return; }
    expense = &event->expenses[event->expenseCount];
    memset(expense, 0, sizeof(*expense));
    expense->expenseId = nextExpenseId();
    expense->eventId = eventId;
    expense->paidBy = payer;
    expense->totalAmount = amount;
    (void)snprintf(expense->title, sizeof(expense->title), "%s", title);
    getCurrentDate(date);
    (void)snprintf(expense->date, sizeof(expense->date), "%s", date);
    puts("Split: 1. Equal  2. Custom");
    split = readInt("Choose split type: ", 1, 2);
    if (split == 1) splitExpenseEqual(event, expense);
    else splitExpenseCustom(event, expense);
    if (expense->participantCount == 0) { free(title); return; }
    ++event->expenseCount;
    /* Record the payment as a real expense for the payer, so their balance reflects cash paid. */
    addTransaction(payer, EXPENSE, OTHERS, amount, date, title);
    {
        int payerIndex = findUserById(payer);
        if (payerIndex >= 0) users[payerIndex].balance -= amount;
    }
    saveAllData();
    printf("Group expense recorded. %s shares saved for %d participant(s).\n",
           splitTypeToString(expense->splitType), expense->participantCount);
    free(title);
}

void viewEventDetails(int eventId)
{
    int index = findEventById(eventId);
    Event *event;
    if (index < 0) { puts("Event not found."); return; }
    event = &events[index];
    printf("\nEvent #%d: %s | Date: %s\n", event->eventId, event->eventName, event->eventDate);
    puts("Participants:");
    for (int i = 0; i < event->participantCount; ++i)
        printf("  %s (ID %d)\n", event->participants[i].name, event->participants[i].userId);
    for (int i = 0; i < event->expenseCount; ++i) {
        EventExpense *expense = &event->expenses[i];
        printf("\nExpense #%d: %s | Total %.2f BDT | Paid by user %d | %s\n",
               expense->expenseId, expense->title, expense->totalAmount, expense->paidBy,
               splitTypeToString(expense->splitType));
        for (int j = 0; j < expense->participantCount; ++j)
            printf("  User %d share: %.2f BDT\n", expense->shares[j].userId, expense->shares[j].shareAmount);
    }
    puts("\nSettlement status (positive = owed to this participant; negative = participant owes):");
    for (int i = 0; i < event->participantCount; ++i) {
        int participantId = event->participants[i].userId;
        printf("  %s (ID %d): %+.2f BDT\n", event->participants[i].name,
               participantId, calculateEventNet(eventId, participantId));
    }
}

double calculateEventNet(int eventId, int userId)
{
    int index = findEventById(eventId);
    double net = 0.0;
    if (index < 0) return 0.0;
    for (int i = 0; i < events[index].expenseCount; ++i) {
        EventExpense *expense = &events[index].expenses[i];
        if (expense->paidBy == userId) net += expense->totalAmount;
        for (int j = 0; j < expense->participantCount; ++j)
            if (expense->shares[j].userId == userId) net -= expense->shares[j].shareAmount;
    }
    /* A positive net means the participant is owed money. Settlements reduce
       the payer's debt and the collector's credit without counting as expenses. */
    for (int i = 0; i < fundingCount; ++i) {
        if (fundings[i].eventId != eventId) continue;
        if (fundings[i].fromUserId == userId) net += fundings[i].amount;
        if (fundings[i].toUserId == userId) net -= fundings[i].amount;
    }
    return net;
}

void collectFund(int eventId, int collectorUserId)
{
    int index = findEventById(eventId), payer;
    double amount;
    char date[MAX_DATE_LEN];
    if (index < 0) { puts("Event not found."); return; }
    if (participantIndex(&events[index], collectorUserId) < 0) { puts("You must be a participant to collect funds."); return; }
    payer = readInt("User ID paying the fund: ", 1, INT_MAX);
    if (participantIndex(&events[index], payer) < 0 || payer == collectorUserId) { puts("Choose another participant in this event."); return; }
    amount = readDouble("Amount to transfer (BDT): ", 0.01, 1000000000000.0);
    if (!isfinite(amount) || amount <= 0.0) { puts("Invalid transfer amount; no balances were changed."); return; }
    if (fundingCount >= MAX_FUNDINGS) { puts("Funding storage is full."); return; }
    if (transactionCount > MAX_TRANSACTIONS - 2) { puts("Not enough transaction capacity to record both sides of this transfer."); return; }
    if (findUserById(payer) < 0 || findUserById(collectorUserId) < 0) { puts("Participant account could not be found."); return; }
    if (users[findUserById(payer)].balance < amount) { puts("Payer has insufficient balance."); return; }
    deductFromUser(payer, amount);
    addToUser(collectorUserId, amount);
    getCurrentDate(date);
    fundings[fundingCount].fundingId = fundingCount + 1;
    fundings[fundingCount].eventId = eventId;
    fundings[fundingCount].fromUserId = payer;
    fundings[fundingCount].toUserId = collectorUserId;
    fundings[fundingCount].amount = amount;
    (void)snprintf(fundings[fundingCount].date, sizeof(fundings[fundingCount].date), "%s", date);
    fundings[fundingCount].type = INTERNAL_TRANSACTION;
    (void)snprintf(fundings[fundingCount].description, sizeof(fundings[fundingCount].description), "Event settlement");
    ++fundingCount;
    recordInternalTransaction(payer, collectorUserId, amount, date, "Event settlement");
    saveAllData();
    puts("Fund transfer recorded.");
}

void deductFromUser(int userId, double amount)
{
    int index = findUserById(userId);
    if (index >= 0 && amount > 0.0) users[index].balance -= amount;
}

void addToUser(int userId, double amount)
{
    int index = findUserById(userId);
    if (index >= 0 && amount > 0.0) users[index].balance += amount;
}

void recordInternalTransaction(int fromUserId, int toUserId, double amount, const char *date, const char *description)
{
    addTransaction(fromUserId, INTERNAL_TRANSACTION, OTHERS, amount, date, description);
    addTransaction(toUserId, INTERNAL_TRANSACTION, OTHERS, amount, date, description);
}

void event_dashboard(int userId)
{
    int choice;
    for (;;) {
        puts("\n========== GROUP EVENT & EXPENSE SHARING ==========");
        puts("1. Create event\n2. View my events\n3. Join/add participant to event\n4. Add event expense\n5. View event details\n6. Settle/transfer funds\n7. Back");
        choice = readInt("Choose: ", 1, 7);
        if (choice == 1) createEvent(userId);
        else if (choice == 2) viewEvents(userId);
        else if (choice == 3) addParticipantToEvent(readInt("Event ID: ", 1, INT_MAX));
        else if (choice == 4) addEventExpense(readInt("Event ID: ", 1, INT_MAX));
        else if (choice == 5) viewEventDetails(readInt("Event ID: ", 1, INT_MAX));
        else if (choice == 6) collectFund(readInt("Event ID: ", 1, INT_MAX), userId);
        else return;
    }
}
