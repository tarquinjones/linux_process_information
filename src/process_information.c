#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/process_details.h"

/*
Linux process listing
- User parses in a pid they want detailed information on. (Likely from ps or using the process listing tool)
- Tool then extracts the following information from /proc
    - Basic process information similar to process listing tool
    - Displays the environment variables set for the process
    - Displays the memory maps for the process
    - Displays the open FDs for the process

*/

void usage()
{
    printf("./process_information [-p pid]\n");
    exit(-1);
}

/*
Program entry point
*/
int main(int argc, char **argv)
{
    if(argc < 3)
    {
        usage();
    }

    if(strncmp(argv[1], "-p", 2) != 0)
    {
        usage();
    }

    //Last check to see if the pid argument is an integer
    if(atoi(argv[2]) == 0)
    {
        usage();
    }

    if(process_details(argv[2]) == -1)
    {
        printf("Something went wrong..\n");
    }
}
