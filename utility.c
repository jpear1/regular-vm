#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "stringStack.h"
#include "intStack.h"
#include "utility.h"

int realatoi(char *const input) {
    if (isdigit(input[0]))
        return atoi(input);
    if (input[0] == '-')
        return -atoi(input+1);
    return INT_MIN;
}

void reverseStringArray(char **a, int size) {
    char *tmp;
    for (int i = 0; i < size / 2; i++) {
        tmp = a[size - i - 1];
        a[size - i - 1] = a[i];
        a[i] = tmp;
    }
}

int doOperation(char operator, int rhs, int lhs) {
    switch (operator) {
        case '+':
            return rhs + lhs;
        case '-':
            return rhs - lhs;
        case '*':
            return rhs * lhs;
        case '/':
            return rhs / lhs;
        exit(-1);
    }
}


int isoperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

void initStringStack(struct stringStack *s) {
    s->size = 0;
    s->contents = malloc(MAX_STRING_STACK_SIZE * sizeof(char*));
}

void initIntStack(struct intStack *s) {
    s->size = 0;
    s->contents = malloc(MAX_INT_STACK_SIZE * sizeof(int));
}

int removeSpaceFromString(char *input) {
    int inLen = strlen(input);
    char spaceFreeOutput[inLen];
    int spaceFreeSize = 0;
    for (int i = 0; i < inLen; i++) {
        if (input[i] != ' ') {
            spaceFreeOutput[spaceFreeSize] = input[i];
            spaceFreeSize++;
        }
    }
    for (int i = 0; i < spaceFreeSize; i++)
        input[i] = spaceFreeOutput[i];
    input[spaceFreeSize] = '\0';
    return spaceFreeSize;
}


void strAppend(char input, char *str) {
    int l = strlen(str);
    str[l] = '\0';
    str[l-1] = input;
}

int getPrecedence(char operator) {
    if (operator == '+' || operator == '-')
        return 0;
    if (operator == '*' || operator == '/')
        return 1;
    return -1;
}

void operatorToOutput(struct stringStack *op, struct stringStack *out) {
    char *s = popString(op);
    pushString(s, out);
    free(s);
}

int isNumber(char *str) {
    if (strlen(str) == 1)
        return isdigit(*str);
    if (strlen(str) > 1)
        return isdigit(str[1]);
    return 0;
}