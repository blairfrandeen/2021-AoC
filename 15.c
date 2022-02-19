/* Advent of code day 15
   https://adventofcode.com/2021/day/15

Synopsis: Given a 2D risk map, find the path from the
upper left corner to the lower right corner with the lowest
risk. Sum the total risk of that path.
Plan: Take the input buffer using malloc
    Determine number of rows & columns
    Use depth-first search similar to day 12
    Once the first path is established, abort any
    paths that have higher risk.

    Use linked lists this time ;)
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

// #define PRINT_DEBUG

struct CaveMap {
    int num_rows, num_cols;
    uint8_t *risk;
};


struct CaveMap* read_input(char data_file[])
{
    FILE* file_handle = fopen(data_file, "r");
    // check that we opened the file
    if (file_handle == NULL) {
        printf("Error opening %s: %s.\n", data_file, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // get the file size
    fseek(file_handle, 0, SEEK_END);
    size_t file_size = ftell(file_handle);
    rewind(file_handle);

    // allocate and read the input buffer
    char *input_buffer = malloc(file_size * sizeof(char *) + 1);
    size_t bytes_read = fread((void*)input_buffer, sizeof(char), file_size, file_handle);
    if (bytes_read != file_size) {
        printf("Error reading input_buffer. Read %lu, expected %lu.\n", bytes_read, file_size);
        exit(EXIT_FAILURE);
    }
    // add a zero at the end
    input_buffer[file_size] = '\0';

    unsigned buffer_index = 0, risk_index = 0;;
    char current_char;

    // initialize the cave map
    struct CaveMap *cm = malloc(sizeof(struct CaveMap));
    cm->num_rows = cm->num_cols = 0;
    cm->risk = malloc(sizeof(uint8_t) * file_size);

    // assign the risk and number of rows/cols to the map
    while((current_char = input_buffer[buffer_index]) != '\0') {
        if (current_char == '\n' && cm->num_cols == 0)
            cm->num_cols = buffer_index;
        if (current_char != '\n') {
            // convert char to uint8_t
            cm->risk[risk_index] = current_char - '0';
            risk_index++;
        }
        buffer_index++;
    }
    cm->num_rows = risk_index / cm->num_cols;

    // clean up
    free(input_buffer);
    fclose(file_handle);

#ifdef PRINT_DEBUG
    printf("Rows: %d, Cols: %d\n", cm->num_rows, cm->num_cols);
    for (int i = 0; i < cm->num_rows; i++) {
        for (int j = 0; j < cm->num_cols; j++) {
            printf("%u", cm->risk[i * cm->num_cols + j]);
        }
        printf("\n");
    }
#endif
    return cm;
}

int main(int argc, char *argv[]) {
    struct CaveMap *map = read_input("data/15test");

    free(map->risk);
    free(map);
    return (EXIT_SUCCESS);
}
