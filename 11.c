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

#define PRINT_DEBUG // comment out to hide debugging
/*
* Remove an element from the end of a queue and return it.
*
* @param    p_queue             queue structure to pop element from
* @param    p_queue_length      pointer to length of the queue.
* @retval                       value of last item in the queue.
*/
uint8_t dequeue(uint8_t *p_queue, unsigned *p_queue_length)
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
void queue(uint8_t *p_queue, unsigned *p_queue_length, uint8_t element)
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
* DEBUG: Print a queue array
*
* @param    p_queue         pointer to queue to print
* @param    p_queue_length  pointer to length of queue
*/
void print_queue(uint8_t *p_queue, unsigned *p_queue_length)
{
#ifdef PRINT_DEBUG
    printf("[ ");
    for (unsigned i = 0; i < *p_queue_length; i++) 
        printf("%d ", p_queue[i]);
    printf("]\n");
#endif
}

int main(int argc, char *argv[])
{
    unsigned queue_length = 5;
    uint8_t test_queue[5] = { 11, 29, 23, 43, 35 };

    // Testing of queue & dequeue functions
    print_queue(test_queue, &queue_length);
    printf("Dequeue: %d.\n", dequeue(test_queue, &queue_length));
    printf("New Length: %d\n", queue_length);
    queue(test_queue, &queue_length, 76);
    print_queue(test_queue, &queue_length);

    return 0;
}
