char *format_procpth(const char *pid);
char *format_filepth(const char *base, const char *file);
char *get_next_line(FILE *fp, int type);
char *extract_stat_comm(const char *stat_line, procstat_info *procstatInfo);
