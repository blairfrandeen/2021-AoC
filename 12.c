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
        if (strcmp(p_network_t->caves[cave_index]->name, cave_name)) {
            printf("Cave %s already exists: %s.\n", cave_name, p_network_t->caves[cave_index]->name);
            return 1;
        }
    }
    struct Cave *p_cave_t_ret = malloc(sizeof(struct Cave));
    strcpy(p_cave_t_ret->name, cave_name);
    p_cave_t_ret->is_big = 0;
    p_network_t->caves[p_network_t->num_caves] = p_cave_t_ret;
    p_network_t->num_caves++;
    return 0;
}
/*
* Read an input file of connections to create a cave network.
*
* @param    file_name           data file to open
* @retval   p_network_t_ret     pointer to cave network
*/
struct CaveNetwork *Network_create(char file_name[])
{
    FILE *file_handle = fopen(file_name, "r");
    if (file_handle == NULL) {
        printf("Error opening file %s: %s.\n", file_name, strerror(errno));
        exit(-1);
    }

    struct CaveNetwork *p_network_t_ret = malloc(sizeof(struct CaveNetwork));
    p_network_t_ret->num_caves = 0;
    Cave_create("start", p_network_t_ret);
    Cave_create("end", p_network_t_ret);
    Cave_create("start", p_network_t_ret);
    p_network_t_ret->num_tunnels = 0;

    char input_buffer[10];      // longest possible line is "start->end\n"

    // read through the input file
    char current_char;
    int cave_name_len = 0;
    char cave_name[5];
    while((current_char = fgetc(file_handle)) != EOF) {
        cave_name[cave_name_len++] = current_char;
        if (current_char == '\n') {
            // goto next line
            p_network_t_ret->num_tunnels++;
            cave_name_len = 0;
            continue;
        } else if (current_char == '-') {
            // goto next cave
            cave_name_len = 0;
            continue;
        } else {
        }
    }
    /*while(fgets(input_buffer, sizeof(input_buffer), file_handle)) {*/
        /*p_network_t_ret->num_tunnels++;*/
        /*int buffer_index = 0;*/
        /*char current_char;*/
        /*while ((current_char = input_buffer[buffer_index++]) != '\n') {*/
            /*// skip the '-' and go to the next cave*/
            /*if (current_char == '-') {*/
                /*printf(":\t Cave signature: %d.\n", cave_signature);*/
                /*num_chars = 0;*/
                /*cave_signature = 0;*/
                /*continue; */
            /*}*/
            /*cave_signature += (current_char - '0') << ++num_chars;*/
            /*printf("%c", current_char);*/
        /*}*/
        /*printf(":\t Cave signature: %d.\n", cave_signature);*/
        /*cave_signature = 0;*/
        /*num_chars = 0;*/
    /*}*/

    fclose(file_handle);

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
    printf("Cave network with %u tunnels.\n", p_network->num_tunnels);
}

int main(int argc, char *argv[])
{
    struct CaveNetwork *test = Network_create("data/12small");
    CaveNetwork_info(test);
    Network_destroy(test);
    return 0;
}
