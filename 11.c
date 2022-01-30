/* Advent of Code day 11
   https://adventofcode.com/2021/day/11
Surrounded by flashing octopi. (Octopuses)?

each Octopus has energy level 0 thru 9. If energy level increases beyond 9,
They flash, increasing the energy of nearby octopi. Each step, energy increases by 1 for all
octopi to start. Each octopus can only flash once per step. After a flash, energy level goes to zero.

Part I Plan:
- Generate an octopus grid as a 1D matrix with the ability to access it by rows & columns.
- Use a queue data structure--first in, first out
- At the start of each turn, add all octopi to the queue
- Step through the queue until it is empty. For each element popped off the end,
    run the increase_energy() function (which just adds 1 to the energy level)
- If the energy level reaches 10, add the neighboring octopi to the end of the queue.
- This will continue to increase energy levels & flash octopi until the queue is exhausted.
- Finally, run through the grid and set any energy levels that are 10 or greater to 0.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <time.h>

//#define PRINT_DEBUG // comment out to hide debugging
#define CLOCK_INIT clock_t start_time, end_time;
#define CLOCK_START start_time = clock();
#define CLOCK_END end_time = clock(); printf("Executed in %.6f seconds.\n", (float)(end_time - start_time)/CLOCKS_PER_SEC);

struct OctopusArmy {
    int num_rows;
    int num_cols;
    int num_octopuses;
    uint8_t *energy_levels;
};

/*
* Read a datafile of inital energy levels and fill an army of flashing octopi 
*
* @param    datafile        the data file to read from
* @retval   p_army_t        an army of octopuses, what else?
*/
struct OctopusArmy *Assemble(char datafile[])
{
    FILE *file_stream = fopen(datafile, "r");
    if (file_stream == NULL) {
        printf("Error: Could not open file %s: %s.\n", datafile, strerror(errno));
        exit(-1);
    }
    struct OctopusArmy *p_army_t = malloc(sizeof(struct OctopusArmy));

    // determine number of columns
    while(fgetc(file_stream) != '\n');
    p_army_t->num_cols = ftell(file_stream) / sizeof(char) - 1;

    // determine number of rows
    fseek(file_stream, 0, EOF);
    p_army_t->num_rows = ftell(file_stream) / sizeof(char) - 1;
    rewind(file_stream);

    p_army_t->num_octopuses = p_army_t->num_rows * p_army_t->num_cols;

    p_army_t->energy_levels = malloc(p_army_t->num_rows * p_army_t->num_cols * sizeof(uint8_t));

    int octopus_index = 0;
    char next_octopus;
    while((next_octopus = fgetc(file_stream)) != EOF) {
        if (next_octopus == '\n') continue;

        p_army_t->energy_levels[octopus_index] = next_octopus - '0';
        octopus_index++;
    }

    fclose(file_stream);
    return p_army_t;
}

/*
* Disperse the army of octopuses back to their ocean homes.
*
* @param    p_army_t            pointer to the army to disperse
*/
void OctopusArmy_disperse(struct OctopusArmy *p_army_t)
{
    free(p_army_t->energy_levels);
    free(p_army_t);
}

/*
* Remove an element from the end of a queue and return it.
*
* @param    p_queue             queue structure to pop element from
* @param    p_queue_length      pointer to length of the queue.
* @retval                       value of last item in the queue.
*/
int dequeue(int *p_queue, unsigned *p_queue_length)
{
    if (p_queue == NULL || p_queue_length == NULL) {
        printf("Bad pointer sent to dequeue().\n");
        exit(-1);
    } else if (*p_queue_length < 1) {
        printf("Dequeue tried to access queue with length %d.\n", *p_queue_length);
        exit(-1);
    } 
    (*p_queue_length)--; // decrease the length of the queue
    return p_queue[*p_queue_length];
}

/*
* Add an element to the rear of the queue
* 
* @param    p_queue             queue structure to add element to
* @param    p_queue_length      pointer to length of the queue.
*/
void queue(int *p_queue, unsigned *p_queue_length, int element)
{
    if (p_queue == NULL || p_queue_length == NULL) {
        printf("Bad pointer sent to queue().\n");
        exit(-1);
    } else if (*p_queue_length < 0) {
        printf("queue tried to access queue with length %d.\n", *p_queue_length);
        exit(-1);
    }
    (*p_queue_length)++; // make space in the queue
    for (unsigned queue_index = *p_queue_length - 1; queue_index > 0; queue_index--) {
        p_queue[queue_index] = p_queue[queue_index - 1];
    }
    p_queue[0] = element;
}

/*
* Add an element to top of a stack
* 
* @param    p_stack             stack structure to add element to
* @param    p_stack_length      pointer to length of the stack.
*/
void push(int *p_stack, unsigned *p_stack_length, int element)
{
    if (p_stack == NULL || p_stack_length == NULL) {
        printf("Bad pointer sent to push().\n");
        exit(-1);
    } else if (*p_stack_length < 0) {
        printf("push() tried to access stack with length %d.\n", *p_stack_length);
        exit(-1);
    }
    p_stack[*p_stack_length] = element;
    (*p_stack_length)++; // make space in the stack
    /*printf("Push: %d to stack size %d.\n", element, *p_stack_length);*/
}

/*
* Remove an element from the end of a queue and return it.
*
* @param    p_stack             stack structure to pop element from
* @param    p_stack_length      pointer to length of the stack.
* @retval                       value of last item in the stack.
*/
int pop(int *p_stack, unsigned *p_stack_length)
{
    if (p_stack == NULL || p_stack_length == NULL) {
        printf("Bad pointer sent to pop().\n");
        exit(-1);
    } else if (*p_stack_length < 1) {
        printf("pop() tried to access stack with length %d.\n", *p_stack_length);
        exit(-1);
    } 
    (*p_stack_length)--; // decrease the length of the stack
    return p_stack[(*p_stack_length)];
}

/*
* DEBUG: Print a queue array
*
* @param    p_queue         pointer to queue to print
* @param    p_queue_length  pointer to length of queue
*/
void print_queue(int *p_queue, unsigned *p_queue_length)
{
#ifdef PRINT_DEBUG
    printf("[ ");
    for (unsigned i = 0; i < *p_queue_length; i++) 
        printf("%d ", p_queue[i]);
    printf("]\n");
#endif
}

/*
* Print info about the octopus army
*
* @param    p_army_t            Pointer to the army to query
*/
void OctopusArmy_info(struct OctopusArmy *p_army_t)
{
#ifdef PRINT_DEBUG
    printf("Army is %d rows x %d cols.\n", p_army_t->num_rows, p_army_t->num_cols);
    printf("There are %d octopusues ready to flash you.\n", p_army_t->num_octopuses);
    for (int i = 0; i < p_army_t->num_octopuses; i++) {
        if (i % p_army_t->num_cols == 0 && i != 0) 
            printf("\n");
        printf("%d", p_army_t->energy_levels[i]);
    }
    printf("\n");
#endif
}

/*
* Allow an octopus to charge its neighbors after flashing.
*
* @param    p_army_t        the army of octopuses
* @param    octopus_index   the index within the army of the flasher
* @param    p_energy_stack  The stack of octopuses to have their energy raised
* @param    p_stack_length  the length of the energy stack
*/
void charge_neighbors(struct OctopusArmy *p_army_t, int octopus_index,
        int *p_energy_stack, unsigned *p_stack_length)
{
    if (p_army_t == NULL) {
        printf("Error: Bad pointer to OctopusArmy in charge_neighbors().\n");
        exit(-1);
    }

    int self_row = octopus_index / p_army_t->num_rows;
    int self_col = octopus_index % p_army_t->num_cols;

    for (int neighbor_row = self_row - 1; neighbor_row <= self_row + 1; neighbor_row++) {
        // skip if we're on the edge of the map
        if (neighbor_row < 0 || neighbor_row > p_army_t->num_rows - 1)
            continue;
        for (int neighbor_col = self_col - 1; neighbor_col <= self_col + 1; neighbor_col++) {
            // skip if we're on the edge of the map
            if (neighbor_col < 0 || neighbor_col > p_army_t->num_cols - 1)
                continue;
            // don't self-charge
            if (neighbor_col == self_col && neighbor_row == self_row)
                continue;
            int neighbor_index = neighbor_col + neighbor_row * p_army_t->num_cols;
            if (p_army_t->energy_levels[neighbor_index] < 10)
                push(p_energy_stack, p_stack_length, neighbor_index);
        }
    }

}

/*
* Allow an octopus army to do its thing and flash at you
* 
* @param    p_army_t        pointer to the octopus army
* @param    target_step     step at which we want to know the total number of flashes
* @retval   total_flashes   total number of times the army flashed prior to synchronizing
*/
unsigned OctopusArmy_flash(struct OctopusArmy *p_army_t, int target_step)
{
    if (p_army_t == NULL) {
        printf("Error: Bad pointer to OctopusArmy in OctopusArmy_flash().\n");
        exit(-1);
    }
    if (target_step < 1) {
        printf("Error: Cannot flash for less than 1 step.\n");
        return 0;
    }
    unsigned total_flashes = 0;
    int step_flashes = 0;
    int step = 0;

    int *energy_stack = malloc(p_army_t->num_octopuses * sizeof(int*) * 9);
    while(step_flashes != p_army_t->num_octopuses) {
        step_flashes = 0;

        unsigned stack_size = 0;

        // add all octopi to the energy increase queue
        for (int octopus_index = 0; octopus_index < p_army_t->num_octopuses; octopus_index++) {
            if (p_army_t->energy_levels[octopus_index] < 10)
                push(energy_stack, &stack_size, octopus_index);

            while(stack_size > 0) {
                int energy_index = pop(energy_stack, &stack_size);
                p_army_t->energy_levels[energy_index] += 1;
                if (p_army_t->energy_levels[energy_index] == 10) {
                    step_flashes++;
                    charge_neighbors(p_army_t, energy_index, energy_stack, &stack_size);
                }
            }
        }

        // reset all octopuses that flashed to 0 energy
        for (int octopus_index = 0; octopus_index < p_army_t->num_octopuses; octopus_index++) {
            if (p_army_t->energy_levels[octopus_index] > 9)
                p_army_t->energy_levels[octopus_index] = 0;
        }

        total_flashes += step_flashes;
        step++;
        if (step == target_step) {
            printf("Total of %u flashes after %d steps.\n", total_flashes, step);
        }
    }
    energy_stack = NULL;
    free(energy_stack);
    printf("Octopuses sync after %d steps and %u flashes.\n", step, total_flashes);

    return total_flashes;
}

int main(int argc, char *argv[])
{

    int num_steps = 100; // set arbitrarily high to solve part II
    CLOCK_INIT
    CLOCK_START
    struct OctopusArmy *test_army = Assemble("data/11test");
    printf("Test Input:\n");
    OctopusArmy_flash(test_army, num_steps);
    OctopusArmy_info(test_army);
    OctopusArmy_disperse(test_army);
    CLOCK_END

    CLOCK_START
    struct OctopusArmy *data_army = Assemble("data/11data");
    printf("Puzzle Input:\n");
    OctopusArmy_flash(data_army, num_steps);
    OctopusArmy_info(data_army);
    OctopusArmy_disperse(data_army);
    CLOCK_END

    CLOCK_START
    struct OctopusArmy *big_army = Assemble("bigdata/11-100");
    printf("Big Input (100x100):\n");
    OctopusArmy_flash(big_army, num_steps);
    OctopusArmy_info(big_army);
    OctopusArmy_disperse(big_army);
    CLOCK_END

    CLOCK_START
    struct OctopusArmy *bigger_army = Assemble("bigdata/11-1000");
    printf("Bigger Input (1000x1000):\n");
    OctopusArmy_flash(bigger_army, num_steps);
    OctopusArmy_info(bigger_army);
    OctopusArmy_disperse(bigger_army);
    CLOCK_END
    return 0;
}
