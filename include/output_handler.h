#ifndef OUTPUT_HANDLER_H
#define OUTPUT_HANDLER_H

void print_proc_basic_output(proc_info *procInfo);
void print_proc_environ(char *environ);
void print_proc_maps(char *maps);
void print_proc_fds(fd_node_t *head);

#endif
