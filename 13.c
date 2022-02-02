/*
Advent of Code Day 13
https://adventofcode.com/2021/day/13

Synopsis: Given list of points in an x-y grid
and a set of folding instructions. Coordinates
refer to dots on the paper (`#`) and anywhere
on the grid without coordinates is empty (`.`)

The fold instructions tell us to fold up along a line y=C
or left along a line x=C

dots may overlap, and become a single dot.
Dots will never be on a fold line.

Part 1: How many dots visible after the first fold?
Part 2: TBD

Note: The maximum number of dots that can be combined in a single fold
is half the dots on the page

Plan:
- Read the input file
- Make an array of points. Use a point struct, points have x and y coords
- Consider sorting the array
- Make a list of folds
- Make a fold function that takes a fold line and an axis, 
    and operates on the list of points. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_POINTS 1024
#define MAX_FOLDS 16

struct Point {
    int x;
    int y;
};

struct Fold {
    char axis;
    int coordinate;
};

struct Point *Point_create(int x, int y)
{
    struct Point *pt = malloc(sizeof(struct Point));
    pt->x = x;
    pt->y = y;
    return pt;
}

struct Fold *Fold_create(char axis, int coordinate)
{
    struct Fold *fd = malloc(sizeof(struct Fold));
    fd->axis = axis;
    fd->coordinate = coordinate;
    return fd;
}

/*
* read data from an input file. Populate points and folds arrays
* and pass by reference number of folds & points.
*
* @param    data_file       Data file to read
* @param    p_points_t      Points array
* @param    p_folds_t       Folds array
* @param    p_num_points    number of points in input
* @param    p_num_folds     number of folds in input
*/
void read_data(char data_file[], struct Point **p_points_t, struct Fold **p_folds_t,
        int *num_points, int *num_folds)
{
    FILE *file_handle = fopen(data_file, "r");
    if (file_handle == NULL) {
        printf("Error reading data file %s: %s\n", data_file, strerror(errno));
        exit(-1);
    }
    printf("Reading data from %s...\n", data_file);

    // find the file size
    fseek(file_handle, 0, SEEK_END);
    size_t file_size = ftell(file_handle);
    rewind(file_handle);

    char *input_buffer = (char *)malloc(file_size * sizeof(char) + 1);

    size_t bytes_read = fread((void*)input_buffer, sizeof(char), file_size, file_handle);
    if (bytes_read != file_size) {
        printf("Error reading file. Read %ld bytes, expected %ld.\n",
                bytes_read, file_size);
        exit(-1);
    }

    fclose(file_handle);
    input_buffer[file_size] = 0;
    char *token = NULL;
    // read the points
    char delimeters[] = "fold along,=\n";
    token = strtok(input_buffer, ",\n");
    while(token != NULL) {
        int x_coord = atoi(token);
        token = strtok(NULL, delimeters);
        int y_coord = atoi(token);
        token = strtok(NULL, delimeters);
        printf("[%d, %d]\n", x_coord, y_coord);
        p_points_t[(*num_points)++] = Point_create(x_coord, y_coord);
        if (token[0] == 'x' || token[0] == 'y') {
            printf("Found start of folds\n");
            break;
        }
    }

    while (token != NULL) {
        char fold_axis = token[0];
        token = strtok(NULL, delimeters);
        int fold_coord = atoi(token);
        printf("Fold along %c at %d.\n", fold_axis, fold_coord);
        p_folds_t[(*num_folds)++] = Fold_create(fold_axis, fold_coord);
        token = strtok(NULL, delimeters);
    }
    printf("Found %d points and %d folds.\n", *num_points, *num_folds);

    free(input_buffer);
}

int main(int argc, char *argv[])
{
    struct Point **points = malloc(sizeof(struct Point*) * MAX_POINTS);
    struct Fold **folds = malloc(sizeof(struct Fold*) * MAX_FOLDS);
    int num_folds = 0;
    int num_points = 0;
    char data_file[] = "data/13test";
    read_data(data_file, points, folds, &num_points, &num_folds);

    for (int i = 0; i < num_points; i++) {
        free(points[i]);
    }
    free(points);
    for (int i = 0; i < num_folds; i++) {
        free(folds[i]);
    }
    free(folds);
    return (EXIT_SUCCESS);
}
