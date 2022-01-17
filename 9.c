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
    unsigned basin_score;
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
*
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

    // populate the heightmap
    int height_index = 0;
    while (height_index < map->num_elements) {
        uint8_t next_height = fgetc(input_file_handle) - '0'; 
        // skip anything that's not 0 thru 9
        if (next_height >= 0 && next_height <= 9) {
            map->heights[height_index] = next_height;
            height_index++;
        }
    }

    fclose(input_file_handle);
    map->risk = 0; // initialize risk
    map->num_low_points = 0; // initialize low points
    map->basin_score = 0;

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
* @param    num_neighbors   pointer to array size for neighbors
* @retval   1               error with parameters
* @retval   0               succeeded
*/
int Heightmap_neighbors(struct Heightmap *map, int index, int *neighbors, int *num_neighbors)
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
    if (row != 0)  // skip if on top row
        push_unique(neighbors, num_neighbors, rc_index(map, row - 1, col));

    // check in front
    if (col != (map->num_cols - 1))  // skip if last column
        push_unique(neighbors, num_neighbors, rc_index(map, row, col + 1));

    // check below
    if (row != (map->num_rows - 1))  // skip if last row
        push_unique(neighbors, num_neighbors, rc_index(map, row + 1, col));

    // check behind
    if (col != 0)  // skip if first column
        push_unique(neighbors, num_neighbors, rc_index(map, row, col - 1));

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
    int num_neighbors = 0;

    if (Heightmap_neighbors(map, index, neighbors, &num_neighbors)) {
        printf("Error in Heightmap_neighbors called from low_point().\n");
        exit(-1);
    }

    for (int neighbor_index = 0; neighbor_index < num_neighbors; neighbor_index++) {
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

/* 
* Starting at a known low point, determine the size of a basin.
*
* @param        map                 the heightmap to look in
* @param        low_point_index     the index of the low point (starting point)
* @retval       basin_size          the size of the basin
*/
int search_basin(struct Heightmap *map, int low_point_index)
{
    if (map == NULL) 
        printf("Error: heightmap not found.\n");
    int basin[map->num_elements];               // worst case, basin is entire map
    basin[0] = low_point_index;                 // first point in basin is always the low point
    int basin_size = 1;                         // low_point counts as part of basin

    int points_to_search[map->num_elements];    // worst case, need to search entire map
    int num_to_search = 0;                      // number of points left to search
    if (Heightmap_neighbors(map, low_point_index, points_to_search, &num_to_search)) {
        printf("Error with Heightmap_neighbors() called in search_basin().\n");
        exit(-1);
    }
    while(num_to_search > 0) { // keep going until we've exhausted our options
        // grab the next thing on top of the points_to_search stack
        int current_index = pop(points_to_search, &num_to_search);

        if (map->heights[current_index] == 9) // we've reached a ridge
            continue;
        if (is_in(current_index, basin, basin_size)) // already in the basin
            continue;

        // if not a ridge or already in the basin, add it to the basin
        push(basin, &basin_size, current_index);

        // TODO: Ensure that we aren't pushing items to the stack that are already there!
        if (Heightmap_neighbors(map, current_index, points_to_search, &num_to_search)) {
            printf("Error with Heightmap_neighbors() called in search_basin() main loop.\n");
            exit(-1);
        }

        /*printf("Basin: ");*/
        /*print_array(basin, basin_size);*/
        /*printf("Points to Search: ");*/
        /*print_array(points_to_search, num_to_search);*/
    }

    return basin_size;
}

/*
* Find the product of the top 3 basins in a heightmap.
*
* @param    map      heightmap to evaluate
* @retval   1        error reading heightmap
* @retval   0        basins evaluated and final result generated
*/
int evaluate_basins(struct Heightmap *map)
{
    if (map == NULL) {
        printf("Error: bad map pointer.\n");
        return 1;
    }
    // make sure we know how many low points / basins we have
    if (map->num_low_points == 0)
        count_low_points(map);

    // initialize array of basins
    int basin_index = 0;
    int basin_sizes[map->num_low_points];

    // look through entire map
    for (int map_index = 0; map_index < map->num_elements; map_index++) {
        if (is_low_point(map, map_index)) {
            basin_sizes[basin_index] = search_basin(map, map_index);
            basin_index++;
        }
    }
    array_sort_descending(basin_sizes, map->num_low_points, 3);
    printf("Top 3 Basins: ");
    print_array(basin_sizes, 3);

    map->basin_score = basin_sizes[0] * basin_sizes[1] * basin_sizes[2];

    return 0; // success
}

/*
* Print information about a height map
* @param    map     map to print information about
*/
void Heightmap_info(struct Heightmap *map)
{
    if (map == NULL) 
        printf("Error: heightmap not found.\n");
    printf("Heightmap is %u rows x %u cols.\n", map->num_rows, map->num_cols);
    if (!count_low_points(map))
        printf("Number of low points: %d\n", map->num_low_points);
    if (!evaluate_risk(map))
        printf("RISK RATING: %u\n", map->risk);
    evaluate_basins(map);
    printf("BASIN SCORE: %d\n", map->basin_score);
    printf("-------\n");
}

int main(int argc, char *argv[])
{
    struct Heightmap *test_map = Heightmap_create("9test");
    Heightmap_info(test_map);
    Heightmap_destroy(test_map);
    free(test_map);
    
    struct Heightmap *data_map = Heightmap_create("9data");
    Heightmap_info(data_map);
    Heightmap_destroy(data_map);
    free(data_map);

    return 0;
}
