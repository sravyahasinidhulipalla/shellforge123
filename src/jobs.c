#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#include "jobs.h"

static job_t job_table[MAX_JOBS];
static int next_job_id = 1;

void jobs_init(void)
{
    memset(job_table, 0, sizeof(job_table));
    next_job_id = 1;
}

int job_add(pid_t pgid,
            job_state_t state,
            const char *command)
{
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (job_table[i].id == 0)
        {
            job_table[i].id = next_job_id++;
            job_table[i].pgid = pgid;
            job_table[i].state = state;

            if (command != NULL)
            {
                strncpy(job_table[i].command,
                        command,
                        sizeof(job_table[i].command) - 1);

                job_table[i].command[
                    sizeof(job_table[i].command) - 1
                ] = '\0';
            }

            return job_table[i].id;
        }
    }

    fprintf(stderr, "jobs: job table full\n");
    return -1;
}

job_t *job_find(int id)
{
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (job_table[i].id == id)
            return &job_table[i];
    }

    return NULL;
}

job_t *job_find_by_pgid(pid_t pgid)
{
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (job_table[i].id != 0 &&
            job_table[i].pgid == pgid)
        {
            return &job_table[i];
        }
    }

    return NULL;
}

void job_remove(int id)
{
    job_t *job = job_find(id);

    if (job != NULL)
        memset(job, 0, sizeof(job_t));
}

void jobs_print(void)
{
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (job_table[i].id == 0)
            continue;

        const char *state;

        switch (job_table[i].state)
        {
            case JOB_RUNNING:
                state = "RUNNING";
                break;

            case JOB_STOPPED:
                state = "STOPPED";
                break;

            case JOB_DONE:
                state = "DONE";
                break;

            default:
                state = "UNKNOWN";
                break;
        }

        printf("[%d] %-8s PGID=%d %s\n",
               job_table[i].id,
               state,
               job_table[i].pgid,
               job_table[i].command);
    }
}

void job_stop(pid_t pgid)
{
    job_t *job = job_find_by_pgid(pgid);

    if (job != NULL)
        job->state = JOB_STOPPED;
}

void job_continue(pid_t pgid)
{
    job_t *job = job_find_by_pgid(pgid);

    if (job != NULL)
    {
        if (kill(-pgid, SIGCONT) < 0)
            perror("SIGCONT");

        job->state = JOB_RUNNING;
    }
}

void job_done(pid_t pgid)
{
    job_t *job = job_find_by_pgid(pgid);

    if (job != NULL)
        job->state = JOB_DONE;
}

void jobs_reap(void)
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-1,
                          &status,
                          WNOHANG | WUNTRACED | WCONTINUED)) > 0)
    {
        job_t *job = job_find_by_pgid(pid);

        if (job == NULL)
            continue;

        if (WIFSTOPPED(status))
            job->state = JOB_STOPPED;
        else if (WIFCONTINUED(status))
            job->state = JOB_RUNNING;
        else if (WIFEXITED(status) ||
                 WIFSIGNALED(status))
            job->state = JOB_DONE;
    }
}
