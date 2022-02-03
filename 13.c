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

    // read the points
    char *token = NULL;
    char delimeters[] = "fold along,=\n";
    token = strtok(input_buffer, delimeters);
    while(token != NULL) {
        int x_coord = atoi(token);
        token = strtok(NULL, delimeters);
        int y_coord = atoi(token);
        token = strtok(NULL, delimeters);
        //printf("[%d, %d]\n", x_coord, y_coord);
        p_points_t[(*num_points)++] = Point_create(x_coord, y_coord);

        // break out of this loop if we've reached the end of the points
        if (token[0] == 'x' || token[0] == 'y') {
            //printf("Found start of folds\n");
            break;
        }
    }

    // read the folds
    while (token != NULL) {
        char fold_axis = token[0];
        token = strtok(NULL, delimeters);
        int fold_coord = atoi(token);
        //printf("Fold along %c at %d.\n", fold_axis, fold_coord);
        p_folds_t[(*num_folds)++] = Fold_create(fold_axis, fold_coord);
        token = strtok(NULL, delimeters);
    }
    printf("Found %d points and %d folds.\n", *num_points, *num_folds);

    free(input_buffer);
}

/*
* Determine whether a point at given x-y coordinates exists
* in list of points
*
* @param    x               x coordinate to check for
* @param    y               y coordinate to check for
* @param    p_points_t      pointer to list of points
* @param    p_num_points    number of points in list
* @retval   1               point at x, y exists in list
* @retval   0               point at x,y does not exist
*/
int Point_exists(int x, int y, struct Point **p_points_t, int *p_num_points)
{
    for (int i = 0; i < *p_num_points; i++) {
        if (p_points_t[i]->x == x && p_points_t[i]->y == y)
            return 1;
    }
    return 0;
}

void print_map(struct Point **p_points_t, int *p_num_points,
        int x_limit, int y_limit)
{
    for (int y_index = 0; y_index < y_limit; y_index++) {
        for (int x_index = 0; x_index < x_limit; x_index++) {
            if (Point_exists(x_index, y_index, p_points_t, p_num_points)) {
                printf("#");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

int fold(struct Point **p_points_t, int *p_num_points, char axis,
        int coordinate)
{
    int points_delta = 0;
    for (int point_index = 0; point_index < *p_num_points; point_index++) {
        if (axis == 'x') {
            if (coordinate > p_points_t[point_index]->x) {
                continue;
            } else {
                int new_x = 2 * coordinate - p_points_t[point_index]->x;
                if (Point_exists(new_x, p_points_t[point_index]->y,
                            p_points_t, p_num_points)) {
                    points_delta--;
                } else {
                    p_points_t[point_index]->x = new_x;
                    //points_delta++;
                }
                    
            }
        } else if (axis == 'y') {
            if (coordinate > p_points_t[point_index]->y) {
                continue;
            } else {
                int new_y = 2 * coordinate - p_points_t[point_index]->y;
                if (Point_exists(p_points_t[point_index]->x, new_y,
                            p_points_t, p_num_points)) {
                    points_delta--;
                } else {
                    p_points_t[point_index]->y = new_y;
                    //points_delta++;
                }
            }
        }
    }
    return points_delta;
}


int main(int argc, char *argv[])
{
    struct Point **points = malloc(sizeof(struct Point*) * MAX_POINTS);
    struct Fold **folds = malloc(sizeof(struct Fold*) * MAX_FOLDS);
    int num_folds = 0;
    int num_points = 0;
    char data_file[] = "data/13data";
    read_data(data_file, points, folds, &num_points, &num_folds);

    int x_limit = 0, y_limit = 0;
    int point_count = num_points;
    for (int i = 0; i < num_folds; i++) {
        char axis = folds[i]->axis;
        int coordinate = folds[i]->coordinate;
        point_count += fold(points, &num_points, axis, coordinate);
        if (i == 0)
            printf("Fold %d: %d points remaining.\n", i + 1, point_count);
        if (axis == 'x')
            x_limit = coordinate;
        if (axis == 'y')
            y_limit = coordinate;
    }
    print_map(points, &num_points, x_limit, y_limit);

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
