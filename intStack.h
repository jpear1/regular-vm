#pragma once

#define MAX_INT_STACK_SIZE 1000

struct intStack {
    int *contents;
    int size;
};

int pushInt(int input, struct intStack *stack);
int popInt(struct intStack *stack);
int topInt(struct intStack *stack);