#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// global for row/column length
#define SET_SIZE 5
#define MAX_CALLS 255
#define MAX_BOARDS 255
/* Assumptions:
    Given input file where first line is comma separated list of
    values.
    Remainder of file is groups of 5 rows of 5 integers each
    Separated by blank row.
    Assuming that numbers are NOT repeated anywhere on a bingo Board!
*/

/*  Plan:
    - Create a struct for Board
        - A Board has 10 Sets of 5 elements each (5 rows, 5 cols)
    - Read an input file
        - First row will be placed into calls[] array
        - Skip a row
        - Next 5 rows are fed into the first Board
    - Functions needed:
        - Check a Set (array of 5 elements) against a series of calls
            and a number of turns
*/

int is_in(int element, int array[], int arr_size) 
{
    int i;
    for (i = 0; i < arr_size; i++) {
        if (array[i] == element) {
            return 1;
        }
    }
    return 0;
}

int is_complete(int Set[], int calls[], int turn)
{
    int counter = 0;
    int i;
    for (i = 0; i < turn; i++) {
        if (is_in(calls[i], Set, SET_SIZE)) {
            counter++;
            if (counter == SET_SIZE) {
                return 1;
            }
        }
    }
    return 0;
}

struct Set { // row or column
    int numbers[SET_SIZE];
};

struct Board {
    struct Set rows[SET_SIZE];
    struct Set cols[SET_SIZE];
    int winning_turn;
};

int Board_complete(struct Board *brd, int calls[], int turn)
{
    int r;
    for (r = 0; r < SET_SIZE; r++) {
        if (is_complete(brd->rows[r].numbers, calls, turn) ||
                is_complete(brd->cols[r].numbers, calls, turn)) {
            brd->winning_turn = turn;
            return 1;
        }
    }
    return 0;
}

struct Set *Set_create(int Set_nums[])
{
    struct Set *Set = malloc(sizeof(struct Set));
    assert(Set != NULL);

    int i;
    for (i = 0; i < SET_SIZE; i++) {
        Set->numbers[i] = Set_nums[i];
    }
    return Set;
}

void Set_print(struct Set *Set)
{
    int i;
    printf("[ ");
    for (i = 0; i < SET_SIZE; i++) {
        printf("%d ", Set->numbers[i]);
    }
    printf("]\n");
}

struct Board *Board_create()
{
    struct Board *brd = malloc(sizeof(struct Board));
    brd->winning_turn = -1;

    return brd;
}

void Board_print(struct Board *brd)
{
    int r;
    for (r = 0; r < SET_SIZE; r++) {
        Set_print(&brd->rows[r]);
    }
}


int split_str(int array[], char str[], char delimeter[])
{
    int splits = 0;
    char *ptr = strtok(str, delimeter);
    while (ptr != NULL) {
        array[splits] = atoi(ptr);
        ptr = strtok(NULL, delimeter);
        splits++;
    }
    return splits;
}

void print_array(int array[], int num_elements) {
    int i;
    printf("[ ");
    for (i = 0; i < num_elements; i++) {
        printf("%d ", array[i]);
    }
    printf("]\n");
}

void fill_cols(struct Board *brd)
{
    int row, col;
    for (row = 0; row < SET_SIZE; row++) {
        for (col = 0; col < SET_SIZE; col++) {
            brd->cols[col].numbers[row] = brd->rows[row].numbers[col];
        }
    }
}

int sum_uncalled(struct Board *brd, int calls[], int turn)
{
    int sum = 0;
    int r, c;
    int num = 0;
    for (r = 0; r < SET_SIZE; r++) {
        for (c = 0; c < SET_SIZE; c++) {
            num = brd->rows[r].numbers[c];
            if (!is_in(num, calls, turn)) {
                sum += num;
            }
        }
    } 
    return sum;
}

void run_tests()
{
    int test_calls[] = { 1, 2, 3, 4, 5, 6, 7, 8, 19, 10 };
    int test_row[] = { 1, 2, 3, 6, 19 };
    int test_col[] = { 5, 4, 3, 2, 1 };
    int test_input[] = { 4, 3, 6, 1, 8 };
    int r1[] = { 3, 4, 5, 6, 7 };
    int r2[] = { 8, 9, 10, 11, 12 };
    int r3[] = { 13, 14, 15, 16, 17 };
    int r4[] = { 18, 19, 20, 21, 22 };
    int r5[] = { 23, 24, 25, 26, 27 };

    struct Set *test_Set = Set_create(test_input);
    struct Set *R1 = Set_create(r1);
    struct Set *R2 = Set_create(r2);
    struct Set *R3 = Set_create(r3);
    struct Set *R4 = Set_create(r4);
    struct Set *R5 = Set_create(r5);

    struct Set *b_test[] = { R1, R2, R3, R4, R5 };
    struct Board *b = Board_create();

    int i;
    for (i = 0; i < SET_SIZE; i++) {
        b->rows[i] = *b_test[i];
    }
    Set_print(b_test[0]);
    Set_print(b_test[1]);
    Board_print(b);
    printf("\n");

    printf("5 in test_col: %d\n", is_in(8, test_col, 5));
    printf("Set size: %ld\n", sizeof(struct Set));
    printf("Board size: %ld\n", sizeof(struct Board));
    printf("test_row complete: %d\n", is_complete(test_row, test_calls, 9));
    printf("test_col complete: %d\n", is_complete(test_col, test_calls, 3));
    printf("test_Set: ");
    Set_print(test_Set);
    printf("test_Set complete: %d\n", is_complete(test_Set->numbers, test_calls, 10));

    char str_test[] = "1,24,33,4,5\n";
    int split_test[12];
    int split_len = split_str(split_test, str_test, ",");
    printf("split_len: %d\n", split_len);
    print_array(split_test, split_len);
}

void read_data(char data_file[], int calls[], int *num_calls, 
        struct Board boards[], int *board_count)
{
    FILE *data = fopen(data_file, "r");
    assert(data != NULL);

    char buffer[MAX_CALLS];
    *num_calls = split_str(calls, fgets(buffer, sizeof(buffer), data), ",");
//    print_array(calls, num_calls);

    int set_count = 0;
    int set_arr[SET_SIZE];
    int row_len;

    // advance past first newline
    fgets(buffer, sizeof(buffer), data);
    boards[0] = *Board_create();
    while(fgets(buffer, sizeof(buffer), data) != NULL) {
        if (buffer[0] == '\n') {
            fill_cols(&boards[*board_count]);
            *board_count += 1;
            boards[*board_count] = *Board_create();
            set_count = 0;
            continue;
        } else {
            row_len = split_str(set_arr, buffer, " ");
            assert(row_len == 5);
            boards[*board_count].rows[set_count] = *Set_create(set_arr);
            // printf("%d-%d: ", board_count, set_count);
            // Set_print(&set_tmp);
            // Board_print(&boards[board_count]);
            set_count++;
            // printf("%s", buffer);
        }
    }

    /*printf("\n");*/
    /*Board_print(&boards[2]);*/
    /*printf("\n");*/
    /*Set_print(&boards[1].cols[4]);*/
    fclose(data);
}

int score(struct Board *brd, int calls[], int turn)
{
    int uncalled = sum_uncalled(brd, calls, turn);
    printf("Sum of uncalled: %d\n", uncalled);
    printf("Last call: %d\n", calls[turn - 1]);
    int result = calls[turn - 1] * uncalled;
    printf("Result: %d\n", result);

    return result;
}

int main(int argc, char *argv[])
{
    int calls[MAX_CALLS];
    int num_calls;
    int board_count = 0;
    struct Board boards[MAX_BOARDS];
    read_data("4data", calls, &num_calls, boards, &board_count);
    //run_tests();

    // start at turn 5, since impossible to win before this
    int turn = 4;
    int b;
    int num_winners = 0;

    while (turn <= num_calls) {
        // each turn, look through all the boards
        for (b = 0; b <= board_count; b++) {
            // only check board that haven't won yet
            if (boards[b].winning_turn == -1) {
                if (Board_complete(&boards[b], calls, turn)) {
                    num_winners++;
                    // Assume all boards win eventually
                    if ((num_winners == 1 || num_winners >= board_count)) {
                        printf("Board number %d complete on turn %d!\n",
                                b, turn);
                        score(&boards[b], calls, turn);
                    }
                }
            }
        }

        turn++;
        if ((turn == num_calls) && (num_winners == 0)) {
            printf("no winner!\n");
        }
    }

    return 0;
}
