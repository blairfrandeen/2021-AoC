#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <stdint.h>

#define MAX_AGE 10
#define MAX_FISH 300

#define CLOCK_INIT clock_t start_time, end_time;
#define CLOCK_START start_time = clock();
#define CLOCK_END end_time = clock(); printf("Executed in %.6f seconds.\n", (float)(end_time - start_time)/CLOCKS_PER_SEC);

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

struct BigNum {
    uint8_t *digits;
    unsigned num_digits;
    unsigned max_digits;
};

struct BigNum *BigNum_init(unsigned max_digits, uint8_t first_digit)
{
    if (first_digit > 9) {
        printf("Warning: BigNum_init() called with first_digit %d > 9. ", first_digit);
        first_digit = first_digit % 10;
        printf("Truncating to %d.\n", first_digit);
    }
    struct BigNum *p_bignum_ret = malloc(sizeof(struct BigNum));
    p_bignum_ret->digits = malloc(max_digits * sizeof(uint8_t *));
    p_bignum_ret->digits[0] = first_digit;
    p_bignum_ret->num_digits = 1;
    p_bignum_ret->max_digits = max_digits;

    return p_bignum_ret;
}

void BigNum_add(struct BigNum *n1, struct BigNum *n2)
{
    unsigned num_digits = n1->num_digits > n2->max_digits ? n1->num_digits : n2->num_digits;
    uint8_t carryover = 0;
    uint8_t sum;
    printf("N: %d.\n", n2->num_digits);
    for (int index = 0; index < num_digits; index++) {
        carryover = ((n1->digits[index] + n2->digits[index] + carryover) / 10);
        sum = (n1->digits[index] + n2->digits[index]) % 10;
        n1->digits[index] = sum;
    }
    /*printf("%u, %u\n", sum, carryover);*/
    if (carryover > 0) {
        if (n1->digits[num_digits-1] > 0 && n1->digits[num_digits] == 0)
            n1->num_digits++;
        n1->digits[num_digits] += carryover;
    }
    /*printf("%d, %d", n1->num_digits, n2->num_digits);*/
}

void BigNum_display(struct BigNum *num)
{
    printf("Big Number with %d digits: ", num->num_digits);
    for (int digit = num->num_digits - 1; digit >= 0; digit--) {
        printf("%d", num->digits[digit]);
    }
    printf("\n");
}
void BigNum_free(struct BigNum *p_bignum)
{
    free(p_bignum->digits);
    free(p_bignum);
}

int main(int argc, char *argv[])
{
    CLOCK_INIT
    CLOCK_START
 //   char fish_test[] = "3,4,3,1,2";
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
    free(fish);

    struct BigNum *n1 = BigNum_init(300, 8);
    struct BigNum *n2 = BigNum_init(300, 8);

    for (int i = 0; i < 150; i++) {
        BigNum_add(n1, n2); // 16
        if (i % 10 == 0)
            BigNum_display(n1);
    }


    BigNum_free(n2);
    BigNum_free(n1);
    CLOCK_END
    return 0;
}
