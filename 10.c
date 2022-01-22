/* Advent of Code Day 10
   https://adventofcode.com/2021/day/10

Synopsis: Given lines of brackets and braces, make sure they
are all closed correctly. Very similar to what a compiler
(and a good code editor) do. A chunk is anything between
brackets.

Part 1: Look for corrupted lines, where chunks are closed
    with the wrong type of bracket. Find the first illegal
    character on each line.
Part 2: TBD - but judging from the data (not all lines the same length)
    and the text of the puzzle, it may ask me to complete the
    remaining lines? And calculate the score required for doing so?

Part 1 Plan:
    This seems like another problem that uses a stack (last in, first out) data structure.
    When I see an opening bracket "([{<" add it to the stack. When I see a closing
    bracket ")]}>", check the thing on the top of the stack. If it matches, great.
    If not, flag an illegal character.

Character values for brackets:
( = 40
) = 41
[ = 91
] = 93
{ = 123
} = 125
< = 60
> = 62
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "util.h"

#define INPUT_LEN 128       // length of each line
#define MAX_LINES 128       // max number of lines
#define NUM_BRACKET_TYPES 4 // number of bracket characters we use

const char openers[] = "([{<";
const char closers[] = ")]}>";
const int illegal_scores[] = { 3, 57, 1197, 25137 };

/*
* Return whether a character is an opening bracket
*
* @param    input       character to check
* @retval   1           character is opening bracket
* @retval   0           character is not opening bracket
*/
int is_opener(char input)
{
    for (int i = 0; i < NUM_BRACKET_TYPES; i++) {
        if (input == openers[i])
            return 1;
    }
    return 0;
}

/*
* Return whether a character is an closing bracket
*
* @param    input       character to check
* @retval   1           character is closing bracket
* @retval   0           character is not closing bracket
*/
int is_closer(char input)
{
    for (int i = 0; i < NUM_BRACKET_TYPES; i++) {
        if (input == closers[i])
            return 1;
    }
    return 0;
}

/*
* Return whether a character is valid for this puzzle.
*
* @param    input       character to check
* @retval   1           character is valid
* @retval   0           character is invalid
*/
int char_is_valid(char input)
{
    return (is_closer(input) || is_opener(input));
}

/*
* Return the matching bracket for a given input
* 
* @param    input       input bracket to match
* @retval   0           No match found
* @retval               matching bracket
*/
char match(char input)
{
    if (!char_is_valid(input)) {
        printf("Error: Invalid character %c sent to match().\n", input);
        return 0;
    } 
    /*printf("Checking for match to %c.\n", input);*/
    for (int i = 0; i < NUM_BRACKET_TYPES; i++) {
        if (input == openers[i]) {
            return closers[i];
        } else if (input == closers[i]) {
            return openers[i];
        } 
    }
    return 0;
}

/*
* Push an unsigned long onto the end of an array
*
* @param    p_array             pointer to the array to push to
* @param    p_n                 pointer to the number of elements in the array
* @param    element             unsigned long element to add to the end of the array
*/
void ul_push(unsigned long *p_array, int *p_n, unsigned long element)
{
    if (p_array == NULL || p_n == NULL) {
        printf("Invalid pointer in push()\n");
        exit(-1);
    }
    if (*p_n < 0) {
        printf("Error: push() attempted to access array of size < 0.\n");
        exit(-1);
    }
    p_array[*p_n] = element;
    (*p_n)++; // increase array size
}

/*
* Push a char onto the end of an array
*
* @param    p_array             pointer to the array to push to
* @param    p_n                 pointer to the number of elements in the array
* @param    element             char to add to the end of the array
*/
void c_push(char *p_array, int *p_n, char element)
{
    if (p_array == NULL || p_n == NULL) {
        printf("Invalid pointer in c_push()\n");
        exit(-1);
    }
    if (*p_n < 0) {
        printf("Error: c_push() attempted to access array of size < 0.\n");
        exit(-1);
    }
    p_array[*p_n] = element;
    (*p_n)++; // increase array size
}

/*
* pop a char off of the end of an array
*
* @param    p_array             pointer to the array to pop from
* @param    p_n                 pointer to the number of elements in the array
* @retval   element             char at the end of the array
*/
char c_pop(char *p_array, int *p_n)
{
    if (p_array == NULL || p_n == NULL) {
        printf("Invalid pointer in c_pop()\n");
        exit(-1);
    }
    if (*p_n < 0) {
        printf("Error: c_pop() attempted to access array of size < 0.\n");
        exit(-1);
    }
    (*p_n)--; // decrease the size of the array
    return p_array[*p_n];
}

/*
* Given an illegal character, return its score.
*
* @param    illegal_char        the illegal character passed
* @retval                       the score for that character
* @retval   0                   unexpected character
*/
int bracket_score(char illegal_char)
{
    if (!char_is_valid(illegal_char)) {
        printf("Warning: Invalid character sent to bracket_score().\n");
        return 0;
    }
    for (int i = 0; i < NUM_BRACKET_TYPES; i++) {
        if (illegal_char == closers[i])
            return illegal_scores[i];
    }
    return 0;
}

/*
* Add to the score for a given line in part 2 based on the character added.
*
* @param    added_char       character that was added
* @param    starting_score   starting score for given line
* @retval   0                bad character input
* @retval   new score
*/
unsigned long part_2_score(char added_char, unsigned long starting_score)
{
    if (!is_closer(added_char)) {
        printf("Warning: Bad added_char to part_2_score.\n");
        return 0;
    }
    unsigned long new_score = starting_score * 5;
    for (int i = 0; i < NUM_BRACKET_TYPES; i++) {
        if (added_char == closers[i]) {
            new_score += i + 1;
            break;
        }
    }
    return new_score;
}

/*
* Print an array to the screen (debug function)
*
* @param    array       the array to print
* @param    n           the length of the array
*                       (or number of elements to print)
*/
void print_array_ul(unsigned long array[], int n)
{
    if (array == NULL) {
        printf("Error: print_array() called with bad pointer.\n");
        exit(-1);
    }
    printf("[ ");
    for (int i = 0; i < n; i++) {
        printf("%lu ", array[i]);
    }
    printf("]\n");
}

/*
* Sort an array of unsigned longs in descending order (largest element first)
*
* @param        array       the array to sort
* @param        n           number of elements in array
* @param        n_to_sort   number of elements to sort
*                           set equal to n to sort entire array
* @retval       0           sort successful
* @retval       1           sort failure
*/
int array_sort_descending(unsigned long *array, int n, int n_to_sort)
{
    if (array == NULL) {
        printf("Error: Bad pointer.\n");
        return 1;
    }
    if (n < 1) {
        printf("Error: Cannot sort array with 0 or fewer elements.\n");
        return 1;
    }
    int num_sorted = 0;
    // keep going until we've sorted as many as required
    while (num_sorted < n_to_sort) {
        // start at the first index that has not been sorted
        int max_index = num_sorted;
        for (int i = num_sorted; i < n; i++) {
            if (array[i] > array[max_index]) {
                max_index = i;
            }
        }
        unsigned long tmp = array[num_sorted];
        array[num_sorted] = array[max_index];
        array[max_index] = tmp;
        num_sorted++;
    }

    return 0; // success
}

/*
* Read and solve the puzzle.
*
* @param    datafile        data file to open
*/
void read_puzzle(char datafile[])
{
    FILE *file_stream = fopen(datafile, "r");
    if (file_stream == NULL) {
        printf("Error reading input file %s: %s\n", datafile, strerror(errno));
        exit(-1);
    }
    // track num opening brackets
    int num_openers;

    // array to track part 1
    int num_corrupted_lines = 0;
    int score_corrupted[MAX_LINES];

    // array to track part 2
    int num_incomplete_lines = 0;
    unsigned long score_incomplete[MAX_LINES]; 

    // buffers for opening brackets & input
    char opening_brackets[INPUT_LEN];
    char input_buffer[INPUT_LEN];

    // read the file one line at a time
    while (fgets(input_buffer, sizeof(input_buffer), file_stream)) {
        unsigned long score_part_2 = 0;
        char current_char;
        num_openers = 0;    // reset counter to zero
        for (int char_index = 0; char_index < INPUT_LEN; char_index++) {
            // get the next character from the buffer
            current_char = input_buffer[char_index];
            if (current_char == '\n') 
                break; // go to next line

            // if character is opening bracket, add it to the stack
            if (is_opener(current_char)) {
                c_push(opening_brackets, &num_openers, current_char);
            } else if (is_closer(current_char)) { // else if character is closing bracket, 
                // pop the stack and compare them
                if (c_pop(opening_brackets, &num_openers) == match(current_char)) {
                    // if they match, continue
                    continue;
                } else {
                    // if not a match, return the illegal character
                    push(score_corrupted, &num_corrupted_lines, bracket_score(current_char));
                    goto next_line; // line is corrupted, move to next
                }
            }
        }
        // part II:
        while (num_openers > 0) { // add closing brackets to all unclosed
            char next = c_pop(opening_brackets, &num_openers);
            score_part_2 = part_2_score(match(next), score_part_2);
        }
        // add final score to array of 
        ul_push(score_incomplete, &num_incomplete_lines, score_part_2);
        /*printf(" - Score: %lu", score_part_2);*/
next_line:
        ;
    }
    printf("\n");
    printf("Num corrupted: %d\n", num_corrupted_lines);
    printf("Corrupted Line Score: %d\n", sum(score_corrupted, &num_corrupted_lines));
    printf("Num incomplete: %d\n", num_incomplete_lines);
    array_sort_descending(score_incomplete, num_incomplete_lines, num_incomplete_lines);
    /*print_array_ul(score_incomplete, num_incomplete_lines);*/
    printf("Incomplete Line Middle Score: %lu\n", score_incomplete[num_incomplete_lines/2]);

    fclose(file_stream);
}

int main(int argc, char *argv[])
{
    printf("Part 1:\n");
    printf("Test Input ");
    read_puzzle("data/10test"); 

    printf("Full Input "); 
    read_puzzle("data/10data"); 
    return 0;
}
