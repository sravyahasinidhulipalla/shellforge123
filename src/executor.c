#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

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
            FILE *input = fopen(cmd->input_file, "r");

            if (input == NULL)
            {
                perror("input");
                exit(EXIT_FAILURE);
            }

            if (dup2(fileno(input), STDIN_FILENO) < 0)
            {
                perror("dup2");
                fclose(input);
                exit(EXIT_FAILURE);
            }

            fclose(input);
        }

        if (cmd->output_file != NULL)
        {
            const char *mode = cmd->append ? "a" : "w";

            FILE *output = fopen(cmd->output_file, mode);

            if (output == NULL)
            {
                perror("output");
                exit(EXIT_FAILURE);
            }

            if (dup2(fileno(output), STDOUT_FILENO) < 0)
            {
                perror("dup2");
                fclose(output);
                exit(EXIT_FAILURE);
            }

            fclose(output);
        }

        execvp(cmd->argv[0], cmd->argv);

        perror(cmd->argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!cmd->background)
        waitpid(pid, NULL, 0);

    return 0;
}
