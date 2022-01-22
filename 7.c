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

int cost_fcn_p1(int dist)
{
    return dist;
}

int cost_fcn_p2(int dist)
{
    // part 2 cost function is sum of first n integers
    return (dist * (dist + 1) / 2);
}

int find_min_cost(int (*cost_function)(int), int *inputs, int num_inputs)
{
    int rng = range(inputs, &num_inputs);
    int cost[rng];
    // this is brute force method

    // loop through all possible positions
    // start at minimum position of all inputs
    int dist;
    int pos = *min(inputs, &num_inputs);
    for (int p = 0; p <= rng; p++) {
        cost[p] = 0;
        // loop through all inputs
        for (int i = 0; i < num_inputs; i++) {
            dist = abs(inputs[i] - pos);
            cost[p] += cost_function(dist);
        }
        pos++; 
    }

    return *min(cost, &num_inputs);
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
        FILE *data = fopen("data/7data", "r");
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

    // int position = min(input, &num_inputs);
    int num_costs = range(input, &num_inputs);
    printf("Number of cost functions evaluated: %d\n", num_costs);

    // some practice for function pointers
    int (*cost_function)(int) = NULL;

    cost_function = cost_fcn_p1;
    printf("Cost for Part 1: %d\n",
            find_min_cost(cost_function, input, num_inputs));

    cost_function = cost_fcn_p2;
    printf("Cost for Part 2: %d\n",
            find_min_cost(cost_function, input, num_inputs));

    return 0;
}
