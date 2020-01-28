#pragma once

#include "stringStack.h"

void processDigit(char *result, const char *input, int *outerIndex);

void moveOp(int *opIndex, struct stringStack input);


int evalStack(struct stringStack input);

int calc(const char *input);

void printStack(struct stringStack s);