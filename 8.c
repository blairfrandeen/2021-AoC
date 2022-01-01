#include <stdio.h>
#include "util.h"
#include <stdlib.h>

#define NUM_SEGMENTS 7
#define NUM_DIGITS 4
#define NUM_INPUTS 10
#define MAX_LEN 256


/* SEVEN SEGMENT DISPLAY DEFINITION
*    AAA
*   F   B
*   F   B
*    GGG
*   E   C
*   E   C
*    DDD 
*/
/* Part I Plan:
   -> How many times do digits in the output
   (after the |) that are identifiable appear?
   -> Identifiable digits are 1, 4, 7, and 8, which use 2, 4, 3, and 7 segments, respectively

   - Use fgets() to grab each line of input
   - Search each line for the | character, and start after that
   - return an array with four ints, corresponding to length of outputs
   - Each time one of those ints is 1, 4, 7, or 8, add one to the counter

    Part II Plan:
    - This is a constraint satisfaction problem
    - I did this 1.5 years ago, and wrote a framework for it in Python
    - I'll be given 10 groups of characters. The order of the characters
        within the groups does not matter.
    - There are 10 possible combinations of characters
    - There are 10 possible configurations:
        BC ABGED ABCDG FGBC AFGCD AFGCED ABC ABCDEFG ABCDFG ABCDEF
    - Brute force logic:
        1) Find word with only two characters, these are B & C 
        2) Find word with three characters. Character that is not in word 1 is A. (A)
        3) Find word with four characters. 
        4) Find word with five characters that has both characters from 3) but
            only 1 character from 1). This is 5. The character from this word that
            is not in 1) or 3) is D. The character from 1 is C.
            The character from 1 that is not present is B. (B, C, D)
        5) Find the word with five characters that has both characters from 1.
            This is 3. The character that we don't yet have is G.
            The character in 3) that is not in this word is F.
            E is the last that remains. (E, F, G)

    - IN CSP Talk:
        - The variables are the letters a-g given in the input
        - The domains are the segments A-G of a standard 7-segment display
        - The constraints are:
            - Variables must come in words of 2-7 characters in length
                anything else is illegal
            - variables must fit into configurations described above.

   */

// return a pointer to the first instance of target within str
char *find_char(char *str, char target)
{
    char *c = str;
    if (*c == target)
        return c;
    while (*c != target && *c != '\0') {
        c++;
        if (*c == target)
            return c;
    }
    return NULL;
}

int words_equal(char *w1, char *w2, int len) {
    int match_count = 0;
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len; j++) {
            if (w1[i] == w2[j])
                match_count++;
        }
    }
    return (match_count == len);
}

void find_lengths(char *str, int *lengths, int num_words, char delim)
{
    if (*str == delim)
        str++;
    for (int i = 0; i < num_words; i++) {
        lengths[i] = 0;
        while(*str != '\0') {
            if (*str == delim || *(str) == '\n') {
                str++;
                break;
            }
//            printf("%c", *str);
            lengths[i]++;
            str++;
        }
//        printf("\n");
    }
}

int part_1(char datafile[])
{
    FILE *data = fopen(datafile, "r");
    char buffer[MAX_LEN];

    int count = 0;
    int *lengths = malloc(NUM_DIGITS * sizeof(int));
    int nums_to_count[4] = { 2, 3, 4, 7 };
    while(fgets(buffer, sizeof(buffer), data) != NULL) {
        find_lengths(find_char(buffer, '|') + 1, lengths, NUM_DIGITS, ' ');
        print_array(lengths, NUM_DIGITS);
        for (int i = 0; i < NUM_DIGITS; i++) {
            if (is_in(lengths[i], nums_to_count, NUM_DIGITS))
                count++;
        }
    }

    fclose(data);
    return count;
}

void tests() {
    char *test_str = "be cfbegad cbdgef fgaecd cgeb fdcge agebfd fecdb fabcd edb | fdgacbe cefdb cefbgd gcbe";
    char *bar = find_char(test_str, '|');
    int num_chars = bar - test_str;
    if (bar != NULL) {
        printf("The bar was found at %p, value %c, %d chars from start.\n", &bar, *bar, num_chars);
    } else {
        printf("The character was not found.\n");
    }

    int *lengths = malloc(NUM_DIGITS * sizeof(int));
    find_lengths(bar, lengths, NUM_DIGITS, ' ');
    // print_array(lengths, NUM_DIGITS);

    char *w1 = "abcdf";
    char *w2 = "bed";
    printf("%s and %s are equal: %d\n", w1, w2, words_equal(w1, w2, 6));
}

void split_str(char *str, char *splits[], char delim)
{
    int i = 0;
    while(*str != '\0') {
        if (*str == delim) {
            str++;
            splits++;
            i = 0;
            continue;
        }
        *splits[i] = *str;
        str++;
        i++;
    }
}
int decode(char *str, int *lengths, int *outputs)
{    
    for (int i = 0; i < NUM_INPUTS; i++) {
        switch (lengths[i]) {
            case 2:
                outputs[i] = 1;
                break;
            case 3:
                outputs[i] = 7;
                break;
            case 4:
                outputs[i] = 4;
                break;
            case 7:
                outputs[i] = 8;
                break;
            case 5:
                // three different options
            case 6:
                // three different options
            default:
                outputs[i] = -1;
        }
    }
    return 0;
}

int part_2(char datafile[]) {
    // get the data
    FILE *data = fopen(datafile, "r");
    char buffer[MAX_LEN];

    int sum = 0;
    int lengths[NUM_INPUTS];
    int nums[NUM_INPUTS];
    char *splits = malloc(sizeof(char) * NUM_INPUTS * NUM_SEGMENTS);
    while(fgets(buffer, sizeof(buffer), data) != NULL) {
        find_lengths(buffer, lengths, NUM_INPUTS, ' ');
        print_array(lengths, NUM_INPUTS);
        sum += decode(buffer, lengths, nums);
        print_array(nums, NUM_INPUTS);
        split_str(buffer, &splits, ' ');
        //printf("%s\n", *splits[1]);

    }

    fclose(data);
    return sum;
}

int main(int argc, char *argv[])
{
    // number of segments corresponding to 7-segment display:
    // int num_segments[] = { 6, 2, 5, 5, 4, 5, 6, 3, 7, 6 };
    // int numbers       = {  0  1  2  3  4  5  6  7  8  9 };
    printf("Total count for Part I: %d\n", part_1("8test"));
    printf("Total sum for Part II: %d\n", part_2("8test"));


    return 0;
}
