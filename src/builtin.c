#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "jobs.h"
#include "executor.h"

static int builtin_id(command_t *cmd)
{
    if (cmd->argc != 2)
    {
        fprintf(stderr,
                "id: usage: id <number>\n");

        return -1;
    }

    printf("ID: %s\n", cmd->argv[1]);

    return 0;
}

static int builtin_cd(command_t *cmd)
{
    if (cmd->argc != 2)
    {
        fprintf(stderr,
                "cd: usage: cd <directory>\n");

        return -1;
    }

    if (chdir(cmd->argv[1]) < 0)
    {
        perror("cd");
        return -1;
    }

    return 0;
}

static int builtin_pwd(command_t *cmd)
{
    char cwd[1024];

    (void)cmd;

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("pwd");
        return -1;
    }

    printf("%s\n", cwd);

    return 0;
}

static int builtin_echo(command_t *cmd)
{
    for (int i = 1; i < cmd->argc; i++)
    {
        printf("%s", cmd->argv[i]);

        if (i < cmd->argc - 1)
            printf(" ");
    }

    printf("\n");

    return 0;
}

static int builtin_jobs(command_t *cmd)
{
    (void)cmd;

    jobs_reap();
    jobs_print();

    return 0;
}

static int builtin_fg(command_t *cmd)
{
    if (cmd->argc != 2)
    {
        fprintf(stderr,
                "fg: usage: fg <job_id>\n");

        return -1;
    }

    int id = atoi(cmd->argv[1]);

    job_t *job = job_find(id);

    if (job == NULL)
    {
        fprintf(stderr,
                "fg: job %d not found\n",
                id);

        return -1;
    }

    return foreground_job(job);
}

static int builtin_bg(command_t *cmd)
{
    if (cmd->argc != 2)
    {
        fprintf(stderr,
                "bg: usage: bg <job_id>\n");

        return -1;
    }

    int id = atoi(cmd->argv[1]);

    job_t *job = job_find(id);

    if (job == NULL)
    {
        fprintf(stderr,
                "bg: job %d not found\n",
                id);

        return -1;
    }

    return background_job(job);
}

int is_builtin(const command_t *cmd)
{
    if (cmd == NULL ||
        cmd->argc == 0)
    {
        return 0;
    }

    return strcmp(cmd->argv[0], "cd") == 0 ||
           strcmp(cmd->argv[0], "pwd") == 0 ||
           strcmp(cmd->argv[0], "echo") == 0 ||
           strcmp(cmd->argv[0], "id") == 0 ||
           strcmp(cmd->argv[0], "exit") == 0 ||
           strcmp(cmd->argv[0], "jobs") == 0 ||
           strcmp(cmd->argv[0], "fg") == 0 ||
           strcmp(cmd->argv[0], "bg") == 0;
}

int execute_builtin(command_t *cmd)
{
    if (strcmp(cmd->argv[0], "cd") == 0)
        return builtin_cd(cmd);

    if (strcmp(cmd->argv[0], "pwd") == 0)
        return builtin_pwd(cmd);

    if (strcmp(cmd->argv[0], "echo") == 0)
        return builtin_echo(cmd);

    if (strcmp(cmd->argv[0], "id") == 0)
        return builtin_id(cmd);

    if (strcmp(cmd->argv[0], "jobs") == 0)
        return builtin_jobs(cmd);

    if (strcmp(cmd->argv[0], "fg") == 0)
        return builtin_fg(cmd);

    if (strcmp(cmd->argv[0], "bg") == 0)
        return builtin_bg(cmd);

    if (strcmp(cmd->argv[0], "exit") == 0)
        return 1;

    return -1;
}
