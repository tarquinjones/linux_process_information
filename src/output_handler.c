#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/process_details.h"

void print_proc_basic_output(procstat_info *procstatInfo);
void print_header_footer(char *title);

/*
Function: print_header_footer
Description: Prints either the header or the footer dependant on type provided.
- Type 1: Prints block header
- Type 2: Prints block footer
*/
void print_header_footer(char *title)
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


void print_proc_basic_output(procstat_info *procstatInfo) {
    print_header_footer("Process Basic Details");
    fprintf(stdout,
    "Process ID: %d\n"
    "Process Parent ID: %d\n"
    "Process Group ID: %d\n"
    "Process Name: %s\n"
    "Process Command Line: %s\n"
    "Process State: %c\n"
    "Process Start Time: %llu\n"
    "Process Threads: %ld\n"
    "Process Memory Size: %lu bytes\n"
    "Process Stack Start Pos: 0x%lx\n"
    "Process Env Start Pos: 0x%lx\n"
    "Process Env End Pos: 0x%lx\n",
    procstatInfo->pid, procstatInfo->ppid, procstatInfo->pgrp,
    procstatInfo->comm, procstatInfo->cmdline, procstatInfo->state,
    procstatInfo->starttime, procstatInfo->threads, procstatInfo->vsize,
    procstatInfo->startstack, procstatInfo->env_start, procstatInfo->env_end
    );
}

void print_proc_environ(char *environ)
{
    print_header_footer("Process Environment");
    fprintf(stdout, "%s", environ);
}
