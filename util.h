#include <stdio.h>
#include <assert.h>

typedef enum {
    NO_ERROR,
    NULL_POINTER_ERROR
} ErrorType;

int is_in(int element, int array[], int arr_size) 
{
    int i;
    for (i = 0; i < arr_size; i++) {
        if (array[i] == element) {
            return 1;
        }
    }
    return 0;
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

/* Arguments:
    arr - array of integers
    *num_elements - pointer to number of elements in array
   Returns:
    int pointer to minimum integer value in array
*/
int *min(int *arr, int *num_elements)
{
    if (arr == NULL) {
        printf("Bad array");
        return NULL;
    }
    assert(arr != NULL);
    assert(num_elements != NULL);
    assert(*num_elements > 0);

    int *min = arr;
    for (int i = 0; i < *num_elements; i++, arr++) {
        if (*arr < *min)
            min = arr;
    }
    return min;
}

int *max(int *arr, int *num_elements)
{
    assert(arr != NULL);
    assert(num_elements != NULL);
    assert(*num_elements > 0);

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
    return (*max(arr, num_elements) - *min(arr, num_elements));
}

int mean(int arr[], int *num_elements)
{
    return(sum(arr, num_elements) / *num_elements);
}
