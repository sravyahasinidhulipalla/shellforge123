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

/* =========================================================
   SIGCHLD HANDLER
   Reap finished background processes
   ========================================================= */

static void sigchld_handler(int sig)
{
    int saved_errno = errno;

    (void)sig;

    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
        /* Reap finished child processes */
    }

    errno = saved_errno;
}

/* =========================================================
   SETUP BACKGROUND PROCESS HANDLER
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
   APPLY INPUT / OUTPUT REDIRECTION
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

    /* -----------------------------------------------------
       CHILD PROCESS
       ----------------------------------------------------- */

    if (pid == 0)
    {
        /*
         * Background processes should not read
         * from the shell terminal.
         */
        if (cmd->background && cmd->input_file == NULL)
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

    /* -----------------------------------------------------
       PARENT PROCESS
       ----------------------------------------------------- */

    if (cmd->background)
    {
        printf("[Background PID: %d]\n", pid);
        fflush(stdout);

        /*
         * Do not wait for background process.
         */
        return 0;
    }

    /* Foreground process */
    if (waitpid(pid, &status, 0) < 0)
    {
        perror("waitpid");
        return -1;
    }

    if (WIFEXITED(status))
        return WEXITSTATUS(status);

    return -1;
}

/* =========================================================
   EXECUTE PIPELINE
   ========================================================= */

int execute_pipeline(pipeline_t *pipeline)
{
    int count;
    int pipefd[2 * (MAX_COMMANDS - 1)];
    pid_t pids[MAX_COMMANDS];
    int background;
    int status = 0;

    if (pipeline == NULL || pipeline->command_count == 0)
        return 0;

    count = pipeline->command_count;

    /*
     * Background status belongs to the last command
     * in the pipeline.
     */
    background =
        pipeline->commands[count - 1].background;

    /* -----------------------------------------------------
       SINGLE COMMAND
       ----------------------------------------------------- */

    if (count == 1)
        return execute_command(&pipeline->commands[0]);

    /* -----------------------------------------------------
       CREATE PIPES
       ----------------------------------------------------- */

    for (int i = 0; i < count - 1; i++)
    {
        if (pipe(&pipefd[i * 2]) < 0)
        {
            perror("pipe");
            return -1;
        }
    }

    /* -----------------------------------------------------
       FORK COMMANDS
       ----------------------------------------------------- */

    for (int i = 0; i < count; i++)
    {
        pids[i] = fork();

        if (pids[i] < 0)
        {
            perror("fork");
            return -1;
        }

        /* -------------------------------------------------
           CHILD PROCESS
           ------------------------------------------------- */

        if (pids[i] == 0)
        {
            command_t *cmd = &pipeline->commands[i];

            /* Read from previous pipe */
            if (i > 0)
            {
                if (dup2(pipefd[(i - 1) * 2],
                         STDIN_FILENO) < 0)
                {
                    perror("dup2");
                    exit(1);
                }
            }

            /* Write to next pipe */
            if (i < count - 1)
            {
                if (dup2(pipefd[i * 2 + 1],
                         STDOUT_FILENO) < 0)
                {
                    perror("dup2");
                    exit(1);
                }
            }

            /*
             * Background pipeline:
             * first command gets /dev/null as stdin
             * unless explicit input redirection exists.
             */
            if (background &&
                i == 0 &&
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

            /* Close all pipe descriptors */
            for (int j = 0; j < 2 * (count - 1); j++)
                close(pipefd[j]);

            execvp(cmd->argv[0], cmd->argv);

            perror(cmd->argv[0]);
            exit(127);
        }
    }

    /* -----------------------------------------------------
       PARENT: CLOSE ALL PIPE DESCRIPTORS
       ----------------------------------------------------- */

    for (int i = 0; i < 2 * (count - 1); i++)
        close(pipefd[i]);

    /* -----------------------------------------------------
       BACKGROUND PIPELINE
       ----------------------------------------------------- */

    if (background)
    {
        printf("[Background Pipeline PID: %d]\n", pids[0]);
        fflush(stdout);

        /*
         * Do not wait for background pipeline.
         */
        return 0;
    }

    /* -----------------------------------------------------
       FOREGROUND PIPELINE
       ----------------------------------------------------- */

    for (int i = 0; i < count; i++)
    {
        int child_status;

        if (waitpid(pids[i], &child_status, 0) < 0)
        {
            perror("waitpid");
            status = -1;
            continue;
        }

        if (i == count - 1 &&
            WIFEXITED(child_status))
        {
            status = WEXITSTATUS(child_status);
        }
    }

    return status;
}
