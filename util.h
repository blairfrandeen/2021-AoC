#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// TODO: Figure out consistent error handling for this library.

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

/*
* Determine whether an integer is in an array
*
* @param    element         the integer to look for
* @param    array           the array to look in
* @param    n               the size of the array
* @retval   0               element is not present
* @reval    1               element is present
*/
int is_in(const int element, const int *array, const int n) 
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

/*
* Print an array to the screen (debug function)
*
* @param    array       the array to print
* @param    n           the length of the array
*                       (or number of elements to print)
*/
void print_array(const int array[], const int n)
{
    if (array == NULL) {
        printf("Error: print_array() called with bad pointer.\n");
        exit(-1);
    }
    printf("[ ");
    for (int i = 0; i < n; i++) {
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
* Push an element onto the end of an array that is not already in
* the array
*
* @param    p_array             pointer to the array to push to
* @param    p_n                 pointer to the number of elements in the array
* @param    element             element to add to the end of the array
*/
void push_unique(int *p_array, int *p_n, int element)
{
    if (!is_in(element, p_array, *p_n)) {
        push(p_array, p_n, element);
    }
}

/*
* pop an element off of the end of an array
*
* @param    p_array             pointer to the array to pop from
* @param    p_n                 pointer to the number of elements in the array
* @retval   element             element at the end of the array
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

/*
* Return a pointer to the minimum value of an array
*
* @param    arr     the array to search
* @param    n       number of elements
* @retval   min     the smallest element in the array
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

/*
* Return a pointer to the maximum value of an array
*
* @param    arr     the array to search
* @param    n       number of elements
* @retval   max     the largest element in the array
*/
int *max(int *arr, int *n)
{
    assert(arr != NULL);
    assert(n != NULL);
//    assert(*n > 0);
    if (*n <= 0) {
        errnum = 1;
    }

    int *max = arr;
    for (int i = 0; i < *n; i++, arr++) {
        if (*arr > *max)
            max = arr;
    }
    return max;
}

/*
* Sum the elements in an array
*
* @param    arr     array to sum
* @param    n       size of the array
*                   (or first n elements to sum)
* @retval   sum     sum of all elements in array
*/
int sum(int *arr, int *n)
{
    int *cur = arr;
    int sum = 0;
    for (; (cur - arr) <= *n; cur++) {
        sum += *cur;
    }
    return sum;
}

/*
* Return the range of an array
*
* @param    arr     array to sum
* @param    n       size of the array
*                   (or first n elements to find range of)
* @retval   range   maximum - minimum values
*/
int range(int *arr, int *n)
{
    if (*n <= 0) {
        errnum = 1;
        return 0;
    }
    return (*max(arr, n) - *min(arr, n));
}

/*
* Return the mean of an array
* @param    arr     array to sum
* @param    n       size of the array
*                   (or first n elements to find mean of)
* @retval   mean    The mean of the first n elements
*/
int mean(int *arr, int *n)
{
    if (*n <= 0) {
        errnum = 1;
        return 0;
    }
    return (sum(arr, n) / *n);
}
