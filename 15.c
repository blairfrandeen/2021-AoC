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

#define PRINT_DEBUG
#define MAX_PATH_LEN 1024

struct CaveMap {
    int num_rows, num_cols;
    int end_index;
    uint8_t *risk;
};

typedef struct Path {
    int risk;
    int length;
    int indices[MAX_PATH_LEN];
} Path;

/* 
* Read input from a data file and build a cave map.
*
* @param    data_file       The data file to read
* @retval   cm              Cave map
*/
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
    cm->end_index = cm->num_rows * cm->num_cols - 1;

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

/*
* Find the neighboring points given an index in a cavemap.
*
* @param    map             The cave map to look in
* @param    index           The index in the map->risk array
* @param    p_neighbors     Pointer to the arry of neighbors to populate
                            (pass by reference)
* @retval   num_neighbors   number of neighbors found
*/
int find_neighbors(struct CaveMap *map, int index, int *p_neighbors)
{
    if (index > map->end_index) {
        printf("Error: Index %d in find_neighbors out of bounds (%d).\n",
                index, map->end_index);
        exit(EXIT_FAILURE);
    }
    if (map == NULL || p_neighbors == NULL) {
        printf("Error: Null pointer passed to find_neighbors.\n");
        exit(EXIT_FAILURE);
    }
    int num_neighbors = 0;
    int row = index / map->num_cols;
    int col = index % map->num_cols;

    // we check the next column & next row first, since
    // these are more likely to be the direction we want
    // to go to get to the end at low risk
    if (col != map->num_cols - 1) { // check next column
        p_neighbors[num_neighbors] = index + 1;
        num_neighbors++;
    }
    if (row != map->num_rows - 1) { // check next row
        p_neighbors[num_neighbors] = index + map->num_cols;
        num_neighbors++;
    }
    if (row != 0) { // check previous row
        p_neighbors[num_neighbors] = index - map->num_cols;
        num_neighbors++;
    }
    if (col != 0) { // check previous column
        p_neighbors[num_neighbors] = index - 1;
        num_neighbors++;
    }

    return num_neighbors;
}

/*
* Determine the risk in a given path
* 
* @param    p_path_t    pointer to path to evaluate
* @retval   risk        risk as evaluated
*/
int path_risk(Path *p_path, struct CaveMap *p_map)
{
    p_path->risk = 0;
    // start path at 1, since the 0th index is the starting
    // position, and does not count towards total risk
    for(int index = 1; index < p_path->length; index++) {
        p_path->risk += p_map->risk[index];
    }
    return p_path->risk;
}

/*
* Determine whether an index is already in a path
*
* @param    index       the index to search for
* @param    p_path      The path to search in
* @retval   0           Not in path
* @retval   1           In path
*/
int in_path(int index, Path *p_path)
{
    for (int i = 0; i < p_path->length; i++) {
        if (index == p_path->indices[i])
            return 1;
    }
    return 0;
}
/*
* DFS search algorithm to find lowest risk path through cave system
*
* @param    start_index     index to start on
* @param    p_map           pointer to cavemap
* @param    p_path          path we're currently on
* @retval   min_risk        minimum risk
*/
int find_lowest_risk(int start_index, struct CaveMap *p_map, Path *p_path)
{
    printf("Start index: %d, path_length: %d, risk: %d\n",
            start_index, p_path->length, p_path->risk);
    if (start_index != p_map->end_index) {
        p_path->risk += p_map->risk[start_index];
        int neighbors[4];
        int num_neighbors = find_neighbors(p_map, start_index, neighbors);
        for (int i = 0; i < num_neighbors; i++) {
            if (in_path(neighbors[i], p_path)) { continue; }
            p_path->indices[p_path->length] = neighbors[i];
            p_path->length++;
            find_lowest_risk(neighbors[i], p_map, p_path);
        }
        p_path->length = 0;
        p_path->risk -= p_map->risk[start_index];
    }
    return p_path->risk;
}

int main(int argc, char *argv[]) {
    struct CaveMap *map = read_input("data/15test");
    Path *path = malloc(sizeof(Path));
    path->risk = 0;
    path->length = 0;
    int min_risk = find_lowest_risk(0, map, path);
    printf("Minimum risk: %d\n", min_risk);

#ifdef PRINT_DEBUG
    int neighbors[4];
    int num_neighbors = find_neighbors(map, 11, neighbors);
    printf("%d neighbors:\n", num_neighbors);
    for (int i = 0; i < num_neighbors; i++) {
        printf("%d ", map->risk[neighbors[i]]);
    }
    printf("\n");
#endif
    free(map->risk);
    free(map);
    return (EXIT_SUCCESS);
}
