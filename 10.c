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

#define INPUT_LEN 128       // length of each line
#define NUM_BRACKET_TYPES 4 // number of bracket characters we use

const char openers[] = "([{<";
const char closers[] = ")]}>";

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
    switch(input) {
        case '(':
            return ')';
        case ')':
            return '(';

        case '[':
            return ']';
        case ']':
            return '[';

        case '{':
            return '}';
        case '}':
            return '{';

        case '<':
            return '>';
        case '>':
            return '<';

        default:
            return '\0';
    }
}

/*
* Push an element onto the end of an array
*
* @param    p_array             pointer to the array to push to
* @param    p_n                 pointer to the number of elements in the array
* @param    element             element to add to the end of the array
*/
void push(char *p_array, int *p_n, char element)
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
* pop an element off of the end of an array
*
* @param    p_array             pointer to the array to pop from
* @param    p_n                 pointer to the number of elements in the array
* @retval   element             element at the end of the array
*/
char pop(char *p_array, int *p_n)
{
    if (p_array == NULL || p_n == NULL) {
        printf("Invalid pointer in pop()\n");
        exit(-1);
    }
    if (*p_n < 0) {
        printf("Error: pop() attempted to access array of size < 0.\n");
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
    switch((int)(illegal_char)) {
        case 41:    // ')'
            return 3;
        case 93:    // ']'
            return 57;
        case 125:   // '}'
            return 1197;
        case 62:    // '>'
            return 25137;
        default:
            printf("Warning: unexpected character %c passed to bracket_score!\n", illegal_char);
            return 0;
    }
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

    // read the file one line at a time
    char current_char;
    int line_num = 0;
    int num_openers = 0;
    int puzzle_score = 0;

    // stack of opening brackets
    char opening_brackets[INPUT_LEN];

    while ((current_char = fgetc(file_stream)) != EOF) {
        // go on to the next part of the puzzle if newline
        if (current_char == '\n') {
            line_num++;
            num_openers = 0;
            continue;
        }

        // if character is opening bracket, add it to the stack
        if (is_opener(current_char)) {
            push(opening_brackets, &num_openers, current_char);
            /*printf("%c - ", current_char);*/
            /*printf("Num openers: %d\n", num_openers);*/
        } else if (is_closer(current_char)) {
            // else if character is closing bracket, pop the stack and compare them
            char opener = pop(opening_brackets, &num_openers);
            /*printf("Char %d: Opener %c match with closer %c? %c\n", num_openers, opener,*/
                    /*current_char, match(current_char));*/
            if (opener == match(current_char)) {
                // if they match, continue
                continue;
            } else {
                // if not a match, return the illegal character
                // printf("Line %d: found illegal character %c\n", line_num, current_char);
                puzzle_score += bracket_score(current_char);
                // skip the rest of the line
                while(fgetc(file_stream) != '\n')
                    continue;
                line_num++;
            }
        }
    }
    printf("Final Score: %d\n", puzzle_score);
}

int main(int argc, char *argv[])
{
    printf("Part 1:\n");
    printf("Test Input ");
    read_puzzle("10test");

    printf("Full Input ");
    read_puzzle("10data");
    return 0;
}
