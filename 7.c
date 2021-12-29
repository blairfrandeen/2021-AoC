#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUTS 1024

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

void print_array(int array[], int num_elements)
{
    int i;
    printf("[ ");
    for (i = 0; i < num_elements; i++) {
        printf("%d ", array[i]);
    }
    printf("]\n");
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

int min(int arr[], int *num_elements)
{
    int i;
    int min = arr[0];
    for (i = 1; i < *num_elements; i++) {
        if (arr[i] < min)
            min = arr[i];
    }
    return min;
}

int max(int arr[], int *num_elements)
{
    int i;
    int max = arr[0];
    for (i = 1; i < *num_elements; i++) {
        if (arr[i] > max)
            max = arr[i];
    }
    return max;
}

int sum(int arr[], int *num_elements)
{
    int i;
    int sum = 0;
    for (i = 0; i < *num_elements; i++) {
        sum += arr[i];
    }
    return sum;
}

int range(int arr[], int *num_elements)
{
    return (max(arr, num_elements) - min(arr, num_elements));
}

int mean(int arr[], int *num_elements)
{
    return(sum(arr, num_elements) / *num_elements);
}

int main(int argc, char *argv[])
{
    char test[] = "16,1,2,0,4,2,7,1,2,14";
    int *input = malloc(MAX_INPUTS * sizeof(int));
    FILE *data = fopen("7data", "r");
    char buffer[4096];
    fgets(buffer, sizeof(buffer), data);
    int num_inputs = split_input(buffer, input, ",");
    // print_array(input, num_inputs);
    printf("num elements: %d\n", num_inputs);
    printf("min: %d\n", min(input, &num_inputs));
    printf("max: %d\n", max(input, &num_inputs));
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
    printf("Minimum cost: %d\n", min(cur_cost, &num_costs));

    printf("-------\nDone right:\n");
    int position = mean(input, &num_inputs);
    int grad = -1;
    int delta = (position - min(input, &num_inputs)) / 2;

    // reset cost array
    memset(cur_cost, -1, sizeof(int));
    int num_costs = 0;
    while (grad < 0) {
        cur_cost[num_costs] = cost(input, &num_inputs, &position);
        num_costs++;
    }

    return 0;
}
