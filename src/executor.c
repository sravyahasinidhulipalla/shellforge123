#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "executor.h"
#include "builtin.h"

int execute_command(command_t *cmd)
{
    if (cmd == NULL || cmd->argc == 0)
        return 0;

    /*
     * Built-ins must run in the shell process.
     * This is especially important for cd.
     */
    if (is_builtin(cmd))
        return execute_builtin(cmd);

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return -1;
    }

    if (pid == 0)
    {
        if (cmd->input_file != NULL)
        {
            int fd = open(cmd->input_file, O_RDONLY);

            if (fd < 0)
            {
                perror("input");
                exit(EXIT_FAILURE);
            }

            if (dup2(fd, STDIN_FILENO) < 0)
            {
                perror("dup2");
                close(fd);
                exit(EXIT_FAILURE);
            }

            close(fd);
        }

        if (cmd->output_file != NULL)
        {
            int flags = O_WRONLY | O_CREAT;

            if (cmd->append)
                flags |= O_APPEND;
            else
                flags |= O_TRUNC;

            int fd = open(cmd->output_file, flags, 0644);

            if (fd < 0)
            {
                perror("output");
                exit(EXIT_FAILURE);
            }

            if (dup2(fd, STDOUT_FILENO) < 0)
            {
                perror("dup2");
                close(fd);
                exit(EXIT_FAILURE);
            }

            close(fd);
        }

        execvp(cmd->argv[0], cmd->argv);

        perror(cmd->argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!cmd->background)
        waitpid(pid, NULL, 0);

    return 0;
}

/* =========================================================
   EXECUTE PIPELINE
   ========================================================= */

int execute_pipeline(pipeline_t *pipeline)
{
    if (pipeline == NULL || pipeline->command_count == 0)
        return 0;

    /* Single command: use normal command execution */
    if (pipeline->command_count == 1)
        return execute_command(&pipeline->commands[0]);

    int pipefd[MAX_COMMANDS - 1][2];
    pid_t pids[MAX_COMMANDS];

    int count = pipeline->command_count;

    /* Create all pipes */
    for (int i = 0; i < count - 1; i++)
    {
        if (pipe(pipefd[i]) < 0)
        {
            perror("pipe");
            return -1;
        }
    }

    /* Create one child for each command */
    for (int i = 0; i < count; i++)
    {
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork");
            return -1;
        }

        if (pid == 0)
        {
            /*
             * Every command except the first reads
             * from the previous pipe.
             */
            if (i > 0)
            {
                if (dup2(pipefd[i - 1][0], STDIN_FILENO) < 0)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            /*
             * Every command except the last writes
             * to the next pipe.
             */
            if (i < count - 1)
            {
                if (dup2(pipefd[i][1], STDOUT_FILENO) < 0)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            /*
             * Explicit input redirection.
             */
            if (pipeline->commands[i].input_file != NULL)
            {
                int fd = open(
                    pipeline->commands[i].input_file,
                    O_RDONLY
                );

                if (fd < 0)
                {
                    perror("input");
                    exit(EXIT_FAILURE);
                }

                if (dup2(fd, STDIN_FILENO) < 0)
                {
                    perror("dup2");
                    close(fd);
                    exit(EXIT_FAILURE);
                }

                close(fd);
            }

            /*
             * Explicit output redirection.
             */
            if (pipeline->commands[i].output_file != NULL)
            {
                int flags = O_WRONLY | O_CREAT;

                if (pipeline->commands[i].append)
                    flags |= O_APPEND;
                else
                    flags |= O_TRUNC;

                int fd = open(
                    pipeline->commands[i].output_file,
                    flags,
                    0644
                );

                if (fd < 0)
                {
                    perror("output");
                    exit(EXIT_FAILURE);
                }

                if (dup2(fd, STDOUT_FILENO) < 0)
                {
                    perror("dup2");
                    close(fd);
                    exit(EXIT_FAILURE);
                }

                close(fd);
            }

            /*
             * Close all pipe descriptors in the child.
             */
            for (int j = 0; j < count - 1; j++)
            {
                close(pipefd[j][0]);
                close(pipefd[j][1]);
            }

            execvp(
                pipeline->commands[i].argv[0],
                pipeline->commands[i].argv
            );

            perror(pipeline->commands[i].argv[0]);
            exit(EXIT_FAILURE);
        }

        pids[i] = pid;
    }

    /*
     * Parent no longer needs any pipe descriptors.
     */
    for (int i = 0; i < count - 1; i++)
    {
        close(pipefd[i][0]);
        close(pipefd[i][1]);
    }

    /*
     * Wait for all commands.
     * Keep the status of the last command.
     */
    int last_status = 0;

    for (int i = 0; i < count; i++)
    {
        int status;

        if (waitpid(pids[i], &status, 0) < 0)
        {
            perror("waitpid");
            return -1;
        }

        if (i == count - 1)
            last_status = status;
    }

    if (WIFEXITED(last_status))
        return WEXITSTATUS(last_status);

    return -1;
}
