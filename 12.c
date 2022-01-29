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
#include <stdint.h>

#define PRINT_DEBUG
// arbitrarily large
#define MAX_CAVES 127

struct Cave {
    char name[5];       // longest cave name is 'start'
    uint8_t is_big;
};

struct Tunnel {
    // a tunnel can only connect two caves
    struct Cave caves[2];
};

struct CaveNetwork {            // graph
    struct Cave *caves[MAX_CAVES];         // nodes
    struct Tunnel *tunnels;     // connections
    unsigned num_caves;
    unsigned num_tunnels;
};

/*
* Create a cave given a name string.
*
* @param    cave_name       name of cave
* @param    p_network_t     pointer to network that cave belongs to
* @retval   0               cave created
* @retval   1               no cave created
*/
int Cave_create(char cave_name[], struct CaveNetwork *p_network_t)
{
    // check if cave already exists in network
    for (int cave_index = 0; cave_index < p_network_t->num_caves; cave_index++) {
        // strcmp returns 0 if strings match
        printf("Attempting to create %s in netowrk with %d caves.\n",
                cave_name, p_network_t->num_caves);
        if (!strcmp(p_network_t->caves[cave_index]->name, (char *)cave_name)) {
            printf("Cave %s already exists!\n", cave_name);
            return 1;
        }
    }

    struct Cave *p_cave_t_ret = malloc(sizeof(struct Cave));
    strcpy(p_cave_t_ret->name, (char *)cave_name);

    // check if cave is big or not
    // if first letter is upper case, we consider it big
    if (cave_name[0] >= 'A' && cave_name[0] <= 'Z') {
        p_cave_t_ret->is_big = 0;
    } else {
        p_cave_t_ret->is_big = 1;
    }
    p_network_t->caves[p_network_t->num_caves] = p_cave_t_ret;
    p_network_t->num_caves++;

    return 0;
}

/*
* Read and input file into the buffer
*
* @param    file_name           input file to read
* @param    p_file_size_ret     pointer to file size to return
* @retval   input_buffer        file contents.
*/
unsigned char* read_input(char file_name[], size_t *p_file_size_ret)
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
    unsigned char *input_buffer = (unsigned char*)malloc(file_size + 1);
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
    fclose(file_handle);

    // store a zero at the end so we know when to quit
    input_buffer[file_size] = 0;
    *p_file_size_ret = file_size;

    return input_buffer;
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
    size_t file_size;
    unsigned char *input_buffer = read_input(file_name, &file_size);
    if (input_buffer == NULL || file_size == 0) {
        printf("Error reading file %s: %s.\n", file_name, strerror(errno));
        exit(-1);
    }

    // allocate memory for the cave network
    struct CaveNetwork *p_network_t_ret = malloc(sizeof(struct CaveNetwork));
    p_network_t_ret->num_caves = 0;
    p_network_t_ret->num_tunnels = 0;

    unsigned char *cave_name = input_buffer;

    while (*(input_buffer++) != 0) {
        if (*input_buffer == '-' || *input_buffer == '\n') {
            int name_len = input_buffer - cave_name; 
            char *new_name = strncpy(new_name, (char *)cave_name, name_len);
            Cave_create(new_name, p_network_t_ret);
            cave_name = input_buffer + 1;
        }

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
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    struct CaveNetwork *test = Network_create("data/12small");
    CaveNetwork_info(test);
    Network_destroy(test);
    return 0;
}
