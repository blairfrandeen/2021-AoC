#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

int errnum = 0; /* Used in test.c */

/** Splits an input string str into an integer array
  * splitting by delims. Returns number of elements
  * in integer array. */
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

int is_in(const int element, const int *array, const int n) 
/*  Arguments:
    element - an integer
    *array - a pointer to an array
    *arr_size - pointer to number of elements in that array
    Returns:
    whether element is in the array.
*/
{
    if (n <= 0 ) {
        errnum = 1; 
    }
    for (int i = 0; i < n; i++) {
        if (*array == element) {
            return 1;
        }
        array++;
    }
    return 0;
}

void print_array(const int array[], const int n)
{
    int i;
    printf("[ ");
    for (i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }
    printf("]\n");
}


/*
* Push an element onto the end of an array
*
* @param    p_array             pointer to the array to push to
* @param    p_n                 pointer to the number of elements in the array
* @param    element             element to add to the end of the array
*/
void push(int *p_array, int *p_n, int element)
{
    if (p_array == NULL || p_n == NULL) {
        printf("Invalid pointer in push()\n");
        exit(-1);
    }
    if (*p_n < 0) {
        printf("Error: push() attempted to access array of size < 0.\n");
        exit(-1);
    }
    p_array[*p_n] = element;
    (*p_n)++; // increase array size
}

/*
* pop an element off of the end of an array
*
* @param    p_array             pointer to the array to pop from
* @param    p_n                 pointer to the number of elements in the array
* @retval   elemenet            element at the end of the array
*/
int pop(int *p_array, int *p_n)
{
    if (p_array == NULL || p_n == NULL) {
        printf("Invalid pointer in pop()\n");
        exit(-1);
    }
    if (*p_n < 0) {
        printf("Error: pop() attempted to access array of size < 0.\n");
        exit(-1);
    }
    (*p_n)--; // decrease the size of the array
    return p_array[*p_n];
}
/* Arguments:
    arr - array of integers
    *num_elements - pointer to number of elements in array
   Returns:
    int pointer to minimum integer value in array
*/
int *min(int *arr, const int *n)
{
    if (arr == NULL) {
        printf("Bad array");
        return NULL;
    }
    assert(arr != NULL);
    assert(n != NULL);
    if (*n <= 0) {
        errnum = 1;
    }

    int *min = arr;
    for (int i = 0; i < *n; i++, arr++) {
        if (*arr < *min)
            min = arr;
    }
    return min;
}

int *max(int *arr, int *num_elements)
{
    assert(arr != NULL);
    assert(num_elements != NULL);
//    assert(*num_elements > 0);
    if (*num_elements <= 0) {
        errnum = 1;
    }

    int *max = arr;
    for (int i = 0; i < *num_elements; i++, arr++) {
        if (*arr > *max)
            max = arr;
    }
    return max;
}

int sum(int *arr, int *num_elements)
{
    int *cur = arr;
    int sum = 0;
    for (; (cur - arr) <= *num_elements; cur++) {
        sum += *cur;
    }
    return sum;
}

int range(int *arr, int *num_elements)
{
    if (*num_elements <= 0) {
        errnum = 1;
        return 0;
    }
    return (*max(arr, num_elements) - *min(arr, num_elements));
}

int mean(int *arr, int *num_elements)
{
    if (*num_elements <= 0) {
        errnum = 1;
        return 0;
    }
    return (sum(arr, num_elements) / *num_elements);
}
