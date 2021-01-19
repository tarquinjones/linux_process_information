#define PROC_PTH "/proc/"

typedef struct procstat_info {
    int pid;
    char *comm;
    char state;
    int ppid;
    int pgrp;
    unsigned long long starttime;
    unsigned long vsize;
} procstat_info;

int process_details(const char *pid);
int extract_proc_stat(const char *proc_path, procstat_info *procstatInfo);
