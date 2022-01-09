#include "util.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/*#define AssertEq(left, right) test_assert_eq(#left " == " #right, left, right)*/
#define CreateTest(left, right) tests[num_tests] = *Test_create(#left " == " #right, left, right); num_tests++;

//#define CTA(fcn, args, res) tests[num_tests] = *Test_create_args(fcn, args, res); num_tests++;

#define MAX_TESTS 1024

// Tests for integer array functions
int test_arr_1[] = { -1, -2, -1, -5, -4, -10 };
int test_arr_2[] = {-5, -3432, -1, 0, -11, -434 };
// min and max at first and last position
int test_arr_3[] = { -77, 432, 3, 29, 20, 1024 };
// min and max at last and first position
int n1 = 6;
int n2 = 1;
int test_arr_4[] = { 3437, 432, 3, 29, 20, -1024 };
// array of single element
int test_arr_5[] = { 5 };
int test_arr_6[] = { 0, 0, 0, 0, 0, 0 }; // array of zeroes

//int errnum = 0;

struct Test {
    int input;
    int output;
    int pass;
    int errnum;
    char *function;
};

struct Test *Test_create(char function[], int input, int output)
{
    /* As written, this function RUNS the test case immediately.
       Goal is to have it only point to a function, and then
       run that function on command. */
    struct Test *ts = malloc(sizeof(struct Test));
    ts->function = strdup(function);
    ts->input = input;
    ts->output = output;
    ts->errnum = errnum;
    errnum = 0;
    if (ts->input != ts->output) {
        ts->pass = 0;
    } else {
        ts->pass = 1;
    }

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
    if (ts->errnum != 0) {
        printf("FAIL: Errnum = %d\n", ts->errnum);
        ts->pass = 0;
    } else if (ts->input == ts->output) {
        printf("PASS\n");
        ts->pass = 1;
    } else {
        printf("FAIL. errno = %d\n", errno);
        printf("Expected: %d; Received: %d\n", ts->input, ts->output);
    }
    errnum = 0; // reset for next function
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
    CreateTest(*min(test_arr_6, &n1), 0);
    CreateTest(*min(test_arr_5, &n2), 5);

    // "util.h/max"
    CreateTest(*max(test_arr_1, &n1), -1);
    CreateTest(*max(test_arr_2, &n1), 0);
    CreateTest(*max(test_arr_3, &n1), 1024);
    CreateTest(*max(test_arr_4, &n1), 3437);
    CreateTest(*max(test_arr_5, &n2), 5);
    CreateTest(*max(test_arr_6, &n1), 0);

    // "util.h/is_in"
    CreateTest(is_in(0, test_arr_2, n1), 1);
    CreateTest(is_in(1777, test_arr_2, n1), 0);
    CreateTest(is_in(1777, test_arr_5, n2), 0);
    CreateTest(is_in(5, test_arr_5, n2), 1);

    // "util.h/range"
    CreateTest(range(test_arr_5, &n2), 0);
    CreateTest(range(test_arr_6, &n2), 0);
    CreateTest(range(test_arr_1, &n1), 9);
    CreateTest(range(test_arr_4, &n1), 4461);

    // "util.h/sum"
    CreateTest(sum(test_arr_5, &n2), 5);
    CreateTest(sum(test_arr_6, &n2), 0);
    CreateTest(sum(test_arr_1, &n1), -23);
    CreateTest(sum(test_arr_4, &n1), 2902);

    // "util.h/mean"
    CreateTest(mean(test_arr_5, &n2), 5);
    CreateTest(mean(test_arr_6, &n2), 0);
    CreateTest(mean(test_arr_1, &n1), -3);
    CreateTest(mean(test_arr_4, &n1), 483);

    Tests_run(tests, &num_tests);

    return 0;
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
