#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "../include/process_details.h"
#include "../include/string_handler.h"

int process_details(const char *pid);
int check_proc_exists(const char *pid_path);

/*
Function: process_details
Description: This is the main control function that goes out and fetches information from other functions within this file
- Before it starts the process of collecting the process information it checks to see if the pid exists!
    - This isn't a conclusive check as it may disappear by the time we start then attempting to parse out the information for the pid.
*/
int process_details(const char *pid) {

    char *pid_path = format_procpth(pid);

    if(check_proc_exists(pid_path) != 1) {
        printf("Error the specified process doesn't exist...\n");
        exit(1);
    }

    procstat_info *procstatInfo = (procstat_info *)malloc(sizeof(procstat_info));
    if(procstatInfo == NULL) {
        perror("Error: ");
        exit(1);
    }

    if(extract_proc_stat(pid_path, procstatInfo) == -1) {
        perror("Error: ");
        exit(1);
    }

    free(pid_path);
    return -1;
}

/*
Function: check_proc_exists
Description: To validate directory exists it simply tries to open it and returns success or failure.
*/
int check_proc_exists(const char *pid_path) {

    DIR *dirent;
    int ret_value = -1;

    dirent = opendir(pid_path);
    if(dirent != NULL)
        ret_value = 1;

    return ret_value;
}


/*
Function Name: extract_proc_stat
Description: Uses the /proc/pid/stat file to parse out information into the procstat struct.
- Extracts the comm name seperately and reformats the line for parsing
    - This is done to mitigate any errors caused by splitting on space.
*/
int extract_proc_stat(const char *proc_path, procstat_info *procstatInfo) {

    char *statpth = format_filepth(proc_path, "stat");
    int ret_value = -1;
    FILE *statfp;
    statfp = fopen(statpth,"r");

    char *stat_line = get_next_line(statfp);

    char *mod_stat_line = extract_stat_comm(stat_line, procstatInfo);

    if(mod_stat_line == NULL) {
        free(stat_line);
        return ret_value;
    }

    int segm_id = 0;
    char *segm = strtok(mod_stat_line, " ");
    //As the comm element has been removed we substract -2 after position 0 to get the index of the value from man proc.
    while( segm != NULL ) {
        switch(segm_id) {
            case 0:
                procstatInfo->pid = atoi(segm);
            case 1:
                procstatInfo->state = segm[0];
            case 2:
                procstatInfo->ppid = atoi(segm);
            case 3:
                procstatInfo->pgrp = atoi(segm);
            case 20:
                procstatInfo->starttime = strtoull(segm, NULL, 10);
            case 21:
                procstatInfo->vsize = strtoul(segm, NULL, 10);
        }
        segm_id++;
        segm = strtok(NULL, " ");
    }

    printf("Name: %s\nPID: %d\nState: %c\nPPID: %d\nMem Size %lu\n", procstatInfo->comm, procstatInfo->pid, procstatInfo->state, procstatInfo->ppid, procstatInfo->vsize);

    free(stat_line);
    free(statpth);
    return 0;
}
