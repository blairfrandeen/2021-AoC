#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Work to go:
1) Re-think how the datafile is read, and use getc() instead of gets()
    Go bit by bit, identify if a newline character or EOF is reached.
    Throw an error if anything other than a 1, 0, or \n is found.
2) Work on second half of problem
3) Write a function to convert string of 1s and 0s to int
4) Write a function to square integers
5) Figure out how to get the -lm flag for math.h properly into the makefile
*/

int main(int argc, char *argv[])
{
    // get the data
    char datafile[] = "data/3data";
    FILE *data = fopen(datafile, "r");
    if (data == NULL) {
        printf("error, no data file\n");
        return 1;
    }
    char buffer[255];

    // each number is 5 bits long
    int data_size = 12;
    int sums[data_size];
    memset(sums, 0, data_size * sizeof(int));

    int p; // bit position
    int data_len = 0; // size of the data array
    while(fgets(buffer, sizeof(buffer), data) != NULL) {
        for (p = 0; p < data_size; p++) {
            if (buffer[p] == '1') {
                sums[p]++;
            }
        }
        data_len++;
    }

    // print intermediate output
    int i;
    for (i = 0; i < data_size; i++) {
        printf("%d ", sums[i]);
    }
    printf("\n");

    int gamma = 0;
    int epsilon = 0;
    int place;
    for (i = 0; i < data_size; i++) {
        place = (int)pow(2, data_size - i - 1);
        if ((double)(sums[i]) / (double)(data_len) > 0.5) {
            gamma += place;
        } else {
            epsilon += place;
        }
    }
    printf("gamma: %d epsilon %d\n", gamma, epsilon);
    printf("product: %d\n", gamma * epsilon);
    return 0;
}
