#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "intStack.h"

int pushInt(int input, struct intStack *stack) {
    assert(stack->size < MAX_INT_STACK_SIZE);
    stack->contents[stack->size] = input;
    stack->size++;
    return 0;
}

int popInt(struct intStack *stack) {
    assert(stack->size > 0);
    stack->size--;
    return stack->contents[stack->size];
}

int topInt(struct intStack *stack) {
    if (stack->size == 0)
        return -1;
    return stack->contents[stack->size - 1];
}