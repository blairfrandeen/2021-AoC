/* Advent of Code Day 12
   https://adventofcode.com/2021/day/12

SYNOPSIS:
    Given connections between caves in a cave system. Graph problem.
    Find paths through the system from 'start' to 'end' that don't
    pass through small caves (lower-case) more than once.
    Part 1: Count number of paths.

    Part 2: TBD

Part 1 Plan:
    What we are given are connections between nodes in a graph. Reading
    the input file should generate a graph struct, which consists of
    an array of nodes and an array of connections.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define PRINT_DEBUG
// arbitrarily large
#define MAX_CAVES 127

struct Cave {
    char name[6];       // longest cave name is 'start'
};

struct Tunnel {
    // a tunnel can only connect two caves
    struct Cave caves[2];
};

struct CaveNetwork {            // graph
    struct Cave *caves[MAX_CAVES];         // nodes
    int *tunnels;           // connections
    unsigned num_caves;
    unsigned num_tunnels;
};

/*
* Read and input file into the buffer
*
* @param    file_name           input file to read
* @param    p_file_size_ret     pointer to file size to return
* @retval   input_buffer        file contents.
*/
char* read_input(char file_name[], int *p_num_rows)
{
    FILE *file_handle = fopen(file_name, "r");
    if (file_handle == NULL) {
        printf("Error opening file %s: %s.\n", file_name, strerror(errno));
        exit(-1);
    }

    fseek(file_handle, 0, SEEK_END);
    size_t file_size = ftell(file_handle);
    rewind(file_handle);

    //printf("File size %ld.\n", file_size);
    char *input_buffer = (char*)malloc(file_size + 1);
    if (input_buffer == NULL) {
        printf("Error allocating memory to input buffer. Buffer size %ld bytes.\n",
                file_size);
        exit(-1);
    }

    int bytes_read = fread((void*)input_buffer, sizeof(unsigned char),
            file_size, file_handle);

    if (bytes_read != file_size) {
        printf("Error reading %s: %s. Expected %ld bytes, read %d bytes.\n",
                file_name, strerror(errno), file_size, bytes_read);
        exit(-1);
    }

    char tmp;
    rewind(file_handle);
    while ((tmp = fgetc(file_handle)) != EOF) {
        // every row has a '-', regardless of \n or \0
        if (tmp == '-') {
            (*p_num_rows)++;
        }
    }
    fclose(file_handle);

    // store a zero at the end so we know when to quit
    input_buffer[file_size] = 0;
    
    return input_buffer;
}

/*
* Determine whether a cave is big or small
* @param    p_cave_t    pointer to cave
* @retval   1           cave is big
* @retval   0           cave is small
*/
int is_big(struct Cave *p_cave_t)
{
    return (isupper(p_cave_t->name[0]));
}

/*
* Create a cave given a name string.
*
* @param    cave_name       name of cave
* @param    p_network_t     pointer to network that cave belongs to
* @retval   cave_index      index of cave in p_network_t->caves
*/
int Cave_create(char cave_name[], int name_len, struct CaveNetwork *p_network_t)
{
    // check if cave already exists in network
    int cave_index;
    for (cave_index = 0; cave_index < p_network_t->num_caves;
            cave_index++) {
        // strcmp returns 0 if strings match
        if (!strncmp(p_network_t->caves[cave_index]->name,
                    (char *)cave_name, name_len)) 
            return cave_index;
    }

    struct Cave *p_cave_t_ret = malloc(sizeof(struct Cave));
    if (p_cave_t_ret == NULL) {
        printf("Error allocating data for cave: %s\n", strerror(errno));
        exit(-1);
    }
    strcpy(p_cave_t_ret->name, cave_name);

    p_network_t->caves[p_network_t->num_caves] = p_cave_t_ret;
    p_network_t->num_caves++;

    return cave_index;
}

/*
* Read an input file of connections to create a cave network.
*
* @param    file_name           data file to open
* @retval   p_network_t_ret     pointer to cave network
*/
struct CaveNetwork *Network_create(char file_name[])
{
    // read the input file
    int num_rows = 0;
    char *input_buffer = read_input(file_name, &num_rows);
    if (input_buffer == NULL) {
        printf("Error reading file %s: %s.\n", 
                file_name, strerror(errno));
        exit(-1);
    }

    // allocate memory for the cave network
    struct CaveNetwork *p_network_t_ret = malloc(sizeof(struct CaveNetwork));
    p_network_t_ret->num_caves = 0;
    p_network_t_ret->num_tunnels = num_rows;

    char *p_cave_name = input_buffer;
    int index = 0;
    // connections array sized such that each
    // pair of indices (e.g. 0-1 and 2-3) represents a connection
    p_network_t_ret->tunnels = malloc(p_network_t_ret->num_tunnels * 2 * sizeof(int));
    int name_len;

    while (*input_buffer != 0) {
        if (!isupper(*input_buffer) && !islower(*input_buffer)) {
            // determine the length of the name
            name_len = input_buffer - p_cave_name;
            // reserve memory for the cave name
            char cave_name[name_len];
            // copy the name into memory
            strncpy(cave_name, p_cave_name, name_len);
            // add null terminator
            cave_name[name_len] = '\0';
            // create a new cave
            p_network_t_ret->tunnels[index++] = Cave_create(cave_name,
                    name_len, p_network_t_ret);
            // move to the next cave
            p_cave_name = input_buffer + 1;
        }
        input_buffer++;
    }

    return p_network_t_ret;
}

/*
* Free the data from a cave network
*
* @param    p_network           pointer to cave network to free
*/
void Network_destroy(struct CaveNetwork *p_network)
{
    /*p_network->caves = NULL;*/
    /*free(p_network->caves);*/
    /*p_network->tunnels = NULL;*/
    /*free(p_network->tunnels);*/
    p_network = NULL;
    free(p_network);
}

/*
* Print data about a cave network
*
* @param    p_network       pointer to the network to describe
*/
void CaveNetwork_info(struct CaveNetwork *p_network)
{
    if (p_network == NULL) {
        printf("Bad network requested.\n");
        exit(-1);
    }
    printf("Cave network with %u caves and %u tunnels.\n", 
            p_network->num_caves, p_network->num_tunnels);
    printf("Caves: ");
    for (int cave_index = 0; cave_index < p_network->num_caves; cave_index++) {
        printf("%s ", p_network->caves[cave_index]->name);
        if (is_big(p_network->caves[cave_index]))
            printf("(BIG) ");
    }
    printf("\n");
    for (int i = 0; i < p_network->num_tunnels * 2; i += 2) {
        printf("%s -> %s\n", p_network->caves[p_network->tunnels[i]]->name,
                p_network->caves[p_network->tunnels[i+1]]->name);
    }
}

int main(int argc, char *argv[])
{
    struct CaveNetwork *test = Network_create("data/12med");
    CaveNetwork_info(test);
    Network_destroy(test);
    return 0;
}
