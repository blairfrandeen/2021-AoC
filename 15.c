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

//#define PRINT_DEBUG
#define MAX_PATH_LEN 1027

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
//    /*
    for (int num_sorted = 0; num_sorted < num_neighbors; num_sorted++) {
        for (int i = num_neighbors - 1; i > num_sorted; i--) {
            int current = map->risk[p_neighbors[i]];
            int prev = map->risk[p_neighbors[i-1]];
            if (current < prev) {
                int tmp = p_neighbors[i];
                p_neighbors[i] = p_neighbors[i - 1];
                p_neighbors[i - 1] = tmp;
            } else if (current == prev && p_neighbors[i] > p_neighbors[i-1]) {
                int tmp = p_neighbors[i];
                p_neighbors[i] = p_neighbors[i - 1];
                p_neighbors[i - 1] = tmp;
            }
        }
    }
    //*/

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
    //for (int i = 0; i < p_path->length; i++) {
    for (int i = p_path->length - 1; i >= 0; i--) {
        if (index == p_path->indices[i])
            return 1;
    }
    return 0;
}

void print_path(Path *p, struct CaveMap *map)
{
    printf("[ ");
    for (int i = 0; i < p->length; i++) {
        printf("%d ", map->risk[p->indices[i]]);
    }
    printf("]\n");
}

void print_path_in_matrix(Path *p, struct  CaveMap *map)
{
    for (int i = 0; i < map->end_index + 1; i++) {
        if ((i % map->num_cols) == 0 && i != 0) { printf("\n"); }
        if (in_path(i, p)) {
            printf("%d", map->risk[i]);
        } else {
            printf("_");
        }
    }
    printf("\n\n");
}
/*
* DFS search algorithm to find lowest risk path through cave system
*
* @param    start_index     index to start on
* @param    p_map           pointer to cavemap
* @param    p_path          path we're currently on
* @param    min_risk        minimum path risk
*/
void find_lowest_risk(int start_index, struct CaveMap *p_map, Path *p_path,
        int *min_risk, int *num_paths)
{
    //printf("Start index: %d, path_length: %d, path_risk: %d, min_risk: %d\n",
            //start_index, p_path->length, p_path->risk, *min_risk);
    if (start_index != 0) 
        p_path->risk += p_map->risk[start_index];
    if (start_index != p_map->end_index) {
        int neighbors[4];
        int num_neighbors = find_neighbors(p_map, start_index, neighbors);
        for (int i = 0; i < num_neighbors; i++) {
            if (in_path(neighbors[i], p_path)) { continue; }
            uint8_t next_risk = p_map->risk[neighbors[i]];
            if ((next_risk + p_path->risk) >= *min_risk) { continue; }
            p_path->indices[p_path->length++] = neighbors[i];
            find_lowest_risk(neighbors[i], p_map, p_path, min_risk, num_paths);
            //printf("Recurse.\n");
            p_path->length -= 1;
            p_path->risk -= p_map->risk[p_path->indices[p_path->length]];
        }
    } else {
        printf("Made it to end. Risk = %d!\n", p_path->risk);
        print_path_in_matrix(p_path, p_map);
        (*num_paths)++;
        //if (*num_paths % 10 == 0) {
            //printf("Found %d paths. Minimum Risk: %d\n", *num_paths, *min_risk);
        //}
        if (p_path->risk < *min_risk) {
            *min_risk = p_path->risk;
        }
    }
}

int main(int argc, char *argv[]) {
    struct CaveMap *map = read_input("data/15data");
    //struct CaveMap *map = read_input("data/15test");
    Path *path = malloc(sizeof(Path));
    path->risk = 0;
    path->length = 1;
    path->indices[0] = 0;
    int num_paths = 0;
    int min_risk = 100000; // arbitrarily high
    find_lowest_risk(0, map, path, &min_risk, &num_paths);
    printf("Minimum risk: %d\n", min_risk);
    printf("Examined %d paths.\n", num_paths);

#ifdef PRINT_DEBUG
    int neighbors[4];
    int num_neighbors = find_neighbors(map, 21, neighbors);
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
