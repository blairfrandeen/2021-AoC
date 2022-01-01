#include "util.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/*#define AssertEq(left, right) test_assert_eq(#left " == " #right, left, right)*/
#define CreateTest(left, right) tests[num_tests] = *Test_create(#left " == " #right, left, right); num_tests++;

#define MAX_TESTS 1024

// Tests for integer array functions
int n1 = 6;
int test_arr_1[] = { -1, -2, -1, -5, -4, -10 };
int test_arr_2[] = {-5, -3432, -1, 0, -11, -434 };
// min and max at first and last position
int test_arr_3[] = { -77, 432, 3, 29, 20, 1024 };
// min and max at last and first position
int test_arr_4[] = { 3437, 432, 3, 29, 20, -1024 };


struct Test {
    int left;
    int right;
    int pass;
    char *function;
};

struct Test *Test_create(char function[], int left, int right)
{
    struct Test *ts = malloc(sizeof(struct Test));
    ts->function = strdup(function);
    ts->left = left;
    ts->right = right;
    ts->pass = 0; // don't pass until test is run

    return ts;
}

void Test_print(struct Test *ts)
{
    printf("Test is: %s\n", ts->function);
}

void Test_assert_eq(struct Test *ts)
{
    printf("Test assertion: ");
    printf("%s: ", ts->function);
    if (ts->left == ts->right) {
        printf("PASS\n");
        ts->pass = 1;
    } else {
        printf("FAIL. errno = %d\n", errno);
        printf("Expected: %d; Received: %d\n", ts->right, ts->left);
    }
}

void Tests_run(struct Test *tests, int *num_tests)
{
    int num_passed = 0;
    for(int i = 0; i < *num_tests; i++) {
        Test_assert_eq(&tests[i]);
        if (tests[i].pass)
            num_passed++;
    }
    printf("----------\n");
    printf("%d/%d Tests Passed.\n", num_passed, *num_tests);
}

int main(int argc, char *argv[])
{
    int num_tests = 0;
    struct Test tests[MAX_TESTS];

    // "util.h/min"
    CreateTest(*min(test_arr_1, &n1), -10);
    CreateTest(*min(test_arr_2, &n1), -3432);
    CreateTest(*min(test_arr_3, &n1), -77);
    CreateTest(*min(test_arr_4, &n1), -1024);

    // "util.h/max"
    CreateTest(*max(test_arr_1, &n1), -1);
    CreateTest(*max(test_arr_2, &n1), 0);
    CreateTest(*max(test_arr_3, &n1), 1024);
    CreateTest(*max(test_arr_4, &n1), 3437);

    Tests_run(tests, &num_tests);
}

int test_assert_eq(char function[], int left, int right)
{
    printf("Test assertion: ");
    printf("%s: ", function);
    if (left == right) {
        printf("PASS\n");
        return 0;
    } else {
        printf("FAIL. errno = %d\n", errno);
        printf("Expected: %d; Received: %d\n", right, left);
        return 1;
    }
}
