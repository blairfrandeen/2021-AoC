#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_AGE 10
#define MAX_FISH 300

int split_input(char str[], unsigned char fish[], char delims[])
{
    int num = 0;
    char *ptr = strtok(str, delims);
    while(ptr != NULL) {
        fish[num] = atoi(ptr);
        assert(fish[num] >= 0);
        assert(fish[num] <= 6);
        ptr = strtok(NULL, delims);
        num++;
    }
    return num;
}

void sort_fish(unsigned char fish[], unsigned long sorted_fish[], int num_fish)
{
    int i;
    for (i = 0; i < num_fish; i++) {
        sorted_fish[fish[i]]++;
    }
}

void print_fish(unsigned long fish[])
{
    int i;
    printf("[ ");
    for (i = 0; i < MAX_AGE; i++) {
        printf("%ld ", fish[i]);
    }
    printf("]\n");
}

unsigned long sum_fish(unsigned long fish[])
{
    int i;
    unsigned long sum = 0;
    for (i = 0; i < MAX_AGE; i++) {
        sum += fish[i];
    }
    return sum;
}

int main(int argc, char *argv[])
{
    char fish_test[] = "3,4,3,1,2";
    char fish_input[] = "1,1,3,5,1,3,2,1,5,3,1,4,4,4,1,1,1,3,1,4,3,1,2,2,2,4,1,1,5,5,4,3,1,1,1,1,1,1,3,4,1,2,2,5,1,3,5,1,3,2,5,2,2,4,1,1,1,4,3,3,3,1,1,1,1,3,1,3,3,4,4,1,1,5,4,2,2,5,4,5,2,5,1,4,2,1,5,5,5,4,3,1,1,4,1,1,3,1,3,4,1,1,2,4,2,1,1,2,3,1,1,1,4,1,3,5,5,5,5,1,2,2,1,3,1,2,5,1,4,4,5,5,4,1,1,3,3,1,5,1,1,4,1,3,3,2,4,2,4,1,5,5,1,2,5,1,5,4,3,1,1,1,5,4,1,1,4,1,2,3,1,3,5,1,1,1,2,4,5,5,5,4,1,4,1,4,1,1,1,1,1,5,2,1,1,1,1,2,3,1,4,5,5,2,4,1,5,1,3,1,4,1,1,1,4,2,3,2,3,1,5,2,1,1,4,2,1,1,5,1,4,1,1,5,5,4,3,5,1,4,3,4,4,5,1,1,1,2,1,1,2,1,1,3,2,4,5,3,5,1,2,2,2,5,1,2,5,3,5,1,1,4,5,2,1,4,1,5,2,1,1,2,5,4,1,3,5,3,1,1,3,1,4,4,2,2,4,3,1,1";

    unsigned char *fish = malloc(MAX_FISH * sizeof(unsigned char));
    memset(fish, -1, MAX_FISH * sizeof(unsigned char));
    // look at realloc
    unsigned long int num_fish = split_input(fish_input, fish, ",");
    printf("Num fish: %ld\n", num_fish);
    unsigned long int sorted_fish[MAX_AGE];
    memset(sorted_fish, 0, (MAX_AGE) * sizeof(unsigned long int));
    sort_fish(fish, sorted_fish, num_fish);
    print_fish(sorted_fish);
    int num_days = 256;
    int n;
    int day;
    unsigned long new_fish = 0;

    for (day = 0; day < num_days; day++) {
        // all fish with timer zero make new fish at max timer
        new_fish = sorted_fish[0];
        for (n = 0; n < (9); n++) {
            sorted_fish[n] = sorted_fish[n+1];
        }
        sorted_fish[6] += new_fish;
        sorted_fish[8] += new_fish;
        /*}*/
        // print_fish(sorted_fish);
        // printf("Day %d: %ld fish\n", day + 1, sum_fish(sorted_fish));
    }
    print_fish(sorted_fish);
    printf("Day %d: %ld fish\n", day, sum_fish(sorted_fish));
    return 0;
}
