#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/process_details.h"

void print_proc_basic_output(proc_info *procInfo);
void print_header(char *title);
void print_proc_fds(fd_node_t *head);

/*
Function: print_header_footer
Description: Prints either the header or the footer dependant on type provided.
- Type 1: Prints block header
- Type 2: Prints block footer
*/
void print_header(char *title)
{
    printf("\n");
    for(int i = 0; i < 40; i++)
    {
        printf("-");
        if(i == 19)
            printf(" %s ", title);
    }
    printf("\n");
}

/*
Just temporary for testing
*/
void print_proc_basic_output(proc_info *procInfo) {
    print_header("Process Basic Details");
    fprintf(stdout,
    "Process ID: %d\n"
    "Process Parent ID: %d\n"
    "Process Group ID: %d\n"
    "Process Name: %s\n"
    "Process Command Line: %s\n"
    "Process Exe Path: %s\n"
    "Process State: %c\n"
    "Process Start Time: %llu\n"
    "Process Threads: %ld\n"
    "Process Memory Size: %lu bytes\n"
    "Process Stack Start Pos: 0x%lx\n"
    "Process Env Start Pos: 0x%lx\n"
    "Process Env End Pos: 0x%lx\n",
    procInfo->pid, procInfo->ppid, procInfo->pgrp,
    procInfo->comm, procInfo->cmdline, procInfo->exe_pth, procInfo->state,
    procInfo->starttime, procInfo->threads, procInfo->vsize,
    procInfo->startstack, procInfo->env_start, procInfo->env_end
    );
}

/*
Just temporary for testing
*/
void print_proc_environ(char *environ)
{
    print_header("Process Environment");
    fprintf(stdout, "%s", environ);
}

/*
Just temporary for testing
*/
void print_proc_maps(char *maps)
{
    print_header("Process Maps");
    fprintf(stdout, "%s", maps);
}

void print_proc_fds(fd_node_t *head)
{
    print_header("Process File Descriptors");
    fd_node_t *temp;
    temp = head;
    while(temp != NULL) {
        printf("FD %s --> %s\n", temp->fd_name, temp->path);
        temp = temp->next;
    }
}
