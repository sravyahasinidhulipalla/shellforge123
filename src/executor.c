#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#include "executor.h"
#include "jobs.h"

static pid_t shell_pgid;
static int shell_terminal;

/* =========================================================
   SIGCHLD HANDLER
   ========================================================= */

static void sigchld_handler(int sig)
{
    (void)sig;
}

/* =========================================================
   SETUP BACKGROUND HANDLER
   ========================================================= */

void setup_background_handler(void)
{
    struct sigaction sa;

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    sigaction(SIGCHLD, &sa, NULL);
}

/* =========================================================
   SETUP JOB CONTROL
   ========================================================= */

void setup_job_control(void)
{
    shell_terminal = STDIN_FILENO;
    shell_pgid = getpid();

    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    if (setpgid(shell_pgid, shell_pgid) < 0)
    {
        if (errno != EACCES && errno != EPERM)
            perror("setpgid");
    }

    if (tcsetpgrp(shell_terminal, shell_pgid) < 0)
        perror("tcsetpgrp");
}

/* =========================================================
   APPLY REDIRECTION
   ========================================================= */

static void apply_redirection(command_t *cmd)
{
    int fd;

    if (cmd->input_file != NULL)
    {
        fd = open(cmd->input_file, O_RDONLY);

        if (fd < 0)
        {
            perror("input redirection");
            exit(1);
        }

        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    if (cmd->output_file != NULL)
    {
        if (cmd->append)
        {
            fd = open(cmd->output_file,
                      O_WRONLY | O_CREAT | O_APPEND,
                      0644);
        }
        else
        {
            fd = open(cmd->output_file,
                      O_WRONLY | O_CREAT | O_TRUNC,
                      0644);
        }

        if (fd < 0)
        {
            perror("output redirection");
            exit(1);
        }

        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
}

/* =========================================================
   RESTORE CHILD SIGNALS
   ========================================================= */

static void restore_child_signals(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
}

/* =========================================================
   EXECUTE SINGLE COMMAND
   ========================================================= */

int execute_command(command_t *cmd)
{
    pid_t pid;
    int status;

    if (cmd == NULL || cmd->argc == 0)
        return 0;

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return -1;
    }

    /* CHILD */
    if (pid == 0)
    {
        restore_child_signals();

        setpgid(0, 0);

        if (cmd->background &&
            cmd->input_file == NULL)
        {
            int null_fd = open("/dev/null", O_RDONLY);

            if (null_fd < 0)
            {
                perror("/dev/null");
                exit(1);
            }

            dup2(null_fd, STDIN_FILENO);
            close(null_fd);
        }

        apply_redirection(cmd);

        execvp(cmd->argv[0], cmd->argv);

        perror(cmd->argv[0]);
        exit(127);
    }

    /* PARENT */
    setpgid(pid, pid);

    /* BACKGROUND */
    if (cmd->background)
    {
        int job_id =
            job_add(pid,
                    JOB_RUNNING,
                    cmd->argv[0]);

        printf("[Background PID: %d]\n", pid);

        if (job_id > 0)
        {
            printf("[%d] Running %s\n",
                   job_id,
                   cmd->argv[0]);
        }

        fflush(stdout);

        return 0;
    }

    /* FOREGROUND */
    if (tcsetpgrp(shell_terminal, pid) < 0)
        perror("tcsetpgrp");

    while (1)
    {
        pid_t result;

        result = waitpid(-pid,
                         &status,
                         WUNTRACED);

        if (result < 0)
        {
            if (errno == EINTR)
                continue;

            if (errno == ECHILD)
                break;

            perror("waitpid");
            break;
        }

        if (WIFSTOPPED(status))
        {
            int job_id =
                job_add(pid,
                        JOB_STOPPED,
                        cmd->argv[0]);

            if (job_id > 0)
            {
                printf("\n[%d] Stopped %s\n",
                       job_id,
                       cmd->argv[0]);
            }

            break;
        }

        if (WIFEXITED(status) ||
            WIFSIGNALED(status))
        {
            break;
        }
    }

    if (tcsetpgrp(shell_terminal, shell_pgid) < 0)
        perror("tcsetpgrp");

    if (WIFEXITED(status))
        return WEXITSTATUS(status);

    return 0;
}

/* =========================================================
   EXECUTE PIPELINE
   ========================================================= */

int execute_pipeline(pipeline_t *pipeline,
                     const char *command_line)
{
    int count;
    int pipefd[2 * (MAX_COMMANDS - 1)];
    pid_t pids[MAX_COMMANDS];
    pid_t pgid = 0;
    int background;
    int status = 0;

    if (pipeline == NULL ||
        pipeline->command_count == 0)
    {
        return 0;
    }

    count = pipeline->command_count;

    background =
        pipeline->commands[count - 1].background;

    if (count == 1)
        return execute_command(&pipeline->commands[0]);

    /* Create pipes */
    for (int i = 0; i < count - 1; i++)
    {
        if (pipe(&pipefd[i * 2]) < 0)
        {
            perror("pipe");
            return -1;
        }
    }

    /* Create children */
    for (int i = 0; i < count; i++)
    {
        pids[i] = fork();

        if (pids[i] < 0)
        {
            perror("fork");
            return -1;
        }

        /* CHILD */
        if (pids[i] == 0)
        {
            command_t *cmd =
                &pipeline->commands[i];

            restore_child_signals();

            /*
             * First child becomes process-group leader.
             */
            if (i == 0)
            {
                setpgid(0, 0);
            }
            else
            {
                setpgid(0, pgid);
            }

            /* Connect input */
            if (i > 0)
            {
                dup2(pipefd[(i - 1) * 2],
                     STDIN_FILENO);
            }

            /* Connect output */
            if (i < count - 1)
            {
                dup2(pipefd[i * 2 + 1],
                     STDOUT_FILENO);
            }

            /* Background input from /dev/null */
            if (background &&
                i == 0 &&
                cmd->input_file == NULL)
            {
                int null_fd =
                    open("/dev/null", O_RDONLY);

                if (null_fd < 0)
                {
                    perror("/dev/null");
                    exit(1);
                }

                dup2(null_fd, STDIN_FILENO);
                close(null_fd);
            }

            apply_redirection(cmd);

            /* Close all pipe descriptors */
            for (int j = 0;
                 j < 2 * (count - 1);
                 j++)
            {
                close(pipefd[j]);
            }

            execvp(cmd->argv[0], cmd->argv);

            perror(cmd->argv[0]);
            exit(127);
        }

        /* PARENT */
        if (pgid == 0)
            pgid = pids[i];

        setpgid(pids[i], pgid);
    }

    /* Parent closes pipes */
    for (int i = 0;
         i < 2 * (count - 1);
         i++)
    {
        close(pipefd[i]);
    }

    /* BACKGROUND PIPELINE */
    if (background)
    {
        int job_id =
            job_add(pgid,
                    JOB_RUNNING,
                    command_line);

        printf("[Background Pipeline PID: %d]\n",
               pgid);

        if (job_id > 0)
        {
            printf("[%d] Running %s\n",
                   job_id,
                   command_line);
        }

        fflush(stdout);

        return 0;
    }

    /* FOREGROUND PIPELINE */
    if (tcsetpgrp(shell_terminal, pgid) < 0)
        perror("tcsetpgrp");

    while (1)
    {
        pid_t result;

        result = waitpid(-pgid,
                         &status,
                         WUNTRACED);

        if (result < 0)
        {
            if (errno == EINTR)
                continue;

            if (errno == ECHILD)
                break;

            perror("waitpid");
            break;
        }

        if (WIFSTOPPED(status))
        {
            int job_id =
                job_add(pgid,
                        JOB_STOPPED,
                        command_line);

            if (job_id > 0)
            {
                printf("\n[%d] Stopped %s\n",
                       job_id,
                       command_line);
            }

            break;
        }

        if (WIFEXITED(status) ||
            WIFSIGNALED(status))
        {
            int alive = 0;

            for (int i = 0; i < count; i++)
            {
                int child_status;

                pid_t r =
                    waitpid(pids[i],
                            &child_status,
                            WNOHANG);

                if (r == 0)
                    alive = 1;
            }

            if (!alive)
                break;
        }
    }

    if (tcsetpgrp(shell_terminal, shell_pgid) < 0)
        perror("tcsetpgrp");

    return status;
}

/* =========================================================
   FOREGROUND JOB
   ========================================================= */

int foreground_job(job_t *job)
{
    int status;
    pid_t pid;

    if (job == NULL)
        return -1;

    /* Give terminal to the job */
    if (tcsetpgrp(shell_terminal, job->pgid) < 0)
    {
        perror("tcsetpgrp");
        return -1;
    }

    /* Continue stopped job */
    if (job->state == JOB_STOPPED)
    {
        if (kill(-job->pgid, SIGCONT) < 0)
        {
            perror("SIGCONT");

            tcsetpgrp(shell_terminal,
                      shell_pgid);

            return -1;
        }

        job->state = JOB_RUNNING;
    }

    /* Wait for the foreground job */
    while (1)
    {
        pid = waitpid(-job->pgid,
                      &status,
                      WUNTRACED);

        if (pid < 0)
        {
            if (errno == EINTR)
                continue;

            if (errno == ECHILD)
                break;

            perror("waitpid");
            break;
        }

        /* Ctrl+Z */
        if (WIFSTOPPED(status))
        {
            job->state = JOB_STOPPED;

            printf("\n[%d] Stopped %s\n",
                   job->id,
                   job->command);

            break;
        }

        /* Job finished */
        if (WIFEXITED(status) ||
            WIFSIGNALED(status))
        {
            job->state = JOB_DONE;
            break;
        }
    }

    /* Give terminal back to shell */
    if (tcsetpgrp(shell_terminal,
                  shell_pgid) < 0)
    {
        perror("tcsetpgrp");
    }

    /* Remove completed job */
    if (job->state == JOB_DONE)
        job_remove(job->id);

    return 0;
}

/* =========================================================
   BACKGROUND JOB
   ========================================================= */

int background_job(job_t *job)
{
    if (job == NULL)
        return -1;

    if (kill(-job->pgid, SIGCONT) < 0)
    {
        perror("SIGCONT");
        return -1;
    }

    job->state = JOB_RUNNING;

    printf("[%d] Running %s\n",
           job->id,
           job->command);

    return 0;
}
