#ifndef _FORK_H
#define _FORK_H

int copy_process(unsigned long fn, unsigned long arg);
int copy_idle_process(unsigned long fn, unsigned long arg);
#endif
