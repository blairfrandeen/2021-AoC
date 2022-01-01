/* 2021 AoC Day 7
   https://adventofcode.com/2021/day/7
    Synopsis: Given array of crab positions,
    find lowest cost for crabs to move from
    current positions to a single position.
    Part 1, cost function is simply sum of distances
    that each crab has to move
    Part 2, cost function increases by distance moved
    c = n*(n+1) / 2
*/
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "util.h"

#define MAX_INPUTS 1024 // Maximum number of positions to evaluate.
#define MAX_DIGITS 4    // maximum number of digits in data

int split_input(char str[], int input[], char delims[])
{
    int num = 0;
    char *ptr = strtok(str, delims);
    while(ptr != NULL) {
        input[num] = atoi(ptr);
        assert(input[num] >= 0);
        ptr = strtok(NULL, delims);
        num++;
    }
    return num;
}

int cost(int inputs[], int *num_inputs, int *position)
{
    int i;
    int cost = 0;
    int dist;
    for (i = 0; i < *num_inputs; i++) {
        dist = abs(inputs[i] - *position);
        // part 1 cost function:
        // cost += dist;

        // part 2 cost function is sum of first n integers
        cost += (dist * (dist + 1) / 2);
    }

    return cost;
}

int main(int argc, char *argv[])
{
    int *input = malloc(MAX_INPUTS * sizeof(int));
    int num_inputs;

    if (argc > 1 && *argv[1] == 't') {
        // test case input
        printf("Test Mode\n");
        char test[] = "16,1,2,0,4,2,7,1,2,14";
        num_inputs = split_input(test, input, ",");
    } else {
        FILE *data = fopen("7data", "r");
        assert(data != NULL);

        // set the buffer size to be the maximum number
        // of positions times the max digits per position
        // plus additional char for ',' between positions
        char buffer[MAX_INPUTS * (MAX_DIGITS + 1)];

        fgets(buffer, sizeof(buffer), data);
        num_inputs = split_input(buffer, input, ",");
    }
    // print_array(input, num_inputs);
    printf("num elements: %d\n", num_inputs);
    printf("min: %d\n", *min(input, &num_inputs));
    printf("max: %d\n", *max(input, &num_inputs));
    printf("range: %d\n", range(input, &num_inputs));
    printf("sum: %d\n", sum(input, &num_inputs));
    printf("mean: %d\n", mean(input, &num_inputs));

    // fuck it, start w/ brute force
    // int position = min(input, &num_inputs);
    int num_costs = range(input, &num_inputs);
    printf("Brute force style: \n");
    printf("Number of cost functions evaluated: %d\n", num_costs);

    // evaluate cost function at first guess
    int cur_cost[num_costs];
    int i;
    for (i = 0; i < num_costs; i++) {
        cur_cost[i] = cost(input, &num_inputs, &i);
    }

    //print_array(cur_cost, num_inputs);
    // printf("Minimum cost: %d\n", min(cur_cost, &num_costs));

    printf("-------\nDone right:\n");
    int position = mean(input, &num_inputs);
    int grad = -1;
    // int delta = (position - min(input, &num_inputs)) / 2;

    // reset cost array
    /*memset(cur_cost, -1, sizeof(int));*/
    /*num_costs = 0;*/
    /*while (grad < 0) {*/
        /*cur_cost[num_costs] = cost(input, &num_inputs, &position);*/
        /*num_costs++;*/
    /*}*/

    return 0;
}
