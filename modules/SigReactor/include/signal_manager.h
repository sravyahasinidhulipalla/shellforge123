#ifndef SIGNAL_MANAGER_H
#define SIGNAL_MANAGER_H

int signal_manager_init(int event_fd);
void signal_manager_cleanup(void);

#endif
