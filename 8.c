/* Synopsis: 7-segment display has all the wires crossed, and we need to uncross them. */
/* SEVEN SEGMENT DISPLAY DEFINITION
*    AAA
*   F   B
*   F   B
*    GGG
*   E   C
*   E   C
*    DDD 
*/

#include <stdio.h>
#include "util.h"
#include <stdlib.h>
#include <string.h>

#define NUM_SEGMENTS 7
#define NUM_WORDS 15 /* 15 total words, including "|" */
#define NUM_INPUTS 10
#define NUM_OUTPUTS 4
#define MAX_LEN 256

/* Part I Plan:
   -> How many times do digits in the output
   (after the |) that are identifiable appear?
   -> Identifiable digits are 1, 4, 7, and 8, which use 2, 4, 3, and 7 segments, respectively

   - Use fgets() to grab each line of input
   - ASSUME:
    - All lines are of the same length
    - All lines have 10 words prior to the `|`, 4 words following the `|`
    - All input is valid.
   - Create an array of 14 ints that corresponds to the lengths of each word.
   - Write a function that counts the occurences of lengths 1, 4, 7, 8 in final 4 words.
*/

/*  Part II Plan:
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

    - Brute force applied:
    be cfbegad cbdgef fgaecd cgeb fdcge agebfd fecdb fabcd edb
    1) B & C are b or e
    2) A = d
    3) F & G are c or g
    4) fdgce is 5
        D = f
        C = e
        B = b
    5) fecdb = 3
        G = c
        F = g
        E = a
    segment string should be: dbefagc
    I'm getting:              debfbgc
    Wrong answers:             ^^ ^

*/

int words_equal(char *w1, char *w2, int n) 
{
    int match_count = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (w1[i] == w2[j])
                match_count++;
        }
    }
    return (match_count == n);
}

void find_lengths(char *str, int *lengths)
{
    for (int i = 0; i < NUM_WORDS; i++) {
        lengths[i] = 0;
        while(*str != '\0') {
            if (*str == ' ' || *str == '\n') {
                str++;
                break;
            }
            // printf("%c", *str);
            lengths[i]++;
            str++;
        }
        // printf("\n");
    }
}

int in(char letter, char *word, int n)
{
    if (word == NULL)
        return 0;
    for (int i = 0; i < n; i++) {
        if (letter == *(word + i))
            return 1;
    }
    return 0;
}

void print_words(char *words[], int *lengths, int n)
{
    for (int i = 0; i < n; i++) {
        printf("%s - ", (char *)words[i]);
    }
    printf("\n");
}

int all_words_found(char *words[])
{
    for (int i = 0; i < NUM_INPUTS; i++) {
        if ((char *)words[i] == NULL)
            return 0;
    }
    return 1;
}

// return the first segment FROM a string 
// of length l_f that is NOT_IN another string of length l_n
char *segment_missing_from(char **from, int l_f, char **not_in, int l_n)
{
    if (from == NULL || not_in == NULL)
        return '\0';
    for (int i = 0; i < l_f; i++) {
        if (!in(*from[i], (char *)not_in, l_n))
            return from[i];
    }
    return '\0';
}

int decode(char *str, int *lengths, char **words)
{    
    // loop through first 10 words in str
    // we already know the length of each word
    char *word; // current word
    char *str_tmp; // our position in the input string
    char segments[NUM_SEGMENTS];
    memset(segments, '\0', NUM_SEGMENTS * sizeof(char));

    while(!all_words_found(words)) {
        str_tmp = str;
        for (int i = 0; i < NUM_WORDS; i++) { 
            int len = lengths[i]; // refactor for easier to read code

            // assign the word to be analyzed
            word = malloc(len * sizeof(char*));
            memset(word, '\0', len * sizeof(char));
            for (int w = 0; w < len; w++) 
                word[w] = *str_tmp++;

            int sol = -1; // needed for logic of case 5
            switch (len) {
                case 2: // number is 1
                    words[1] = (char *)word;
                    break;
                case 3: // number is 7
                    words[7] = (char *)word;
                    // segments[0] = character in 7 that is not in 1
                    segments[0] = *segment_missing_from(&words[7], 3, &words[1], 2);
                    /*if (words[1] != NULL && segments[0] == '\0') {*/
                        /*for (int j = 0; j < 3; j++) {*/
                            /*if (!(in(words[7][j], words[1], 3))) {*/
                                /*segments[0] = words[7][j];*/
                                /*break;*/
                            /*}*/
                        /*}*/
                    /*}*/
                    break;
                case 4: // number is 4
                    words[4] = (char *)word;
                    break;
                case 7: // number is 8
                    words[8] = (char *)word;
                    break;
                case 5: // number is 2, 3, or 5
                    // if word includes all of words[1] -> 3
                    if (words[1] != NULL && words[4] != NULL) {
                        sol = 1;
                        for (int j = 0; j < 2; j++) {
                            if (!in(words[1][j], word, len)) {
                                sol = -1;
                                break; 
                            }
                        }
                        if (sol == 1) {
                            words[3] = (char *)word;
                            //  -> segments[5] = character in words[4] that is not in word(3)
                            for (int j = 0; j < 4; j++) {
                                if (!in(words[4][j], word, len) && segments[5] == '\0') {
                                    segments[5] = words[4][j];
                                    break;
                                }
                            }
                            //  -> segments[6] = character in words[4] that is not segments[5]
                            for (int j = 0; j < 4; j++) {
                                if (in(words[4][j], word, len) && segments[5] != words[4][j]
                                        && segments[6] == '\0') {
                                    // and not in words[1]
                                    if (!in(words[4][j], words[1], 2)) {
                                        segments[6] = words[4][j];
                                        break;
                                    }
                                    
                                }
                            }
                        }
                    }

                    // if word includes segments[5] and [6] -> 5
                    if (segments[5] != '\0' && segments[6] != '\0' && sol == -1) {
                        if (in(segments[5], word, len) && in(segments[6], word, len)) {
                            words[5] = (char *)word;
                            //  -> segments[2] = character not in word(5) that is in words[1]
                            for (int j = 0; j < 2; j++) {
                                if(!in(words[1][j], word, len) && segments[1] == '\0') {
                                    segments[1] = words[1][j];
                                    //  -> segments[1] = character in words[1] that is not segments[2]
                                    if (j == 0) {
                                        segments[2] = words[1][1];
                                    } else {
                                        segments[2] = words[1][0];
                                    }
                                    break;
                                }
                            }
                        } else { // else -> 2
                            words[2] = (char *)word;
                            //  -> segments[4] = character in word(2) that is not in words[5] and not segments[1]
                            if (words[7] == NULL || words[5] == NULL || segments[1] == '\0')
                                break;
                            for (int j = 0; j < len; j++) {
                                if(!in(words[2][j], words[5], len) && words[2][j] != segments[1]
                                        && segments[4] == '\0') {
                                    segments[4] = (char)word[j];
                                    break;
                                }
                            }
                            //  -> segments[3] = character in word(2) that is not in words[4] and words[7] and
                            //      is not segments[4]
                            for (int j = 0; j < len; j++) {
                                if(!in(word[j], words[4], len) && !in(word[j], words[7], len)
                                        && word[j] != segments[4]) {
                                    segments[3] = word[j];
                                    break;
                                }
                            }
                        }
                    }
                    break;
                case 6: // number is 6, 9, or 0
                    // check that we have all information to execute conditional statement
                    if (segments[6] != '\0' && segments[1] != '\0') {
                        // if all segments but last filled -> 0
                        if (!in(segments[6], word, len)) {
                            words[0] = (char *)word;
                        // if 6 segments but doesn't include all of 1 -> 9
                        } else if (!in(segments[1], word, len)) {
                            words[6] = (char *)word;
                        } else {
                            words[9] = (char *)word;
                        }
                    }
                    break;
                default:
                    ; // do nothing
            }

            // assign the last 4 words as they are given
            if (i >= NUM_INPUTS)
                words[i] = (char *)word;

            // cleanup
            word = NULL;
            free(word);

            // advance the pointer past the space
            str_tmp++;
        }

        // handy debug for printing the segments that have been found
        /*for (int i = 0; i < NUM_SEGMENTS; i++) */
            /*printf("%c", segments[i] == '\0' ? '_' : segments[i]);*/
        /*printf("\n");*/
    }
    return 0;
}

int pwr(int x, int y)
{
// raise int x to the power of y
// good candidate for util.h
    for (int i = 0; i < y; i++)
        x *= 10;
    return x;
}

int find_duplicates(char *words[], int *num_segments)
{
    int dupes = 0;
    for (int i = 0; i < NUM_INPUTS; i++) {
        for (int j = 0; j < NUM_INPUTS; j++) {
            if (num_segments[i] == num_segments[j] && i != j
                    && words_equal(words[i], words[j], num_segments[i])) {
                printf("ERROR: Duplicate word found.\n");
                dupes = 1;
            }
        }
    }
    return dupes;
}

int main(int argc, char *argv[])
{
    // number of segments corresponding to 7-segment display:
    int num_segments[] =       { 6, 2, 5, 5, 4, 5, 6, 3, 7, 6 };
    // corresponding digits:= {  0  1  2  3  4  5  6  7  8  9 };
    char datafile[] = "8data";
    if (argc > 1 && *argv[1] == 't') {
        printf("Test Mode!\n");
        strcpy(datafile, "8test");
    }
    FILE *data = fopen(datafile, "r");
    char buffer[MAX_LEN];

    // lengths of words, in order of appearance
    int *lengths = malloc(NUM_WORDS * sizeof(int));

    // words, to be ordered by number value
    char **words = NULL;

    int nums_to_count[NUM_OUTPUTS] = { 2, 3, 4, 7 };
    int part_1_count = 0;
    int part_2_sum = 0;
    while(fgets(buffer, sizeof(buffer), data) != NULL) {
        words = malloc(NUM_WORDS * sizeof(char *));
        memset(words, 0, NUM_WORDS * sizeof(char *));
        find_lengths(buffer, lengths);
        // print_array(lengths, NUM_WORDS);

        decode(buffer, lengths, words);
        if (find_duplicates(words, num_segments)) {
            print_words(words, lengths, NUM_WORDS);
            printf("%s", buffer);
        }

        int power = 0;
        for (int i = NUM_INPUTS + 1; i < NUM_WORDS; i++) { // start after |
            // part 1 solution
            if (is_in(lengths[i], nums_to_count, NUM_OUTPUTS))
                part_1_count++;
            for (int j = 0; j < NUM_INPUTS; j++) {
                if (lengths[i] == num_segments[j] && words_equal(words[j], words[i], lengths[i])) {
                    power = NUM_WORDS - i - 1;
                    //printf("%s =? %s\n", words[i], words[j]);
                    part_2_sum += pwr(j, power);
                    // printf("%d * 10^%d = %d\n",j,power,pwr(j,power));
                }
            }
        }
    }
    // clear words
    /*for (int i = 0; i < NUM_WORDS; i++) {*/
        /*words[i] = NULL;*/
        /*free(words[i]);*/
    /*}*/
    words = NULL;
    free(words);

    printf("Total count for Part I: %d\n", part_1_count);

    printf("Total sum for Part II: %d\n", part_2_sum);

    // clean up
    lengths = NULL;
    free(lengths);

    return 0;
}
