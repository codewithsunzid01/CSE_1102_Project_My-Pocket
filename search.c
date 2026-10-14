#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "budget.h"

void tokenize_text(const char *text)
{
    char temp[200];
    char *token;

    strcpy(temp, text);

    printf("\nTokens:\n");

    token = strtok(temp, " ,.-\n");
    while (token != NULL)
    {
        printf("%s\n", token);
        token = strtok(NULL, " ,.-\n");
    }
}

int search_keyword(const char *text, const char *keyword)
{
    char temp[200];
    char *token;

    strcpy(temp, text);

    token = strtok(temp, " ,.-\n");
    while (token != NULL)
    {
        if (strcasecmp(token, keyword) == 0)
        {
            return 1;
        }
        token = strtok(NULL, " ,.-\n");
    }

    return 0;
}

void search_and_tokenization_demo(void)
{
    char text[200];
    char keyword[50];

    printf("\n========== SEARCH & TOKENIZATION ==========\n");

    getchar(); /* clear newline */
    printf("Enter a sentence: ");
    fgets(text, sizeof(text), stdin);

    tokenize_text(text);

    printf("\nEnter keyword to search: ");
    scanf("%s", keyword);

    if (search_keyword(text, keyword))
    {
        printf("Keyword '%s' found in text.\n", keyword);
    }
    else
    {
        printf("Keyword '%s' not found in text.\n", keyword);
    }
}