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
//            printf("%c", *str);
            lengths[i]++;
            str++;
        }
//        printf("\n");
    }
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

int in(char letter, char *word, int n)
{
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

int all_segments_found(char *segments)
{
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        if (segments[i] == '\0')
            return 0;
    }
    return 1;
}

int all_words_found(char *words[])
{
    for (int i = 0; i < NUM_INPUTS; i++) {
        if ((char *)words[i] == NULL)
            return 0;
    }
    return 1;
}

int decode(char *str, int *lengths, char *words[], char *segments)
{    
    // loop through first 10 words in str
    // we already know the length of each word
    int pos; // position in *str
    char *word; // current word
    char *str_tmp;
    while(/*!all_segments_found(segments) ||*/ !all_words_found(words)) {
        pos = 0;
        str_tmp = str;
        for (int i = 0; i < NUM_WORDS; i++) { //// only decode words before bar
            word = malloc(lengths[i] * sizeof(char*));
            for (int w = 0; w < lengths[i]; w++) {
                word[w] = *str_tmp++;
            }
            //skip the space
            str_tmp++;
            // printf("%s\n", (char *)word);
            int sol = -1;
            switch (lengths[i]) {
                case 2: // number is 1
                    words[1] = (char *)word;
                    break;
                case 3: // number is 7
                    words[7] = (char *)word;
                    // segments[0] = character in 7 that is not in 1
                    if (words[1] != NULL && segments[0] == '\0') {
                        for (int j = 0; j < 3; j++) {
                            if (!(in(words[7][j], words[1], 3))) {
                                segments[0] = words[7][j];
                                break;
                            }
                        }
                    }
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
                            if (!in(words[1][j], word, lengths[i])) {
                                sol = -1;
                                break; 
                            }
                        }
                        if (sol == 1) {
                            words[3] = (char *)word;
                            //  -> segments[5] = character in words[4] that is not in word(3)
                            for (int j = 0; j < 4; j++) {
                                if (!in(words[4][j], word, lengths[i]) && segments[5] == '\0') {
                                    segments[5] = words[4][j];
                                    break;
                                }
                            }
                            //  -> segments[6] = character in words[4] that is not segments[5]
                            for (int j = 0; j < 4; j++) {
                                if (in(words[4][j], word, lengths[i]) && segments[5] != words[4][j]
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
                        if (in(segments[5], word, lengths[i]) && in(segments[6], word, lengths[i])) {
                            words[5] = (char *)word;
                            //  -> segments[2] = character not in word(5) that is in words[1]
                            for (int j = 0; j < 2; j++) {
                                if(!in(words[1][j], word, lengths[i]) && segments[1] == '\0') {
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
                            for (int j = 0; j < lengths[i]; j++) {
                                if(!in(words[2][j], words[5], lengths[i]) && words[2][j] != segments[1]
                                        && segments[4] == '\0') {
                                    segments[4] = (char)word[j];
                                    break;
                                }
                            }
                            //  -> segments[3] = character in word(2) that is not in words[4] and words[7] and
                            //      is not segments[4]
                            for (int j = 0; j < lengths[i]; j++) {
                                if(!in(word[j], words[4], lengths[i]) && !in(word[j], words[7], lengths[i])
                                        && word[j] != segments[4]) {
                                    segments[3] = word[j];
                                    break;
                                }
                            }
                        }
                    }
                    break;
                case 6: // number is 6, 9, or 0
                    // if all segments but last filled -> 0
                    if (segments[6] != '\0' && segments[1] != '\0') {
                        if (!in(segments[6], word, lengths[i])) {
                            words[0] = (char *)word;
                        } else if (!in(segments[1], word, lengths[i])) {
                            words[6] = (char *)word;
                        } else {
                            words[9] = (char *)word;
                        }
                    }
                    //  -> segments[3] = character not in 4 or 7
                    // if word includes all of 7 but not all of 4 -> 0
                    //  -> number is 0
                    //  -> segments[6] = character
                    break;
                default:
                    ;// outputs[i] = -1;
            }

            if (i >= NUM_INPUTS)
                words[i] = (char *)word;
            // next position in *str is length of last word +1 for space
            pos += lengths[i];
            word = NULL;
            free(word);
        }
        /*for (int i = 0; i < NUM_SEGMENTS; i++) */
            /*printf("%c", segments[i] == '\0' ? '_' : segments[i]);*/
        /*printf("\n");*/
    }
    return 0;
}

int pwr(int x, int y) {
    for (int i = 0; i < y; i++)
        x *= 10;
    return x;
}

int find_duplicates(char *words[], int *num_segments)
{
    int dupes = 0;
    for (int i = 0; i < NUM_INPUTS; i++) {
        for (int j = 0; j < NUM_INPUTS; j++) {
            if (i != j && words_equal(words[i], words[j], num_segments[i])
                    && num_segments[i] == num_segments[j]) {
                printf("Ya done fucked up bro.\n");
                dupes = 1;
            }
        }
    }
    return dupes;
}
int main(int argc, char *argv[])
{
    // number of segments corresponding to 7-segment display:
    int num_segments[] = { 6, 2, 5, 5, 4, 5, 6, 3, 7, 6 };
    // int numbers       = {  0  1  2  3  4  5  6  7  8  9 };
    char datafile[] = "8data";
    FILE *data = fopen(datafile, "r");
    char buffer[MAX_LEN];


    // lengths of words, in order of appearance
    int *lengths = malloc(NUM_WORDS * sizeof(int));

    // words, to be ordered by number value
    char **words = NULL;

    // segments, to be ordered by standard 7-segment definition
    char segments[NUM_SEGMENTS];
    memset(segments, '\0', NUM_SEGMENTS * sizeof(char));

    int nums_to_count[NUM_OUTPUTS] = { 2, 3, 4, 7 };
    int part_1_count = 0;
    int part_2_sum = 0;
    while(fgets(buffer, sizeof(buffer), data) != NULL) {
        words = malloc(NUM_WORDS * sizeof(char *));
        find_lengths(buffer, lengths);
        // print_array(lengths, NUM_WORDS);

        decode(buffer, lengths, words, segments);
        if (find_duplicates(words, num_segments)) {
            print_words(words, lengths, NUM_WORDS);
            printf("%s", buffer);
        }

        // part 1 solution
        int power = 0;
        for (int i = NUM_INPUTS + 1; i < NUM_WORDS; i++) { // start after |
            power = NUM_WORDS - i - 1;
            if (is_in(lengths[i], nums_to_count, NUM_OUTPUTS))
                part_1_count++;
            for (int j = 0; j < NUM_INPUTS; j++) {
                if (lengths[i] == num_segments[j] && words_equal(words[j], words[i], lengths[i])) {
                    //printf("%s =? %s\n", words[i], words[j]);
                    part_2_sum += pwr(j, power);
                    // printf("%d * 10^%d = %d\n",j,power,pwr(j,power));
                }
            }
        }
        // clear segments
        //printf("Segments found: ");
        for (int i = 0; i < NUM_SEGMENTS; i++) {
        //    printf("%c", segments[i] == '\0' ? '_' : segments[i]);
            segments[i] = '\0';
        }
        //printf("\n");
        // clear words
        for (int i = 0; i < NUM_INPUTS; i++) {
            words[i] = NULL;
            free(words[i]);
        }
        words = NULL;
        free(words);
    }

    printf("Total count for Part I: %d\n", part_1_count);

    printf("Total sum for Part II: %d\n", part_2_sum);


    return 0;
}

/*void tests() */
/*{*/
    /*char *test_str = "be cfbegad cbdgef fgaecd cgeb fdcge agebfd fecdb fabcd edb | fdgacbe cefdb cefbgd gcbe";*/
    /*char *bar = find_char(test_str, '|');*/
    /*int num_chars = bar - test_str;*/
    /*if (bar != NULL) {*/
        /*printf("The bar was found at %p, value %c, %d chars from start.\n", &bar, *bar, num_chars);*/
    /*} else {*/
        /*printf("The character was not found.\n");*/
    /*}*/

    /*int *lengths = malloc(NUM_WORDS * sizeof(int));*/
    /*find_lengths(bar, lengths);*/
    /*// print_array(lengths, NUM_DIGITS);*/

    /*char *w1 = "abcdf";*/
    /*char *w2 = "bed";*/
    /*printf("%s and %s are equal: %d\n", w1, w2, words_equal(w1, w2, 6));*/
/*}*/

/*int part_2(char datafile[]) */
/*{*/
    /*// get the data*/
    /*FILE *data = fopen(datafile, "r");*/
    /*char buffer[MAX_LEN];*/

    /*int sum = 0;*/
    /*int lengths[NUM_INPUTS];*/
    /*int nums[NUM_INPUTS];*/
    /*char *splits = malloc(sizeof(char) * NUM_INPUTS * NUM_SEGMENTS);*/
    /*while(fgets(buffer, sizeof(buffer), data) != NULL) {*/
        /*find_lengths(buffer, lengths, NUM_INPUTS, ' ');*/
        /*print_array(lengths, NUM_INPUTS);*/
        /*sum += decode(buffer, lengths, nums);*/
        /*print_array(nums, NUM_INPUTS);*/
        /*split_str(buffer, &splits, ' ');*/
        /*//printf("%s\n", *splits[1]);*/

    /*}*/

    /*fclose(data);*/
    /*return sum;*/
/*}*/
