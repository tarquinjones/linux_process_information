#define PROC_PTH "/proc/"

#define PROCSTAT_FMTSTR "%d %c %d %d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %ld %*ld %llu %lu %*ld %*lu %*lu %*lu %lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %lu %lu"

typedef struct procstat_info {
    int pid; // pos 1
    char *comm;
    char *cmdline;
    char state; // pos 2
    int ppid; //pos 3
    int pgrp; // pos 4
    long threads; // pos 19
    unsigned long long starttime; //pos 21
    unsigned long vsize; // pos 22
    size_t startstack; // pos 27
    size_t env_start;
    size_t env_end;
} procstat_info;

int process_details(const char *pid);
int extract_proc_stat(const char *proc_path, procstat_info *procstatInfo);
