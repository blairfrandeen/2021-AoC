/** Advent of Code Day 9
* https://adventofcode.com/2021/day/9
* Synopsis: Given a heightmap corresponding to a cave floor
*    find the low points. Heights are digits 1-9
*    A low point is a point where all digits adjacent to it
*    (up, down, left, right) are higher.
*
* Part 1: Find the total risk level:
*    Risk level for a given low point is 1 plus its height
*    Find all low points, their corresponding risk level, and sum them
* Part 2: Find the product of the size of the 3 largest basins
*   A basin is all locations that flow down to a single low point
*   Locations of height 9 do not count, and thus surround basins
*   A basin's size is the number of locations within it, including the low point
*
* PART 1 PLAN:
*   - Read the input file, determine number of rows & cols
*   - Create an m x n matrix
*   - Loop through each element of the matrix and check
*   - Whether or now it's a low point.
* PART 2 PLAN:
*   - Modify the heightmap struct to include an int array of low-points
*       (get this to work for part 1 before continuing)
*   - This is a recursive search problem, I think.
*   - Starting at a low point, make a list of other points to search
*       (not 9, not off the map)
*   - Search each point, continue adding to list
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include "util.h"

#define MAX_NEIGHBORS 4 // maximum neighboring points on a heightmap (no diagonals)

struct Heightmap {
    unsigned num_rows;
    unsigned num_cols;
    unsigned num_elements;
    uint8_t *heights; // array of heights
    unsigned num_low_points;
    unsigned risk;
};

/*
* Determine the risk level for a given low point
*
* @param    low_point_height    height of low point
* @retval   low_point_height +1
*/
int low_point_risk(int low_point_height)
{
    return low_point_height + 1;
}

/*
* Create a heightmap given an input file.
* - Open the file
* - Determine the number of rows
* - Determine the number of columns
* - Allocate a pointer for the heights
* - Set risk to zero
* @param    file_handle     input file to read
* @retval   map             pointer to height map
*/
struct Heightmap *Heightmap_create(char *file_name)
{
    FILE* input_file_handle = fopen(file_name, "r");
    if (input_file_handle == NULL) {
        printf("Error: reading input file %s: %s\n", file_name, strerror(errno));
        exit(-1);
    }

    // allocate memory for the height map object
    struct Heightmap *map = malloc(sizeof(struct Heightmap));

    // search for the first newline to get the row size
    while (fgetc(input_file_handle) != '\n');
    size_t row_size = ftell(input_file_handle) - 1; // ignore the newline

    fseek(input_file_handle, 0, SEEK_END);
    size_t file_size = ftell(input_file_handle);
    rewind(input_file_handle);

    printf("Reading file %s...\n", file_name);
    printf("File size: %lu bytes\n", file_size);
    printf("Row size: %lu bytes\n", row_size);

    map->num_cols = row_size / sizeof(char);
    map->num_rows = file_size / (row_size + 1); // add 1 to rowsize to count newlines
    map->num_elements = map->num_cols * map->num_rows;

    // allocate array for heightmap
    map->heights = (uint8_t *)malloc(map->num_elements * sizeof(uint8_t));

    for (int height_index = 0; height_index < map->num_elements; height_index++) {
        uint8_t next_height = fgetc(input_file_handle) - '0'; 
        if (next_height >= 0 && next_height <= 9) {
            map->heights[height_index] = next_height;
        } else {
            height_index--;
            continue;
        }
    }

    fclose(input_file_handle);
    map->risk = 0; // initialize risk
    map->num_low_points = 0; // initialize low points

    return map;
}

/*
* Free the memory associated with a Heightmap
*/
void Heightmap_destroy(struct Heightmap *map)
{
    free(map->heights);
}

/* 
* Given row and column coordinates, return the index from a given heightmap.
*
* @param        map         pointer to Heightmap to query
* @param        row         row in Heightmap
* @param        col         column in Heightmap
* @retval                   index in 1d heightmap
*/
int rc_index(struct Heightmap *map, int row, int col)
{
    if (map == NULL) {
        printf("Error: Bad access to heightmap.\n");
        exit(-1);
    }
    if (row > map->num_rows) {
        printf("Error: Tried to access map row %d of %d!\n", row, map->num_rows);
        exit(-1);
    }
    if (col > map->num_cols) {
        printf("Error: Tried to access map col %d of %d!\n", col, map->num_cols);
        exit(-1);
    }
    return (row * map->num_cols + col);
}

/*
* Given an index of a given heightmap, return the row index.
*
* @param        map         pointer to heightmap to query
* @param        index       index to query
* @retval                   row index of point.
*/
int row_index(struct Heightmap *map, int index)
{
    if (map == NULL) {
        printf("Error: Bad access to heightmap.\n");
        exit(-1);
    }
    if (index > map->num_elements) {
        printf("Error: Tried to access map index %d of %d!\n", index, map->num_elements);
        exit(-1);
    }
    return (int)(index / map->num_cols);
}

/*
* Given an index of a given heightmap, return the col index.
*
* @param        map         pointer to heightmap to query
* @param        index       index to query
* @retval                   col index of point.
*/
int col_index (struct Heightmap *map, int index)
{
    if (map == NULL) {
        printf("Error: Bad access to heightmap.\n");
        exit(-1);
    }
    if (index > map->num_elements) {
        printf("Error: Tried to access map index %d of %d!\n", index, map->num_elements);
        exit(-1);
    }
    return (int)(index % map->num_cols);
}

/*
* Given a heightmap and an index, identify neighboring indices
* Neighboring being in the next or previous row or
* or next or previous column
* 
* @param    map             heightmap to search
* @param    index           index within heightmap
* @param    neighbors       array to populate with neighboring indices
*                           an index of indicates edge of map
* @retval   1               error
* @retval   0               success
*/
int Heightmap_neighbors(struct Heightmap *map, int index, int *neighbors)
{
    if (map == NULL || neighbors == NULL) {
        printf("Error: Bad access to heightmap.\n");
        return 1;
    }
    if (index < 0 || index > map->num_elements) {
        printf("Error: Index out of bounds. Attempted to access element %d of %d.\n",
                index, map->num_elements);
        return 1;
    }
    
    int row = row_index(map, index);
    int col = col_index(map, index);

    // check above
    if (row == 0) {
        neighbors[0] = -1;
    } else {
        neighbors[0] = rc_index(map, row - 1, col);
    }

    // check in front
    if (col == (map->num_cols - 1)) {
        neighbors[1] = -1;
    } else {
        neighbors[1] = rc_index(map, row, col + 1);
    }

    // check below
    if (row == (map->num_rows - 1)) {
        neighbors[2] = -1;
    } else {
        neighbors[2] = rc_index(map, row + 1, col);
    }

    // check behind
    if (col == 0) {
        neighbors[3] = -1;
    } else {
        neighbors[3] = rc_index(map, row, col - 1);
    }

    return 0; // success
}

/*
* Determine if a point in a given heightmap at row,col
* coordinates is a low point
* 
* @param    map         height map to look in
* @param    index       index of point to query
* @retval   0           point is not a low point
* @retval   1           point is a low point
*/
int is_low_point(struct Heightmap *map, int index)
{
    int neighbors[MAX_NEIGHBORS];

    if (Heightmap_neighbors(map, index, neighbors)) {
        printf("Error in Heightmap_neighbors.\n");
        exit(-1);
    }

    for (int neighbor_index = 0; neighbor_index < MAX_NEIGHBORS; neighbor_index++) {
        if (neighbors[neighbor_index] == - 1) { // no neighbor to check
            continue;
        }
        if (map->heights[index] >= map->heights[neighbors[neighbor_index]]) // not a low point
            return 0;
    }

    /* DEBUG
    printf("LP @ Index %d (%d, %d). LP = %d, Neighbors = ", index, row_index(map, index),
            col_index(map,index), map->heights[index]);
    for (int i = 0; i < 4; i++)
        printf("%d ", map->heights[neighbors[i]]);

    print_array(neighbors, MAX_NEIGHBORS);
    */
    return 1; // default if no other points lower
}

/*
* Look through a height map and determine the number of lowpoints
* Assign a risk to the map based on the risk function.
*
* @param    map             height map to evaluate
* @retval   1               error reading heightmap
* @retval   0               low points counted and assigned to map
*/
int count_low_points(struct Heightmap *map)
{
    if (map == NULL) {
        printf("Error: bad map pointer.\n");
        return 1;
    }
    for (int map_index = 0; map_index < map->num_elements; map_index++) {
        if (is_low_point(map, map_index)) {
            map->num_low_points++;
        }
    }
    
    return 0; // success
}

/*
* Evaluate the total risk rating for a Heightmap
*
* @param    map      heightmap to evaluate
* @retval   1        error reading heightmap
* @retval   0        risk evaluated and assigned to map
*/
int evaluate_risk(struct Heightmap *map)
{
    if (map == NULL) {
        printf("Error: bad map pointer.\n");
        return 1;
    }

    for (int map_index = 0; map_index < map->num_elements; map_index++) {
        if (is_low_point(map, map_index)) {
            map->risk += low_point_risk(map->heights[map_index]);
        }
    }

    return 0; // risk evaluated successfully
}

/*
* Print information about a height map
* @param    map     map to print information about
*/
void Heightmap_info(struct Heightmap *map)
{
    if (map == NULL) 
        printf("Error: heightmap not found.\n");
    printf("-------\n");
    printf("Heightmap is %u rows x %u cols.\n", map->num_rows, map->num_cols);
    if (!count_low_points(map))
        printf("Number of low points: %d\n", map->num_low_points);
    if (!evaluate_risk(map))
        printf("RISK RATING: %u\n", map->risk);
}

/*
* Sort an array in descending order (largest element first)
*
* @param        array       the array to sort
* @param        n           number of elements in array
* @param        n_to_sort   number of elements to sort
*                           set equal to n to sort entire array
* @retval       0           sort successful
* @retval       1           sort failure
*/
int array_sort_descending(int *array, int n, int n_to_sort)
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
        int tmp = array[num_sorted];
        array[num_sorted] = array[max_index];
        array[max_index] = tmp;
        num_sorted++;
    }

    return 0; // success
}

// look through entire heightmap for low points.
// if a low point is found, begin building the basin
// initialize an array 
// is_low_point can be generalized to:
//  from this index, populate an array of indices, up to 4, to search in
//  then for each of those indices, check if the value is lower
int main(int argc, char *argv[])
{
    char test_file[] = "9test";
    char data_file[] = "9data";

    struct Heightmap *test_map = Heightmap_create(test_file);
    struct Heightmap *data_map = Heightmap_create(data_file);

    Heightmap_info(test_map);
    Heightmap_info(data_map);

    Heightmap_destroy(test_map);
    Heightmap_destroy(data_map);
    free(test_map);
    free(data_map);

    return 0;
}

void tests() {
    int arr[] = { -77, 3, 28, 1, 9, 0, -1, 18, 3232 };
    print_array(arr, 9);
    array_sort_descending(arr, 9, 3);
    print_array(arr, 9);
    int product_three_largest = arr[0] * arr[1] * arr[2];
    printf("Product of 3 largest = %d\n", product_three_largest);
}
