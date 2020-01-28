#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "stringStack.h"
#include "intStack.h"
#include "utility.h"
#include "calc.h"


void processDigit(char *result, const char *input, int *outerIndex) {
    char token = input[*outerIndex];
    char num[50];
    int i = 0;
    while (isdigit(token)) {
        num[i] = token;
        i++;
        char result[50];
        token = input[*outerIndex + i];
    }
    num[i] = '\0';
    *outerIndex += i - 1;
    strcpy(result, num);
}

void moveOp(int *opIndex, struct stringStack input) {
    while (!isoperator(*input.contents[*opIndex])) {
        if (*opIndex == input.size)
            break;
        (*opIndex)++;
    }
}

int evalStack(struct stringStack input) {
    struct intStack numStack;
    initIntStack(&numStack);
    char *top;
    int first;
    int second;
    int result;
    while (input.size > 0) {
        top = popString(&input);
        if (isNumber(top))
            pushInt(realatoi(top), &numStack);
        else {
            first = popInt(&numStack);
            second = popInt(&numStack);
            result = doOperation(*top, second, first);
            pushInt(result, &numStack);
        }
    }
    assert(numStack.size == 1);
    return topInt(&numStack);
}

int calc(const char *input) {
    struct stringStack operatorStack;
    initStringStack(&operatorStack);
    struct stringStack outputStack;
    initStringStack(&outputStack);

    for (int i = 0; input[i] != '\0'; i++) {
        char token = input[i];

        if (isdigit(token)) {
            char out[50];
            processDigit(out, input, &i);
            pushString(out, &outputStack);
        }
        if (isoperator(token)) {
            while (getPrecedence(*topString(&operatorStack)) >= getPrecedence(token)) {
                operatorToOutput(&operatorStack, &outputStack);
            }
            pushString(&token, &operatorStack);
        }
        if (token == '(')
            pushString(&token, &operatorStack);
        if (token == ')') {
            char *prevString;
            while (*topString(&operatorStack) != '(' && topString(&operatorStack) != "")
                operatorToOutput(&operatorStack, &outputStack);
            if (*topString(&operatorStack) == '(')
                free(popString(&operatorStack));
        }
    }
    while (operatorStack.size > 0) {
        operatorToOutput(&operatorStack, &outputStack);
    }
    reverseStringArray(outputStack.contents, outputStack.size);
    // printStack(outputStack);
    return evalStack(outputStack);
}

void printStack(struct stringStack s) {
    char *out[1000];
    int i;
    for (i = 0; s.size > 0; i++) {
        out[i] = popString(&s);
    }
    for (int j = 0; j < i; j++) {
        printf("%s\n", out[j]);
    }
}
