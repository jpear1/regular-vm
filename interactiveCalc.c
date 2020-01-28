#include <stdio.h>
#include "calc.h"

int main() {
    printf("Enter your expression then press return:\n");
    int max_len = 100;
    char input [max_len];
    fgets(input, max_len, stdin);
    int result = calc(input);
    printf("%d\n", result);
}