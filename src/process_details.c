#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "../include/process_details.h"
#include "../include/string_handler.h"
#include "../include/output_handler.h"

int process_details(const char *pid);
int check_proc_exists(const char *pid_path);
int extract_proc_environ(const char *proc_path, char **environ);

/*
Function: process_details
Description: This is the main control function that goes out and fetches information from other functions within this file
- Before it starts the process of collecting the process information it checks to see if the pid exists!
    - This isn't a conclusive check as it may disappear by the time we start then attempting to parse out the information for the pid.
- Also worth noting that we may get half way through collecting the information and the proc disappears so we need to handle this...
*/
int process_details(const char *pid)
{
    char *environ = NULL;
    char *maps = NULL;

    char *pid_path = format_procpth(pid);

    if(check_proc_exists(pid_path) != 1)
    {
        printf("Error the specified process doesn't exist...\n");
        exit(1);
    }

    int procstat, proccmdline, procenviron, procexe, procmaps = 0;
    proc_info *procInfo = (proc_info *)malloc(sizeof(proc_info));
    if(procInfo == NULL)
    {
        perror("Error: ");
        exit(1);
    }

    /*
    TO DO USE BIT FIELDS FOR THIS SO ONE VALUE CAN BE PARSED TO DETERMINE THE OUTPUT
    */
    //Get proc stat information
    if(extract_proc_stat(pid_path, procInfo) != -1)
    {
        procstat = 1;
    }

    if(extract_proc_cmdline(pid_path, procInfo) != -1) {
        proccmdline = 1;
    }

    if(get_proc_exe(pid_path, procInfo) != -1)
    {
        procexe = 1;
    }

    if(extract_proc_environ(pid_path, &environ) != -1) {
        procenviron = 1;
    }

    if(get_proc_maps(pid_path, &maps) != -1) {
        procmaps = 1;
    }

    /*
    Output is determined on what we were able to fetch
    */
    print_proc_basic_output(procInfo);

    if(procstat == 1)
        free(procInfo->comm);

    if(proccmdline == 1)
        free(procInfo->cmdline);
    free(procInfo);

    if(procenviron == 1)
    {
        print_proc_environ(environ);
        free(environ);
    }

    if(procmaps == 1)
    {
        print_proc_maps(maps);
        free(maps);
    }


    free(pid_path);
    return 0;
}


/*
Function: check_proc_exists
Description: To validate directory exists it simply tries to open it and returns success or failure.
*/
int check_proc_exists(const char *pid_path)
{

    DIR *dirent;
    int ret_value = -1;

    dirent = opendir(pid_path);
    if(dirent != NULL)
        ret_value = 1;

    return ret_value;
}

/*
Function Name: extract_proc_cmdline
Description: Reads from /proc/pid/cmdline to get cmdline of specfied pid
Return Value: -1 if error or 1 on success
*/
int extract_proc_cmdline(const char *proc_path, proc_info *procInfo)
{
    char *cmdlinepth = format_filepth(proc_path, "cmdline");
    int ret_value = -1;
    FILE *cmdlinefp;
    cmdlinefp = fopen(cmdlinepth,"r");

    char *cmdline_line = get_next_line(cmdlinefp, 0);
    if(cmdline_line != NULL)
    {
        procInfo->cmdline = cmdline_line;
        ret_value = 1;
    }

    fclose(cmdlinefp);
    return ret_value;
}

/*
Function Name: extract_proc_stat
Description: Uses the /proc/pid/stat file to parse out information into the procstat struct.
- Extracts the comm name seperately and reformats the line for parsing
    - This is done to mitigate any errors caused by splitting on space.
Return Value: -1 if error or 1 on success
*/
int extract_proc_stat(const char *proc_path, proc_info *procInfo)
{
    char *statpth = format_filepth(proc_path, "stat");
    int ret_value = -1;
    FILE *statfp;
    statfp = fopen(statpth,"r");

    char *stat_line = get_next_line(statfp, 0);

    char *mod_stat_line = extract_stat_comm(stat_line, procInfo);

    if(mod_stat_line == NULL)
    {
        free(stat_line);
        return ret_value;
    }

    if(sscanf(mod_stat_line, PROCSTAT_FMTSTR,
    &procInfo->pid, &procInfo->state, &procInfo->ppid,
    &procInfo->pgrp, &procInfo->threads, &procInfo->starttime,
    &procInfo->vsize, &procInfo->startstack, &procInfo->env_start,
    &procInfo->env_end
    ) != EOF) {
        ret_value = 1;
    }

    free(stat_line);
    free(mod_stat_line);
    free(statpth);
    fclose(statfp);
    return ret_value;
}

/*
Function Name: extract_proc_environ
Description: Extracts the information stored in /proc/pid/environ
- This function is permission dependant so you'll only get this information applicable to a process started by the user/group you are in
*/

int extract_proc_environ(const char *proc_path, char **environ)
{

    int ret_val = -1;
    int c;
    int ch_count = 0;
    FILE *environfp;

    char *environ_pth = format_filepth(proc_path, "environ");
    environfp = fopen(environ_pth,"r");

    if(environfp == NULL)
    {
        perror("Proc Environ");
        return ret_val;
    }
    while((c = fgetc(environfp)) != EOF) {
        if(ch_count == 0) {
            ch_count = 2;
            *environ = (char *)malloc(ch_count);
            if(*environ == NULL) {
                perror("Error: ");
                ret_val = -1;
                break;
            }
        } else {
            ch_count++;
            char *tmp = (char *)realloc(*environ, ch_count);
            if(tmp == NULL) {
                perror("Error: ");
                ret_val = -1;
                break;
            }
            *environ = tmp;
        }

        if(c == '\0') {
            c = '\n';
        }

        *(*environ+ch_count-2) = c;
        *(*environ+ch_count-1) = '\0';
        ret_val = 1;
    }

    fclose(environfp);
    free(environ_pth);
    return ret_val;

}

int get_proc_exe(const char *proc_path, proc_info *procInfo)
{
    char *exepth = format_filepth(proc_path, "exe");
    readlink(exepth, procInfo->exe_pth, PATH_MAX);
    return 1;
}

int get_proc_maps(const char *proc_path, char **maps)
{
    char c;
    FILE *mapsfp;
    int ch_count = 0;
    int ret_val = -1;

    char *mapspth = format_filepth(proc_path, "maps");
    mapsfp = fopen(mapspth, "r");
    if(mapsfp == NULL)
    {
        perror("Proc Maps");
        exit(1);
    }

    while((c = fgetc(mapsfp)) != EOF)
    {
        if(ch_count == 0)
        {
            ch_count = 2;
            *maps = (char *)malloc(ch_count);
            if(*maps == NULL)
            {
                perror("Error:");
                ret_val = -1;
            }
        }
        else
        {
            ch_count++;
            char *tmp = (char *)realloc(*maps, ch_count);
            if(tmp == NULL) {
                perror("Error: ");
                ret_val = -1;
                break;
            }
            *maps = tmp;
        }
        *(*maps+ch_count-2) = c;
        *(*maps+ch_count-1) = '\0';
        ret_val = 1;
    }

    return ret_val;
}
