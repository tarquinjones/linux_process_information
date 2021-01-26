#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "../include/process_details.h"
#include "../include/process_information.h"
#include "../include/string_handler.h"
#include "../include/output_handler.h"

ssize_t get_btime();
int process_details(const char *pid, u_int8_t options);
void free_fd_list(fd_node_t **head);
int check_proc_exists(const char *pid_path);
int extract_proc_cmdline(const char *proc_path, proc_info *procInfo);
int extract_proc_stat(const char *proc_path, proc_info *procInfo);
char *extract_proc_environ(const char *proc_path);
int get_proc_exe(const char *proc_path, proc_info *procInfo);
char *get_proc_maps(const char *proc_path);
void push_fd_node(fd_node_t **head, char *value, char *fd_name);
fd_node_t *get_proc_fds(const char *proc_path);

ssize_t get_btime()
{
    char *fpath = format_filepth(PROC_PTH, "stat");
    FILE *fp;
    char *line;
    ssize_t btime;

    fp = fopen(fpath, "r");
    if(fp == NULL)
    {
        perror("Error:");
        exit(-1);
    }

    //Get each line until we match on btime
    while( (line = get_next_line(fp)) != NULL) {
        if( strncmp(line, "btime", 5) == 0 )
        {
            sscanf(line, "%*s %lu", &btime);
        }
        free(line);
    }
    return btime;
}

/*
Function: process_details
Description: This is the main control function that goes out and fetches information from other functions within this file
- Before it starts the process of collecting the process information it checks to see if the pid exists!
    - This isn't a conclusive check as it may disappear by the time we start then attempting to parse out the information for the pid.
- Also worth noting that we may get half way through collecting the information and the proc disappears so we need to handle this...
*/
int process_details(const char *pid, u_int8_t options)
{
    char *environ = NULL, *maps = NULL;
    //Need this to determine process start time
    ssize_t boot_time = get_btime();
    char *pid_path = format_procpth(pid);

    if(check_proc_exists(pid_path) != 1)
    {
        printf("Error the specified process doesn't exist...\n");
        exit(1);
    }

    int procstat = 0, proccmdline = 0;

    //INIT Structure to store FDs
    fd_node_t *fds_head = NULL;

    proc_info *procInfo = (proc_info *)malloc(sizeof(proc_info));
    if(procInfo == NULL)
    {
        perror("Error: ");
        exit(1);
    }

    if(options & BASIC) {
        procstat = extract_proc_stat(pid_path, procInfo);
        //Convert the start time
        procInfo->starttime = boot_time+(procInfo->starttime/sysconf(_SC_CLK_TCK));
        proccmdline = extract_proc_cmdline(pid_path, procInfo);
        if( get_proc_exe(pid_path, procInfo) == -1) {
            memcpy(procInfo->exe_pth, "(PERMISSION DENIED)",strlen("(PERMISSION DENIED)"));
            procInfo->exe_pth[strlen("(PERMISSION DENIED)")] = '\0';
        }
    }
    if(options & ENVIRON)
        environ = extract_proc_environ(pid_path);
    if(options & MAPS)
        maps = get_proc_maps(pid_path);
    if(options & FDS)
        fds_head = get_proc_fds(pid_path);

    /*
    - Handling the output based on options and then freeing stuff...
    */
    if(procstat == 1) {
        print_proc_basic_output(procInfo);
        free(procInfo->comm);
    }

    if(proccmdline == 1)
        free(procInfo->cmdline);

    free(procInfo);

    if(environ != NULL)
    {
        print_proc_environ(environ);
        free(environ);
    }

    if(maps != NULL)
    {
        print_proc_maps(maps);
        free(maps);
    }

    if(fds_head != NULL)
    {
        print_proc_fds(fds_head);
        free_fd_list(&fds_head);
    }

    printf("\n");
    free(pid_path);
    return 0;
}

/*
Function: free_fd_list
Description: Free that linked list we made to store the FD information
Return value: (void)
*/
void free_fd_list(fd_node_t **head)
{
    fd_node_t *tmp;
    while(*head != NULL)
    {
        tmp = (*head)->next;
        free(*head);
        (*head) = tmp;
    }
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

    char *cmdline_line = get_next_line(cmdlinefp);
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

    char *stat_line = get_next_line(statfp);

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

char *extract_proc_environ(const char *proc_path)
{

    int c;
    int ch_count = 0;
    FILE *environfp;

    char *environ_pth = format_filepth(proc_path, "environ");
    environfp = fopen(environ_pth,"r");
    char *environ = NULL;
    if(environfp == NULL)
        goto exit;
    while((c = fgetc(environfp)) != EOF) {
        if(ch_count == 0) {
            ch_count = 2;
            environ = (char *)malloc(ch_count);
            if(environ == NULL) {
                perror("Malloc Error: ");
                exit(-1);
            }
        } else {
            ch_count++;
            char *tmp = (char *)realloc(environ, ch_count);
            if(tmp == NULL) {
                perror("Realloc Error: ");
                exit(-1);
            }
            environ = tmp;
        }

        if(c == '\0') {
            c = '\n';
        }

        environ[ch_count-2] = c;
        environ[ch_count-1] = '\0';
    }

    fclose(environfp);
    free(environ_pth);
    return environ;
exit:
    return environ;
}

/*
Function get_proc_exe
Description: Gets the path of the exe associated with the process
Return value: 1 on success or -1 on failure;
*/
int get_proc_exe(const char *proc_path, proc_info *procInfo)
{
    int ret_val = -1;
    char *exepth = format_filepth(proc_path, "exe");

    if(readlink(exepth, procInfo->exe_pth, PATH_MAX) > 0)
        ret_val = 1;

    return ret_val;
}

/*
Function: get_proc_maps
Description: Uses the maps file to print all the process memory map
Return value: 1 on success or -1 on failure
*/
char *get_proc_maps(const char *proc_path)
{
    char c;
    FILE *mapsfp;
    int ch_count = 0;
    char *maps = NULL;

    char *mapspth = format_filepth(proc_path, "maps");
    mapsfp = fopen(mapspth, "r");
    if(mapsfp == NULL)
        goto exit;

    while((c = fgetc(mapsfp)) != EOF)
    {
        if(ch_count == 0)
        {
            ch_count = 2;
            maps = (char *)malloc(ch_count);
            if(maps == NULL)
            {
                perror("Malloc Error:");
                exit(-1);
            }
        }
        else
        {
            ch_count++;
            char *tmp = (char *)realloc(maps, ch_count);
            if(tmp == NULL) {
                perror("Realloc Error: ");
                break;
            }
            maps = tmp;
        }

        maps[ch_count-2] = c;
        maps[ch_count-1] = '\0';
    }

exit:
    return maps;
}


/*
Function: push_fd_node
Description: Adds node to the end of the FD linked list
Return value: (void)
*/
void push_fd_node(fd_node_t **head, char *value, char *fd_name)
{
    fd_node_t *new_node = (fd_node_t*)malloc(sizeof(fd_node_t));
    if(new_node == NULL)
    {
        perror("Malloc error:");
        exit(1);
    }

    if(memcpy(new_node->path, value, strlen(value)+1) == NULL)
    {
        perror("Issue during memcpy...");
        exit(1);
    }
    new_node->fd_name = strdup(fd_name);

    new_node->next = NULL;

    if(*head == NULL) {
        *head = new_node;
    } else {
        fd_node_t *last_node = *head;

        while(last_node->next != NULL) {
            last_node = last_node->next;
        }

        last_node->next = new_node;
    }
}

/*
Function: get_proc_fds
Description: Retrieves all FDs (including deleted) for the specified process
    - It stores the results in a linked list for later processing.
Return value: fd_node_t
*/
fd_node_t *get_proc_fds(const char *proc_path)
{
    DIR *fd_dir;
    struct dirent *fd_dirent;
    char link_path[PATH_MAX];
    char *fdpth = format_filepth(proc_path, "fd/");
    char *temp_fdpth;

    fd_node_t *head = NULL;

    fd_dir = opendir(fdpth);
    if(fd_dir != NULL)
    {
        while((fd_dirent = readdir(fd_dir)) != NULL) {
            if(fd_dirent->d_type == DT_LNK) {
                temp_fdpth = format_filepth(fdpth, fd_dirent->d_name);
                ssize_t link_size = readlink (temp_fdpth, link_path, PATH_MAX);
                if(link_size > 0) {
                    //As readlink doesn't add the '\0' termninator we do it's work for it
                    link_path[link_size] = '\0';
                    push_fd_node(&head, link_path, fd_dirent->d_name);
                }
                free(temp_fdpth);
            }
        }
    }
    return head;
}
