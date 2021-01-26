#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../include/process_details.h"
#include "../include/process_information.h"

/*
Linux process listing
- User parses in a pid they want detailed information on. (Likely from ps or using the process listing tool)
- Tool then extracts the following information from /proc
    - Basic process information similar to process listing tool - DONE
    - Displays the environment variables set for the process - DONE
    - Displays the memory maps for the process - DONE
    - Displays the open FDs for the process - DONE
    - Displays the exe path - DONE
    - Convert process start time - TO DO
    - Handle NULL exe path - TO DO
    - Use bit fields to parse options at cmd line - DONE
*/

void usage()
{
    printf("./process_info [-p pid] [-b] [-m] [-f] [-e] [-a]\n");
    exit(-1);
}

void help()
{
    printf("./process_info\n");
    printf("\t -p: Mandatory. Specify the PID you want more information on\n");
    printf("\t -b: Returns basic information about the process\n");
    printf("\t -e: Returns the environment variables set for the process\n");
    printf("\t -f: Returns the file descriptors for the process\n");
    printf("\t -m: Returns the memory maps for the process\n");
    printf("\t -a: Runs all of the options above!\n");
    exit(0);
}

int process_args(int argc, char **argv, char **pid)
{
    extern char *__progname;
    u_int8_t options = 0;
    for(int i = 0; i < argc; i++)
    {
        if(strncmp(__progname, argv[i], strlen(__progname)) != 0)
        {
            if(strncmp(argv[i], "-m",2) == 0) {
                options = options|MAPS;
            }
            if(strncmp(argv[i], "-e",2) == 0)
                options = options | ENVIRON;
            if(strncmp(argv[i], "-f",2) == 0)
                options = options | FDS;
            if(strncmp(argv[i], "-b",2) == 0)
                options = options | BASIC;
            if(strncmp(argv[i], "-a",2) == 0)
                options = ALL;
            if(strncmp(argv[i], "-h",2) == 0)
                help();
            if(strncmp(argv[i], "-p",2) == 0) {
                // First we need to check if the input is an integer
                char *tmp_pid = strdup(argv[i+1]);
                for(int i = 0; i < strlen(tmp_pid); i++) {
                    if(isdigit(tmp_pid[i]) == false) {
                        fprintf(stderr,"ERROR: PID needs to be an int...\n");
                        usage();
                    }
                }
                *pid = argv[i+1];
                free(tmp_pid);
            }
        }
    }

    if(options == 0)
        help();

    return options;
}

/*
Program entry point
*/
int main(int argc, char **argv)
{
    char *pid = 0;
    //Process the options before continuing
    u_int8_t options = process_args(argc, argv, &pid);

    if(process_details(pid, options) == -1)
    {
        printf("Something went wrong..\n");
    }
}
