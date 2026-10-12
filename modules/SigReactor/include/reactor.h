
#ifndef REACTOR_H
#define REACTOR_H

int reactor_init(void);
int reactor_get_fd(void);
void reactor_run_once(void);
int reactor_launch_worker(const char *path, const char *label);
void reactor_close(void);

#endif
