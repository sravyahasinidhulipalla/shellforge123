#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

#define MAX_JOBS 100

typedef enum
{
    JOB_RUNNING,
    JOB_STOPPED,
    JOB_DONE
} job_state_t;

typedef struct
{
    int id;
    pid_t pgid;
    job_state_t state;
    char command[1024];
} job_t;

void jobs_init(void);

int job_add(pid_t pgid,
            job_state_t state,
            const char *command);

job_t *job_find(int id);

job_t *job_find_by_pgid(pid_t pgid);

void job_remove(int id);

void jobs_print(void);

void job_stop(pid_t pgid);

void job_continue(pid_t pgid);

void job_done(pid_t pgid);

void jobs_reap(void);

#endif
