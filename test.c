#include "util.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define AssertEq(left, right) test_assert_eq(#left " == " #right, left, right)
#define CreateTest(left, right) Test_create(#left " == " #right, left, right)

// Tests for integer array functions
int n1 = 6;
int test_arr_1[] = { -1, -2, -1, -5, -4, -10 };
int test_arr_2[] = {-5, -3432, -1, 0, -11, -434 };
// min and max at first and last position
int test_arr_3[] = { -77, 432, 3, 29, 20, 1024 };
// min and max at last and first position
int test_arr_4[] = { 3437, 432, 3, 29, 20, -1024 };

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

void int_array_tests()
{
    // "util.h/min"
    AssertEq(*min(test_arr_1, &n1), -10);
    AssertEq(*min(test_arr_2, &n1), -3432);
    AssertEq(*min(test_arr_3, &n1), -77);
    AssertEq(*min(test_arr_4, &n1), -1024);
    struct Test *t1 = CreateTest(*min(test_arr_4, &n1), -1024);
    Test_print(t1);

    // "util.h/max"
    AssertEq(*max(test_arr_1, &n1), -1);
    AssertEq(*max(test_arr_2, &n1), 0);
    AssertEq(*max(test_arr_3, &n1), 1024);
    AssertEq(*max(test_arr_4, &n1), 3437);

}

int main(int argc, char *argv[])
{
    int_array_tests();
    return 0;
}
