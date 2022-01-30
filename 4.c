/* Advent of Code day 4
   https://adventofcode.com/2021/day/4

Synopsis: Bingo game with a giant squid. Given a sequence
of calls, and a sequence of bingo boards, determine which board
will win. Diagonals don't count.
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#define PRINT_DEBUG
// global for row/column length
#define SET_SIZE 5
#define MAX_CALLS 900
#define MAX_BOARDS 900
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

/*
* Determine if an element is in an array
*
* @param    element     the element to check for
* @param    p_array     pointer to the array to look in
* @param    array_size  number of elements in the array
* @retval   0           element not in the array
* @retval   1           element is in array
*/
int is_in(int element, int *p_array, int arr_size) 
{
    for (int i = 0; i < arr_size; i++) {
        if (p_array[i] == element) {
            return 1;
        }
    }
    return 0;
}

int is_complete(int Set[], int calls[], int turn)
{
    int counter = 0;
    for (int i = 0; i < turn; i++) {
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

struct Card { // bingo card to replace board struct
    uint8_t *numbers;
    uint8_t card_size;
    unsigned winning_turn;
};

struct Card *Card_create(uint8_t card_size)
{
    struct Card *card = malloc(sizeof(struct Card));
    card->winning_turn = 0;
    card->card_size = card_size;
    card->numbers = malloc(card_size * card_size * sizeof(uint8_t*));

    return card;
}
/*
* Determine whether a board is complete (a winner)
*
* @param    p_brd_t     pointer to the Board to check
* @param    p_calls     pointer to the array of calls
* @param    turn        turn number we're on
* @retval   0           board is not complete
* @retval   1           board is complete
*/
int Board_complete(struct Board *p_brd_t, int *p_calls, int turn)
{
    for (int r = 0; r < SET_SIZE; r++) {
        if (is_complete(p_brd_t->rows[r].numbers, p_calls, turn) ||
                is_complete(p_brd_t->cols[r].numbers, p_calls, turn)) {
            p_brd_t->winning_turn = turn;
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

/*
* display the contents of an integer array
*
* @param    p_array         pointer to the array to display
* @param    num_elements    number of elemnts to disply
*/
void print_array(int *p_array, int num_elements)
{
#ifdef PRINT_DEBUG
    printf("[ ");
    for (int i = 0; i < num_elements; i++) {
        printf("%d ", p_array[i]);
    }
    printf("]\n");
#endif
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

/*
* read from a data file a list of calls, and all bingo boards.
*
* @param    data_file       the data file to read from
* @param    num_calls       the number of calls expected
* @param    p_calls         pointer to the list of calls
* @param    num_boards      the number of boards expected
* @param    p_boards_t      pointer to list of boards
*/
void read_data(char data_file[], int *p_calls, int num_calls, 
        struct Board boards[], int num_boards)
{
    FILE *data = fopen(data_file, "r");
    assert(data != NULL);

    int *calls = malloc(num_calls * sizeof(int *));

    char current_input;
    int current_num = 0;
    int lines_read = 0;
    int calls_found = 0;
    /*int cards_found = 0;*/
    /*int card_index = 0;*/

    // read through the input file one character at a time
    while((current_input = fgetc(data)) != EOF) {
        if (current_input < '0' || current_input > '9') {
            if (lines_read == 0) {
                p_calls[calls_found] = current_num;
                calls_found++;
            } else {
            }
            if (current_input == '\n') lines_read++;

            current_num = 0;
            continue;
        }
        current_num = current_num * 10 + current_input - '0';
    }

    assert(num_calls == calls_found);

    printf("Num calls: %d\n", num_calls);
    print_array(p_calls, num_calls);

/* OLD VERSION

    int set_count = 0;
    int set_arr[SET_SIZE];
    int row_len;

    // advance past first newline
    fgets(buffer, sizeof(buffer), data);
    boards[0] = *Board_create();
    while(fgets(buffer, sizeof(buffer), data) != NULL) {
        if (buffer[0] == '\n') {
            fill_cols(&boards[num_boards]);
            num_boards += 1;
            boards[num_boards] = *Board_create();
            set_count = 0;
            continue;
        } else {
            row_len = split_str(set_arr, buffer, " ");
            assert(row_len == SET_SIZE);
            boards[num_boards].rows[set_count] = *Set_create(set_arr);
            // printf("%d-%d: ", board_count, set_count);
            // Set_print(&set_tmp);
            // Board_print(&boards[board_count]);
            set_count++;
            // printf("%s", buffer);
        }
    }

    printf("\n");
    Board_print(&boards[2]);
    printf("\n");
    Set_print(&boards[1].cols[4]);
    fclose(data);
*/
}

/*
* Calculate the score for a bingo board based on the
* uncalled numbers
*
* @param    p_brd_t pointer to the board to calculate for
* @param    calls   the array of calls so far
* @param    turn    the turn we are on
* @retval   result  the resulting score
*/
int score(struct Board *p_brd_t, int calls[], int turn)
{
    int uncalled = sum_uncalled(p_brd_t, calls, turn);
    printf("Sum of uncalled: %d\n", uncalled);
    printf("Last call: %d\n", calls[turn - 1]);
    int result = calls[turn - 1] * uncalled;
    printf("Result: %d\n", result);

    return result;
}

int main(int argc, char *argv[])
{
    /*int *calls = NULL;*/
    /*int num_calls = 27;*/
    int *calls;
    printf("Address of calls: %p\n", &calls);
    /*int board_count = 0;*/ struct Board boards[MAX_BOARDS];
    // puzzle input
    read_data("data/4test", calls, 27, boards, 3);
    printf("calls[2]: %d\n", calls[2]);
    /*read_data("data/4data", calls, 100, boards, 125);*/
    /*read_data("bigdata/4-900-15", calls, 900, boards, 900);*/
    /*read_data("bigdata/4-3600-30", calls, 3600, boards, 3600);*/
    //run_tests();

    /* OLD VERSION
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
    */

    return 0;
}
