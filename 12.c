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

    start
    /   \
c--A-----b--d
    \   /
     end
   From start, two paths: start-A, start-b
   From A, 3 paths: start-A-c, start-A-b, start-Aend
   From b, 3 paths: start-b-A, start-b-d, start-b-end
   from d, 1 path: start-b-d-b - this should invalidate, since b visited twice
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define PRINT_DEBUG
// arbitrarily large
#define MAX_CAVES 128

struct Cave {
    char name[8];       // longest cave name is 'start'
    int num_tunnels;
    int tunnels[MAX_CAVES - 1];
};

struct CaveNetwork {            // graph
    struct Cave *caves[MAX_CAVES];         // nodes
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
    p_cave_t_ret->num_tunnels = 0;

    p_network_t->caves[p_network_t->num_caves] = p_cave_t_ret;
    p_network_t->num_caves++;

    return cave_index;
}

/*
* Given a CaveNetwork and a list of connections, assign connections
* to the Cave structs, so that a Cave may be queried for its connections
* 
* @param    p_network_t     the cave network
* @param    p_tunnels       An integer array of cave indices representing tunnels.
*                           Pairs of indices represent connections
* @retval   0               Success
* @retval   1               Error assigning tunnels
*/
void Network_assign_tunnels(struct CaveNetwork *p_network_t, int *p_tunnels)
{
    if (p_network_t == NULL) {
        printf("Error: NULL Cave Network: %s\n", strerror(errno));
        exit(-1);
    }
    if ((*p_tunnels % 2) != 0) {
        printf("Error: Uneven number of tunnels!\n");
        exit(-1);
    }

    // loop through the tunnels array two at a time
    for (int i = 0; i < p_network_t->num_tunnels * 2; i += 2) {
        // create pointers to the cave pairs
        struct Cave *p_cave_1 = p_network_t->caves[p_tunnels[i]];
        struct Cave *p_cave_2 = p_network_t->caves[p_tunnels[i+1]];

        // Assign tunnels to opposite caves
        p_cave_1->tunnels[p_cave_1->num_tunnels] = p_tunnels[i+1];
        p_cave_2->tunnels[p_cave_2->num_tunnels] = p_tunnels[i];

        p_cave_1->num_tunnels++;
        p_cave_2->num_tunnels++;
    }
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

    // create "start" and "end" caves to ensure they
    // are at known indices
    Cave_create("start", 5, p_network_t_ret);   // index 0
    Cave_create("end", 3, p_network_t_ret);     // index 1

    char *p_cave_name = input_buffer;

    // connections array sized such that each
    // pair of indices (e.g. 0-1 and 2-3) represents a connection

    // each row in the data file represents one tunnel between two caves
    int tunnels[2 * num_rows];
    int tunnel_index = 0;

    // preserve the input_buffer pointer so we can free it
    char *next_char = input_buffer;

    while (*next_char != 0) {
        if (!isupper(*next_char) && !islower(*next_char)) {
            // determine the length of the name
            int name_len = next_char - p_cave_name;
            // reserve memory for the cave name
            char cave_name[name_len];
            // copy the name into memory
            strncpy(cave_name, p_cave_name, name_len);
            // add null terminator
            cave_name[name_len] = '\0';
            // create a new cave
            tunnels[tunnel_index++] = Cave_create(cave_name,
                    name_len, p_network_t_ret);
            // move to the next cave
            p_cave_name = next_char + 1;
        }
        next_char++;
    }

    free(input_buffer);

    // assign the tunnels to the caves
    Network_assign_tunnels(p_network_t_ret, tunnels);

    return p_network_t_ret;
}

/*
* Free the data from a cave network
*
* @param    p_network           pointer to cave network to free
*/
void Network_destroy(struct CaveNetwork *p_network)
{
    //free(p_network->tunnels);
    for (int cave_index = 0; cave_index < p_network->num_caves;
            cave_index++) {
        free(p_network->caves[cave_index]);
    }
    free(p_network);
}

/*
* Print data about a cave network
*
* @param    p_network       pointer to the network to describe
*/
void CaveNetwork_info(struct CaveNetwork *p_network)
{
#ifdef PRINT_DEBUG
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
    for (int i = 0; i < p_network->num_caves; i++) {
        printf("Cave %s connected to: ", p_network->caves[i]->name);
        for (int j = 0; j < p_network->caves[i]->num_tunnels; j++) {
            printf("%s ", p_network->caves[p_network->caves[i]->tunnels[j]]->name);
        }
        printf("\n");
    }
#endif
}

/*
* Determine whether a cave index is already in a path
*
* @param    index           cave index to check for
* @param    p_path          path to look in
* @param    p_path_length   length of path
*/
int is_in_path(int index, int *p_path, int *p_path_length)
{
    for (int p_path_index = 0; p_path_index < *p_path_length; p_path_index++) {
        if (index == p_path[p_path_index])
            return 1;
    }
    return 0;
}

 /*
* Search a cave network for all possible paths from start to end
*
* @param    start_index     index at which to start the search
* @param    end_index       index at which paths complete
* @param    p_current_path    current path being investigated
* @param    p_path_length     length of current path
* @param    p_num_paths       number of paths through the network
* @param    p_network_t     pointer to the cave network
*/
void Network_find_paths(int start_index, int end_index, int *p_current_path,
        int *p_path_length, int *p_num_paths, int *p_small_visit, struct CaveNetwork *p_network_t)
{
    // pointer to cave we're starting at
    struct Cave *p_start = p_network_t->caves[start_index];

    // keep recursing as long as we're not in the final cave
    if (start_index != end_index) {
        // look at all the caves connected to this one
        for (int tunnel_index = 0; tunnel_index < p_start->num_tunnels; tunnel_index++) {
            // pointer to the cave we're looking towards
            int next_index = p_start->tunnels[tunnel_index];
            struct Cave *p_next = p_network_t->caves[next_index];
            // if we've already been there
            if (is_in_path(next_index, p_current_path, p_path_length)) {
                // skip if it's a start or end cave
                if (next_index == 0 || next_index == 1)
                    continue;
                // if it's a small cave
                if (!is_big(p_next)) {
                    //if (*p_small_visit == 1)
                        continue;
                    //*p_small_visit = 1;
                }
            }
            // otherwise add the connected cave to the path
            p_current_path[(*p_path_length)++] = next_index;

            // search starting at the next cave
            Network_find_paths(next_index, 1, p_current_path,
                    p_path_length, p_num_paths, p_small_visit, p_network_t);

            // once the recursion finishes, go back to the previous position
            *p_path_length -= 1;
        }
    } else { // if we're at the last cave
#ifdef PRINT_DEBUG
        printf("Found path: ");
        for (int path_index = 0; path_index < *p_path_length; path_index++) {
            printf("%s ", p_network_t->caves[p_current_path[path_index]]->name);
        }
        printf("\n");
#endif
        (*p_small_visit) = 0;   // reset small visit counter
        (*p_num_paths)++;       // add to the count of paths
    }
}

/*
* Count the paths through a cave network
*
* @param    data_file   data file to open
* @retval   num_paths   total number of paths
*/
int Network_count_paths(char data_file[])
{

    struct CaveNetwork *network = Network_create(data_file);
    CaveNetwork_info(network);
    int path[MAX_CAVES];
    path[0] = 0;
    int path_length = 1;
    int num_paths = 0;
    int small_visit = 0;

    Network_find_paths(0, 1, path, &path_length, &num_paths, &small_visit, network);
    printf("There are %d paths through network described by '%s'.\n",
            num_paths, data_file);

    Network_destroy(network);
    return num_paths;
}

int main(int argc, char *argv[])
{
    Network_count_paths("data/12small");
    //Network_count_paths("data/12med");
    //Network_count_paths("data/12large");
    //Network_count_paths("data/12puzzle");
    return 0;
}
