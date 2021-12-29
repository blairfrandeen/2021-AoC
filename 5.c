/* ASSUMPTIONS:
1) Data in the form of x1,y1 -> x2,y2
2) All rows are same format as agove
3)  0------> +X
    |
    |
    |
    V
    +Y
*/
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define LINE_LEN 20
#define MAX_LINES 1000
#define NUM_COORDS 4
#define GRID_SIZE 1000

struct Point {
    int x, y;
};

struct Line {
    struct Point p1;
    struct Point p2;
    int dx;
    int dy;
};

struct Point *Point_create(int x, int y)
{
    struct Point *pt = malloc(sizeof(struct Point));
    pt->x = x;
    pt->y = y;

    return pt;
}

int is_horiz(struct Line *ln);
int is_vert(struct Line *ln);

struct Line *Line_create(struct Point p1, struct Point p2)
{
    struct Line *ln = malloc(sizeof(struct Line));

    // by convention, p1 is always start, p2 end
    ln->p1 = p1;
    ln->p2 = p2;
    ln->dx = p2.x - p1.x;
    ln->dy = p2.y - p1.y;

    return ln;
}

void Line_print(struct Line *ln)
{
    printf("Line from [%d, %d] to [%d, %d]\n", ln->p1.x, ln->p1.y,
            ln->p2.x, ln->p2.y);
}

void print_array(int array[], int num_elements) {
    int i;
    printf("[ ");
    for (i = 0; i < num_elements; i++) {
        printf("%d ", array[i]);
    }
    printf("]\n");
}

void split_str(char str[], int coords[], char delims[])
{
    int i;
    char *ptr = strtok(str, delims);
    for (i = 0; i < NUM_COORDS; i++) {
        assert(ptr != NULL);
        coords[i] = atoi(ptr);
        ptr = strtok(NULL, delims);
    }
    // print_array(coords, NUM_COORDS);
}

int is_horiz(struct Line *ln)
{
    return (ln->p1.y == ln->p2.y);
}

int is_vert(struct Line *ln)
{
    return (ln->p1.x == ln->p2.x);
}

int is_diag(struct Line *ln)
{
    return (abs(ln->dx) == abs (ln->dy));
}


int read_data(char datafile[], struct Line lines[])
{
    FILE *data = fopen(datafile, "r");
    assert(data != NULL);

    char buffer[LINE_LEN];
    int num_lines = 0;
    int coords[NUM_COORDS];

    struct Point p1, p2;
    while(fgets(buffer, sizeof(buffer), data) != NULL) {
        split_str(buffer, coords, " ->,");
        p1 = *Point_create(coords[0], coords[1]);
        p2 = *Point_create(coords[2], coords[3]);
        lines[num_lines] = *Line_create(p1, p2);
        /*if (is_vert(&lines[num_lines])) {*/
            /*printf("Vertical ");*/
            /*Line_print(&lines[num_lines]);*/
        /*}*/
        /*if (is_horiz(&lines[num_lines])) {*/
            /*printf("Horizontal ");*/
            /*Line_print(&lines[num_lines]);*/
        /*}*/
        num_lines++;
    }

    fclose(data);
    return num_lines;
}

int Line_len(struct Line *ln)
{
    int len = 0;
    if (is_horiz(ln)) {
        len = ln->p1.x - ln->p2.x;
    } else if (is_vert(ln)) {
        len = ln->p1.y - ln->p2.y;
    } else if (is_diag(ln)) {
        len = ln->p1.x - ln->p2.x;
    } else { // only return length of horizontal or vertical lines
        return -1;
    }

    return abs(len);
}

int main(int argc, char *argv[])
{
    // struct Line lines[50];
    // run_tests();
    struct Line lines[MAX_LINES];
    int num_lines = read_data("5data", lines);
    int grid[GRID_SIZE][GRID_SIZE];
    memset(grid, 0, GRID_SIZE * GRID_SIZE * sizeof(int));

    int l, s, dy, dx;
    int len = 0;
    for (l = 0; l < num_lines; l++) {
        if (!is_horiz(&lines[l]) && !is_vert(&lines[l])
                && !is_diag(&lines[l])) 
            continue;
        len = Line_len(&lines[l]);
        dx = lines[l].dx / len;
        dy = lines[l].dy / len;
        for (s = 0; s <= len; s++) {
            grid[lines[l].p1.x + s * dx][lines[l].p1.y + s * dy] += 1;
        }
        // Line_print(&lines[l]);
    }
    int x = 0;
    int y = 0;
    int score = 0;
    for (y = 0; y < GRID_SIZE; y++) {
        for (x = 0; x < GRID_SIZE; x++) {
            if (grid[x][y] > 1)
                score++;
            if (grid[x][y] == 0) {
        //        printf(".");
            } else {
        //        printf("%d", grid[x][y]);
            }
        }
        //printf("\n");
    }
    printf("Final score: %d\n", score);
    // print_grid(&grid);
    // printf("%d %d %d\n", grid[7][2], grid[7][3], grid[7][4]);
    /*Line_print(&lines[4]);*/
    /*printf("Length: %d\n", Line_len(&lines[4]));*/
    return 0;
}

void run_tests() {
    char test_str[] = "7, 0 -> 7, 4";
    int coords[NUM_COORDS];
    split_str(test_str, coords, " ->,");
    printf("%d\n", coords[0]);
    printf("%d\n", coords[3]);
}

