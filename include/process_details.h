#include <linux/limits.h>
#define PROC_PTH "/proc/"

#define PROCSTAT_FMTSTR "%d %c %d %d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %ld %*ld %llu %lu %*ld %*lu %*lu %*lu %lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %lu %lu"

typedef struct proc_info {
    int pid; // pos 1
    char *comm;
    char *cmdline;
    char exe_pth[PATH_MAX];
    char state; // pos 2
    int ppid; //pos 3
    int pgrp; // pos 4
    long threads; // pos 19
    unsigned long long starttime; //pos 21
    unsigned long vsize; // pos 22
    size_t startstack; // pos 27
    size_t env_start;
    size_t env_end;
} proc_info;

typedef struct FD_node
{
    char path[PATH_MAX];
    char *fd_name;
    struct FD_node *next;
} fd_node_t;

int process_details(const char *pid, u_int8_t options);
int extract_proc_stat(const char *proc_path, proc_info *procInfo);
