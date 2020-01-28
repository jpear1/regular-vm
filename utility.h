#pragma once

#include "stringStack.h"
#include "intStack.h"

int realatoi(char *const input);
void reverseStringArray(char **a, int size);
int doOperation(char operator, int rhs, int lhs);
int isoperator(char c);
void initStringStack(struct stringStack *s);
void initIntStack(struct intStack *s);
int removeSpaceFromString(char *input);
void strAppend(char input, char *str);
int getPrecedence(char operator);
void operatorToOutput(struct stringStack *op, struct stringStack *out);
int isNumber(char *str);