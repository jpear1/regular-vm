#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "stringStack.h"

// this stack doesn't copy strings that are added to it

int pushString(char *inString, struct stringStack *stack) {
    assert(stack->size < MAX_STRING_STACK_SIZE);
    int inLen = strlen(inString);
    char *copy = malloc(inLen * sizeof(char) + 1);
    copy = strcpy(copy, inString);
    stack->contents[stack->size] = copy;
    stack->size++;
    return 0;
}

char* popString(struct stringStack *stack) {
    assert(stack->size > 0);
    stack->size--;
    return stack->contents[stack->size];
}

char* topString(struct stringStack *stack) {
    if (stack->size == 0)
        return "";
    return stack->contents[stack->size - 1];
}

void stringStackTest() {
    struct stringStack stack;
    stack.contents = malloc(MAX_STRING_STACK_SIZE * sizeof(char*));
    stack.size = 0;
    char *ops[2] = {"+", "-"};
    pushString(ops[0], &stack);
    pushString(ops[1], &stack);
    assert(popString(&stack) == "-");
    assert(popString(&stack) == "+");
}