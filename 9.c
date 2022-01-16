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

struct Heightmap {
    unsigned int num_rows;
    unsigned int num_cols;
    unsigned int num_elements;
    unsigned int *heights; // array of heights
    unsigned int risk;
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
    unsigned row_size = ftell(input_file_handle) - 1; // ignore the newline

    fseek(input_file_handle, 0, SEEK_END);
    size_t file_size = ftell(input_file_handle);
    rewind(input_file_handle);

    printf("File size: %lu bytes\n", file_size);
    printf("Row size: %u bytes\n", row_size);

    map->num_cols = row_size / sizeof(char);
    map->num_rows = file_size / (row_size + 1); // add 1 to rowsize to count /n
    map->num_elements = map->num_cols * map->num_rows;

    // allocate array for heightmap
    map->heights = (unsigned int *)malloc(map->num_elements * sizeof(unsigned int *));

    for (int height_index = 0; height_index < map->num_elements; height_index++) {
        unsigned next_height = (unsigned)(fgetc(input_file_handle) - '0'); 
        if (next_height >=0 && next_height <= 9) {
            map->heights[height_index] = next_height;
        } else {
            height_index--;
            continue;
        }
    }

    fclose(input_file_handle);
    map->risk = 0; // initialize risk

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
* Given row and column coordinates, return the height from a given heightmap.
*
* @param        map         pointer to Heightmap to query
* @param        row         row in Heightmap
* @param        col         column in Heightmap
*/
int rc_height(struct Heightmap *map, int row, int col)
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
    return (map->heights[row * map->num_cols + col]);
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
    int row = row_index(map, index);
    int col = col_index(map, index);
    // check above
    if (row != 0) { // skip if we're on the first row
        if (map->heights[index] >= rc_height(map, row - 1, col))
            return 0;
    }

    // check below
    if (row != map->num_rows - 1) { // skip if we're on the last row
        if (map->heights[index] >= rc_height(map, row + 1, col))
            return 0;
    }

    // check behind
    if (col != 0) { // skip if first column
        if (map->heights[index] >= rc_height(map, row, col - 1))
            return 0;
    }

    // check ahead
    if (col != map->num_cols - 1) { // skip if last column
        if (map->heights[index] >= rc_height(map, row, col + 1))
            return 0;
    }

    return 1; // default if no other points lower
}

/*
* Look through a height map and determine the number of lowpoints
* Assign a risk to the map based on the risk function.
*
* @param    map             height map to evaluate
* @retval   num_low_points  number of low points found
*/
int count_low_points(struct Heightmap *map)
{
    int num_low_points = 0;
    for (int height_index = 0; height_index < map->num_elements; height_index++) {
        if (is_low_point(map, height_index)) {
            num_low_points++;
            map->risk += low_point_risk(map->heights[height_index]);
        }
    }
    
    return num_low_points;
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
    printf("Number of low points: %d\n", count_low_points(map));
    printf("RISK RATING: %u\n", map->risk);
}

int main(int argc, char *argv[])
{
    char test_file[] = "9test";
    char data_file[] = "9data";
    char *input_file_name = data_file;
    if (argc > 1 && *argv[1] == 't')
        input_file_name = test_file;

    printf("Taking input from %s\n", (char *)input_file_name);

    struct Heightmap *hm = Heightmap_create(input_file_name);
    Heightmap_info(hm);

    Heightmap_destroy(hm);
    free(hm);

    return 0;
}
