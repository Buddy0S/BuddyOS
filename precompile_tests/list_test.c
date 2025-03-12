#include <list.h>
#include <stdio.h>

#define NUM_TESTS 2

struct TestCase {
    int (*test)(void);
    const char* test_string;
};

int test1(void) {
    return 0;
}

int test2(void) {
    return -1;
}

struct TestCase testcases[NUM_TESTS] = {
    {test1, "Will always pass"},
    {test2, "Will always fail"}
};

int main(void) {
   for (int i = 0; i < NUM_TESTS; i++) {
       printf("%s: ", testcases[i].test_string);
       if (testcases[i].test()) {
           printf("\033[31;1m✘\033[0m\n");
       } else {
           printf("\033[32;1m✔\033[0m\n");
       }
   }
}
