
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int value = 100;

    printf("Before fork: value = %d\n", value);
    fflush(stdout);

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        value = 200;
        printf("Child process: value = %d\n", value);
        printf("Child PID: %d\n", getpid());
        _exit(0);
    }
    else
    {
        if (waitpid(pid, NULL, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        printf("Parent process: value = %d\n", value);
        printf("Parent PID: %d\n", getpid());
    }

    return 0;
}
