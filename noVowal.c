/******************************************************************************
By: Laith Alaboodi
noVowal.c
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>

typedef struct strNode
{
    char *string;
    struct strNode *next;
} strNode;

typedef struct node
{
    struct strNode *str;
    struct node *next;
} node;

void memoryError()
{
    printf("ERROR: Memory allocation failed\n");
    exit(1);
}

int isNoVowelEqual(char w1[], char w2[])
{
    int w1Pos = 0, w2Pos = 0;
    char w1Char = tolower(w1[w1Pos]);
    char w2Char = tolower(w2[w2Pos]);
    while (w1Char != '\0' || w2Char != '\0')
    {
        if (w1Char == 'a' || w1Char == 'e' || w1Char == 'i' || w1Char == 'o' || w1Char == 'u')
        {
            w1Pos++;
        }
        else if (w2Char == 'a' || w2Char == 'e' || w2Char == 'i' || w2Char == 'o' || w2Char == 'u')
        {
            w2Pos++;
        }
        else
        {
            if (w1Char != '\0')
            {
                w1Pos++;
            }
            if (w2Char != '\0')
            {
                w2Pos++;
            }
            if (w1Char != w2Char)
            {
                return 0;
            }
        }

        w1Char = tolower(w1[w1Pos]);
        w2Char = tolower(w2[w2Pos]);
    }
    return 1;
}

int legal(char word[])
{
    int i = 0;
    char c;
    while ((c = word[i++]) != '\0')
    {
        if (!isalpha(c))
        {
            return 0;
        }
    }
    return 1;
}

strNode *nextStrNode(char word[])
{
    strNode *newNode = malloc(sizeof(strNode));
    if (newNode == NULL)
    {
        memoryError();
    }
    else
    {
        newNode->string = strdup(word);
        newNode->next = NULL;
    }
    return newNode;
}

node *nextNode(char word[])
{
    node *newNode = malloc(sizeof(node));
    if (newNode == NULL)
    {
        memoryError();
    }
    else
    {
        newNode->next = NULL;
        newNode->str = nextStrNode(word);
    }
    return newNode;
}

void print(node *head)
{
    node *tmp = head;
    while (tmp != NULL)
    {
        strNode *strTmp = tmp->str;
        while (strTmp != NULL)
        {

            printf("%s ", strTmp->string);
            strTmp = strTmp->next;
        }
        printf("\n");
        tmp = tmp->next;
    }
}

void addNode(strNode *sNode, char word[]) {
    strNode *tmp = sNode;
    while (tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    tmp->next = nextStrNode(word);
}


void add(char word[], node *head)
{
    node *tmp = head;
    while (tmp->next != NULL)
    {
        if (isNoVowelEqual(word, tmp->str->string))
        {
            addNode(tmp->str, word);
            return;
        }
        tmp = tmp->next;
    }
    if (isNoVowelEqual(word, tmp->str->string))
    {
        addNode(tmp->str, word);
        return;
    }
    else
    {
        node *newNode = malloc(sizeof(node));
        if (newNode == NULL)
        {
            memoryError();
        }
        else
        {
            newNode->str = nextStrNode(word);
            newNode->next = NULL;
            tmp->next = newNode;
        }
    }
}

int main()
{
    int invalidWords = 0;
    node *head = malloc(sizeof(node));
    if (head == NULL)
    {
        memoryError();
    }
    head->str = malloc(sizeof(strNode));
    if (head->str == NULL)
    {
        memoryError();
    }
    head->str = nextStrNode("");
    //test
    head->str->next = NULL;
    head->next = NULL;
    //-------------------------
    char input[64];
    while (scanf("%64s", input) != EOF)
    {
        if (legal(input))
        {
            add(input, head);
        }
        else
        {
            fprintf(stderr, "Bad word: %s \n", input);
            invalidWords++;
        }
    }
    if (head->next != NULL)
    {
        print(head->next);
    }
    return invalidWords;
}

