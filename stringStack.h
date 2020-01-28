#pragma once

#define MAX_STRING_STACK_SIZE 1000

struct stringStack {
    char **contents;
    int size;
};

int pushString(char *inString, struct stringStack *stack);
char* popString(struct stringStack *stack);
char* topString(struct stringStack *stack);
void stringStackTest();