#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include "../include/process_details.h"

//Function declarations
int reg_compare(const char *line, const char *pattern, regmatch_t pmatch[2]);
char *format_procpth(const char *pid);
char *format_filepth(const char *base, const char *file);
char *get_next_line(FILE *fp);
char *extract_stat_comm(const char *stat_line, proc_info *procInfo);
int reg_compare(const char *line, const char *pattern, regmatch_t pmatch[2]);
//End function declarations


/*
Function: format_procpth

Description: Takes a pid as input as returns /proc/pid/ for use in other functions
- It uses the PROC_PTH macro to determine the string length

Return value: char pointer to the new full pid path
*/
char *format_procpth(const char *pid)
{
    // +2 added for '/' and '\0'
    char *path = (char *)malloc(strlen(PROC_PTH)+strlen(pid)+2);
    if(path == NULL)
    {
        perror("Error: ");
        exit(1);
    }
    memcpy(path, PROC_PTH, strlen(PROC_PTH));
    memcpy(path+strlen(PROC_PTH), pid, strlen(pid));
    path[strlen(PROC_PTH)+strlen(pid)] = '/';
    path[strlen(PROC_PTH)+strlen(pid)+1] = '\0';

    return path;
}

/*
Function: format_filepth

Description: Takes a base folder as input and appends the filename to the end of the path

Return value: char pointer to the new path
*/
char *format_filepth(const char *base, const char *file)
{
    // +1 added for '\0'
    char *file_path = (char *)malloc(strlen(base)+strlen(file)+1);
    if(file == NULL)
    {
        perror("Error: ");
        exit(1);
    }
    memcpy(file_path, base, strlen(base));
    memcpy(file_path+strlen(base), file, strlen(file));
    file_path[strlen(base)+strlen(file)] = '\0';

    return file_path;
}

/*
Function: get_next_line

Description: Taken from linux_process_listing
- This takes a file pointer as input and seeks to the end of the line dynamically allocating the require memory
Return Value: char pointer to the extracted lineint
*/
char *get_next_line(FILE *fp)
{
    char *line = NULL;
    int i = 0;
    char c;

    while((c = getc(fp)) != EOF)
    {
        //Fixes issue with parsing cmdline
        if(c == '\0')
            c = ' ';

        if(c == '\n')
            break;

        if(i == 0)
        {
            line = (char *)malloc(sizeof(char *)*2);
            if(line == NULL)
            {
                perror("Error: ");
                return NULL;
            }
        }
        else
        {
            void *tmp = (char *)realloc(line, sizeof(char *)*(i+2));
            if(tmp != NULL)
            {
                line = tmp;
            }
            else
            {
                perror("Error: ");
            }
        }
        line[i+1] = '\0';
        line[i] = c;
        i++;

    }
    return line;
}

/*
Function: extract_stat_comm

Description: It uses a regex to extract the comm name from the stat line and return a modified line without this value
- The extract comm name is added to the pidInfo struct.

Return value: char pointer to the new stat new
*/
char *extract_stat_comm(const char *stat_line, proc_info *procInfo)
{

    char *new_line = NULL;
    regmatch_t reg_matches[2];
    if(reg_compare(stat_line, "[ (].*[)]", reg_matches) == REG_NOMATCH)
    {
        perror("Error with regex when extracting comm name...");
        return new_line;
    }
    else
    {
        //First we extract add the comm name to procstatInfo struct
        int comm_size = (reg_matches[0].rm_eo-1)-(reg_matches[0].rm_so+2);
        procInfo->comm = (char *)malloc(comm_size+1);
        if(procInfo->comm == NULL)
        {
            perror("Error: ");
            return new_line;
        }
        memcpy(procInfo->comm, stat_line+reg_matches[0].rm_so+2, comm_size);
        procInfo->comm[comm_size] = '\0';

        //Next we create a new stat line without the comm name so we can split on spaces safely!
        new_line = (char *)malloc((strlen(stat_line)+1)-comm_size);
        if(new_line == NULL)
        {
            perror("Error: ");
            return new_line;
        }
        memcpy(new_line, stat_line, reg_matches[0].rm_so);
        memcpy(new_line+reg_matches[0].rm_so, stat_line+reg_matches[0].rm_eo, strlen(stat_line)-reg_matches[0].rm_so-comm_size);
        new_line[strlen(stat_line)+comm_size] = '\0';
        return new_line;
    }
}

/*
Function: reg_match

Description: Takes a POSIX regex and string as input and attempts to find the start and end byte offset of the first match

Return value: Returns regex_t which can then be used to extract [0].so and [0].eo
*/
int reg_compare(const char *line, const char *pattern, regmatch_t pmatch[2])
{
    regex_t regex;
    int retv;
    retv = regcomp(&regex, pattern, 0);
    if (retv != 0)
    {
        perror("Regex compilation error: ");
    }
    retv = regexec(&regex, line, 1, pmatch, 0);
    return retv;
}

