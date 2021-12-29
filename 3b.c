#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

/* Part I Plan:
   - Read a datafile
   - Determine the word size
   - Determine the number of words
   - What to do with the data?
    - Array of strings, i.e. "100101"
        - "bulky" - will take lots of data
        - Doesn't require translating to ints initially
        - But binary to int function will be required regardless
    - Array of ints from binary strings
        - Leaner
        - Easily translate to binary and back
    - Array of locations in filestream
        - probably hard to work with
        - even harder to work with in part II
   --> I am choosing to make an array of ints from bin strings
   - How to put the data into an array?
    - I can't create an array of ints prior to opening the filestream
        and learning how big that array needs to be.
    - I can use malloc to make an array of an arbitrarily 
        large size prior to passing the filestream to bin_to_int(),
        then attempt to free up any remaining space in that array.
    - I can open the filestream from main(), and call get_word_size() and
        get_num_rows() from main(), then allocate an array of the correct
        size.
    - I can use an OS-specific function like fstat() to read the file size from
        the OS
    - I can write a function to append to an array
   --> I am choosing to open the filestream twice, since this allows
        for proper array sizing and ultimately more flexibility
   - Determine the most common bit at a given index
   - Use most common bits to get gamma
   - Use ~gamma to get epsilon

   Part II Plan:
   - Re-use all code except gamma/epsilon from above
   - Write a filtering function that takes an array
        of filestream positions as input, and returns
        a new array of filestream positions based on
        a filtering criteria
   - Recursively call the filtering function until the part II 
        answers are completed

   */
int get_word_size(FILE *data)
{
    rewind(data); // start at the beginning
    int word_size = 0;
    while(fgetc(data) != '\n') {
        word_size++;
    }
    return word_size;
}

int get_num_rows(FILE *data, int word_size)
{
    fseek(data, 0, SEEK_END);
    int end_bytes = ftell(data);
    rewind(data);

    return end_bytes / (word_size + 1);
}

int bin_to_int(char bin_str[], int *word_size)
{
    int result = 0;
    int bit_index;
    for (bit_index = 0; bit_index < *word_size; bit_index++) {
        switch (bin_str[bit_index]) {
            case '0':
                break;
            case '1':
                result += 0b1 << (*word_size - bit_index - 1);
                break;
            case '\n':
            case '\0':
                break;
            default:
                printf("ERROR: Bad data (%c)\n", bin_str[bit_index]);
        }
    }
    return result;
}

void read_file_info(char datafile[], int *word_size, int *num_rows)
{
    FILE *data = fopen(datafile, "r");
    assert(data != NULL);
    
    *word_size = get_word_size(data);
    *num_rows = get_num_rows(data, *word_size);

    // be kind
    rewind(data);
    fclose(data);
}

void read_data(char datafile[], int *word_size, int *num_rows, int numbers[])
{
    FILE *data = fopen(datafile, "r");
    assert(data != NULL);

    char buffer[*(word_size + 1)]; // additional char for \n
    int i = 0;
    while (fgets(buffer, sizeof(buffer), data) != NULL) {
        numbers[i] = bin_to_int(buffer, word_size);
        // printf("%d\n", numbers[i]);
        i++;
    }
    
    // be kind
    rewind(data);
    fclose(data);
}

unsigned short int most_common(int numbers[], int bit_index, int num_rows)
{
    int i;
    int count1 = 0;
    int count0 = 0;
    int test_bit = 0b1 << bit_index;
    for (i = 0; i < num_rows; i++) {
        if (numbers[i] & test_bit) {
            count1++;
        } else {
            count0++;
        }
    }
    if (count1 >= count0) {
        return 1;
    } else {
        return 0;
    }
}

unsigned short int least_common(int numbers[], int bit_index, int num_rows)
{
    if (most_common(numbers, bit_index, num_rows) == 1) {
        return 0;
    } else {
        return 1;
    }
}

int filter(int array[], int num_elements, int bit_index, int criteria)
{
    assert(criteria == 1 || criteria == 0);
    int i;
    int filtered_elements = 0;
    int test_bit = 0b1 << bit_index;

    for (i = 0; i < num_elements; i++) {
        if ((array[i] & test_bit) == (criteria * test_bit)) {
            array[filtered_elements] = array[i];
            filtered_elements++;
        }
    }
    return filtered_elements;
}

void print_array(int array[], int num_elements) {
    int i;
    printf("[ ");
    for (i = 0; i < num_elements; i++) {
        printf("%d ", array[i]);
    }
    printf("]\n");
}

int invert(int input, int word_size)
{
    int i;
    int result = 0;
    for (i = 0; i < word_size; i++) {
        if (input & (0b1 << i)) {
            continue;
        } else {
            result += (0b1 << i);
        }
    }
    return result;
}

int main(int argc, char *argv[])
{
    int word_size, num_rows;
    char data_file[] = "3data.txt";

    read_file_info(data_file, &word_size, &num_rows);

    int numbers[num_rows];
    read_data(data_file, &word_size, &num_rows, numbers);

    printf("word size: %d bits\n", word_size);
    printf("file size: %d rows\n", num_rows);

    int i;
    unsigned int gamma = 0;
    unsigned int epsilon = 0;
    for (i = 0; i < word_size; i++) {
        gamma += most_common(numbers, i, num_rows) << i;
        epsilon += least_common(numbers, i, num_rows) << i;
    }
    printf("gamma: %u\n", gamma);
    printf("epsilon: %u\n", epsilon);

    int index;
    int criteria;

    for (index = word_size - 1; index >= 0; index--) { // start at msb
        criteria = most_common(numbers, index, num_rows);
        // print_array(numbers, num_rows);
        // printf("most common at index %d: %d\n", index, criteria);
        num_rows = filter(numbers, num_rows, index, criteria);
        if (num_rows == 1)
            break;
    }
    int ox_rate = numbers[0];
    read_file_info(data_file, &word_size, &num_rows);
    read_data(data_file, &word_size, &num_rows, numbers);

    for (index = word_size - 1; index >= 0; index--) { // start at msb
        criteria = invert(most_common(numbers, index, num_rows), 1);
        // print_array(numbers, num_rows);
        // printf("most common at index %d: %d\n", index, criteria);
        num_rows = filter(numbers, num_rows, index, criteria);
        if (num_rows == 1)
            break;
    }

    int co2_rate = numbers[0];
    printf("Oxygen Rate: %d - CO2 Rate: %d\n", ox_rate, co2_rate);
    printf("Result: %d\n", ox_rate * co2_rate);

    return 0;
}
