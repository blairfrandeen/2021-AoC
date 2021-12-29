#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sum_int_array(int target[], int target_size)
// return the sum of an array of integers
{
    int sum = 0;
    int i;
    for (i = 0; i < target_size; i++) {
        sum += target[i];
        //printf("%d\n", target[i]);
    }
    return sum;
}

void print_window(int target[], int size)
// print every element in an array (debug function)
{
    int i;
    for (i = 0; i < size; i ++) {
        printf("%d ", target[i]);
    }
    printf("\n");
}

int depths_sliding(char input_file[], int window_size)
{
    int depth_increases = 0;
    FILE *file_pointer = fopen(input_file, "r");
    char read_buffer[255];
    
    int sliding_window[window_size];

    // fill the sliding_window array with zeros
    memset(sliding_window, 0, window_size * sizeof(int));
    int current_window_sum = 0;
    int prev_window_sum = 0;
    int i;

    while(fgets(read_buffer, sizeof(read_buffer), file_pointer) != NULL) {
        // shift contents of sliding window
        if (window_size > 1) {
            for (i = 0; i < window_size - 1; i++) { 
                sliding_window[i] = sliding_window[i+1];
            }
        }
        // last element in window is next integer in text file.
        sliding_window[i] = atoi(read_buffer);
        if (sliding_window[0] != 0) { // don't start counting until window is full
            current_window_sum = sum_int_array(sliding_window, window_size);
            if (current_window_sum > prev_window_sum && prev_window_sum != 0) {
                depth_increases++;
            }
        }
        prev_window_sum = current_window_sum;

    }
    fclose(file_pointer);
    return depth_increases;
}

int main(int argc, char *argv[])
{
    printf("Increases on sliding window: %d\n", 
            depths_sliding("depths.txt", 7));
    return 0;
}
