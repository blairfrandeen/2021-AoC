#include <stdio.h>
#include <stdlib.h>

int main()
{
    //int d[] = {199,200,208,210,100,207,240,269,260,263};
    char file_name[] = "data/depths.txt";
    char buff[8];

    FILE *file_pointer = fopen(file_name, "r");

    int previous_depth = 0;
    int descents = 0;
    int depth = 0;
    while(fgets(buff, sizeof buff, file_pointer) != NULL) {
        depth = atoi(buff);
        if (depth > previous_depth && previous_depth != 0) {
            descents++;
        }
        previous_depth = depth;
    }

    printf("We have descented %d times.\n", descents);

    fclose(file_pointer);
    return 0;
}
