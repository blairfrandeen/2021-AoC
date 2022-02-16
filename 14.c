/*
Advent of Code Day 14
https://adventofcode.com/2021/day/14

Given a polymer template and pair insertion rules, run the model
for n number of steps. Find the count of each element after n steps.

Template = NNCB
Three pairs: NN, NC, CB
Applying insertion rules NN->C, NC->B and CB->H we get pairs
NC CN NB BC CH HB
contracts to
NCNBCHB

Next step:
NB BC CC CN NB BB BB BC CB BH HC CB
Contracts to
NBCCNBBBCBHCB
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define MAX_TEMPLATE 127
#define MAX_ELEMENTS 26     // only A-Z allowed
#define MAX_RULES 255         // number of chars per rules (AB -> C\n)

/* Each pair rule has three attributes:
   Its name (i.e. "NC")
   The character that gets inserted (i.e. 'N')
   Pointers to the two rules that follow ("NN" and "CN")
*/
typedef struct pairrule_t {
    char pair[3];
    char insertion;
    struct pairrule_t *child1, *child2;
} pairrule_t;

typedef struct polymer_t {
    int num_rules;
    pairrule_t **rules;
    uintmax_t *rule_counts;

    int num_elements;
    char elements[MAX_ELEMENTS];
    uintmax_t element_counts[MAX_ELEMENTS];

    char template[MAX_TEMPLATE];
} polymer_t;

int find_rule_index(polymer_t *polymer, char pair[])
{
    for (int rule_index = 0; rule_index < polymer->num_rules; rule_index++) {
        if (!strcmp(pair, polymer->rules[rule_index]->pair)) {
            return rule_index;
        }
    }
    return -1;
}

void create_rule(polymer_t *polymer, char pair[], char insertion)
{
    //printf("Creating rule %s...\n", pair);
    int rule_index = find_rule_index(polymer, pair);
    if (rule_index >= 0) {
        polymer->rules[rule_index]->insertion = insertion;
    } else {
        pairrule_t *rule = (pairrule_t*)malloc(sizeof(pairrule_t));
        strcpy(rule->pair, pair);
        rule->insertion = insertion;
        rule->child1 = rule->child2 = NULL;
        polymer->rules[polymer->num_rules] = rule;
    }
}

void link_rules(polymer_t *pm)
{
    for (int rule_index = 0; rule_index < pm->num_rules; rule_index++) {
        char child_1[3], child_2[3];
        child_1[0] = pm->rules[rule_index]->pair[0];
        child_1[1] = child_2[0] = pm->rules[rule_index]->insertion;
        child_2[1] = pm->rules[rule_index]->pair[1];
        child_1[2] = child_2[2] = '\0';
        pm->rules[rule_index]->child1 = 
            (struct pairrule_t*)pm->rules[find_rule_index(pm, child_1)];
        pm->rules[rule_index]->child2 = 
            (struct pairrule_t*)pm->rules[find_rule_index(pm, child_2)];
        pm->rule_counts[rule_index] = 0;
    }
}

/*
* Find the index of an element in a polymer.
*
* @param    element     element to look for
* @param    polymer_t   polymer to look in
* @retval   index       index of polymer
* @retval   -1          polymer not found.
*/
int element_index(char element, polymer_t *p_poly)
{
    for (int index = 0; index < p_poly->num_elements; index++) {
        if (p_poly->elements[index] == element) {
            return index;
        }
    }
    return -1;
}

void print_polymer(polymer_t *pm)
{
    for (int i = 0; i < pm->num_elements; i++) {
        printf("%c - %llu\n", pm->elements[i], pm->element_counts[i]);
    }
    //for (int rule_index = 0; rule_index < pm->num_rules; rule_index++) {
        //printf("Rule: %s -> %c (%dX), Children: %s, %s.\n",
                //pm->rules[rule_index]->pair,
                //pm->rules[rule_index]->insertion,
                //pm->rule_counts[rule_index],
                //pm->rules[rule_index]->child1->pair,
                //pm->rules[rule_index]->child2->pair);
    //}
}

void element_count_range(polymer_t *pm)
{
    uintmax_t max_count = pm->element_counts[0], 
                  min_count = pm->element_counts[0];
    for (int i = 1; i < pm->num_elements; i++) {
        uintmax_t current_element = pm->element_counts[i];
        if (current_element > max_count) {
            max_count = current_element;
        }
        if (current_element < min_count) {
            min_count = current_element;
        }
    }
    printf("Min: %llu\nMax: %llu\n", min_count, max_count);
    printf("Result: %llu\n", (max_count - min_count));
}
/*
* Create a polymer given a template.
* @param    template        template string
* @param    n               length of template string
* @retval   pm              pointer to the template
*/
polymer_t* create_polymer(char template[], size_t *template_length, char *input_buffer)
{
    polymer_t *pm = malloc(sizeof(polymer_t));
    pm->num_elements = 0;
    pm->num_rules = 0;
    strcpy(pm->template, template);


    pm->rules = (pairrule_t**)malloc(sizeof(pairrule_t*) * MAX_RULES);
    pm->rule_counts = (uintmax_t*)malloc(sizeof(uintmax_t*) * MAX_RULES);
    char delimeters[] = " ->\n";
    char *token = NULL;
    token = strtok(input_buffer, delimeters);
    while(token != NULL) {
        char rule_pair[8];
        strcpy(rule_pair, token);
        token = strtok(NULL, delimeters);
        char rule_insertion = token[0];
        create_rule(pm, rule_pair, rule_insertion);
        pm->num_rules++;
        token = strtok(NULL, delimeters);
        //printf("%s -> %c\n", rule_pair, rule_insertion);
    }
    free(input_buffer);

    link_rules(pm);

    for (int template_index = 0; template_index < *template_length; template_index++) {
        char element = template[template_index];
        if (template_index < (*template_length - 1)) {
            char next = template[template_index + 1];
            char pair[3] = { element, next, '\0' };
            pm->rule_counts[find_rule_index(pm, pair)]++;
        }

        int current_index = element_index(element, pm);
        if (current_index >= 0) {
            pm->element_counts[current_index]++;
            continue; 
        } else {
            pm->elements[pm->num_elements] = element;
            pm->element_counts[pm->num_elements] = 1;
            pm->num_elements++;
        }
    }

    return pm;
}

void grow_polymer(polymer_t *pm, int num_steps)
{
    for (int step = 0; step < num_steps; step++) {
        uintmax_t new_element_counts[pm->num_elements];
        memset(new_element_counts, 0, pm->num_elements * sizeof(uintmax_t));
        uintmax_t new_rule_counts[pm->num_rules];
        memset(new_rule_counts, 0, pm->num_rules * sizeof(uintmax_t));

        for (int rule_index = 0; rule_index < pm->num_rules; rule_index++) {
            pairrule_t *current_rule = pm->rules[rule_index];
            uintmax_t num_current_rule = pm->rule_counts[rule_index];
            int new_element_index, new_rule_index;
            if ((new_element_index = element_index(current_rule->insertion, pm)) >= 0) {
                new_element_counts[new_element_index] += num_current_rule;
            } else {
                pm->elements[pm->num_elements] = current_rule->insertion;
                pm->element_counts[pm->num_elements] = 0;
                new_element_counts[pm->num_elements] = num_current_rule;
                pm->num_elements++;
            }
            if ((new_rule_index = find_rule_index(pm, current_rule->child1->pair)) >= 0) {
                new_rule_counts[new_rule_index] += num_current_rule;
            }
            if ((new_rule_index = find_rule_index(pm, current_rule->child2->pair)) >= 0)
                new_rule_counts[new_rule_index] += num_current_rule;
            pm->rule_counts[rule_index] = 0;
        }

        for (int i = 0; i < pm->num_elements; i++) {
            pm->element_counts[i] += new_element_counts[i];
        }
        for (int i = 0; i < pm->num_rules; i++) {
            pm->rule_counts[i] += new_rule_counts[i];
        }

    }
}

/*
* Free a polymer from memory.
* @param    polymer_t   polymer to free.
*/
void free_polymer(polymer_t *pm)
{
    for(int i = 0; i < pm->num_rules; i++) {
        free(pm->rules[i]);
    }
    free(pm->rules);
    free(pm->rule_counts);
    free(pm);
}

/*
* Read input from a data file.
* Assume first row is a template, second row is blank
* All other rows are pair rules separated by " -> "
*
* @param    data_file       the data file to read
* @param    p_template      pointer to the template buffer
* @retval   num_rules       number of rules found
*/
char* read_input(char data_file[], char *p_template, size_t *template_length)
{
    FILE *file_handle = fopen(data_file, "r");
    if (file_handle == NULL) {
        printf("Error opening %s: %s.\n", data_file, strerror(errno));
        exit(EXIT_FAILURE);
    }


    // first line is the template
    fgets(p_template, MAX_TEMPLATE * sizeof(char), file_handle);
    *template_length = ftell(file_handle) - 1; // ignore newline
    p_template[*template_length] = '\0';
    //printf("Template length %lu bytes\n", template_length);

    // get the remaining file size
    fseek(file_handle, 0, SEEK_END);
    size_t file_size = ftell(file_handle);
    fseek(file_handle, *template_length + 1, SEEK_SET);

    // get the rest of the input into a buffer
    char *input_buffer = (char *)malloc(file_size * sizeof(char) + 1);
    size_t bytes_read = fread(input_buffer, sizeof(char), file_size, file_handle);
    if (bytes_read != (file_size - *template_length - 1)) {
        printf("Error reading input buffer. Expected %lu, read %lu.\n",
                (file_size - *template_length - 1), bytes_read);
        exit(EXIT_FAILURE);
    }

    fclose(file_handle);
    input_buffer[file_size] = '\0';

    return input_buffer;
}

int main(int argc, char *argv[])
{
    char template[MAX_TEMPLATE];
    size_t template_length;
    char *input_buffer = read_input("data/14data", template, &template_length);
    polymer_t *pm = create_polymer(template, &template_length, input_buffer);
    grow_polymer(pm, 40);
    print_polymer(pm);
    element_count_range(pm);
    free_polymer(pm);
    return (EXIT_SUCCESS);
}
