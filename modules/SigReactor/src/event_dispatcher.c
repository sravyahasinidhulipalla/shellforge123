#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

#include "event_dispatcher.h"

void dispatch_event(char event)
{
    printf("\n[REACTOR] Event received\n");

    if (event == SIGUSR1)
    {
        printf("[EVENT] SIGUSR1 -> User event handled\n");
    }
    else if (event == SIGUSR2)
    {
        printf("[EVENT] SIGUSR2 -> User event handled\n");
    }
    else if (event == SIGCHLD)
    {
        printf("[EVENT] SIGCHLD -> Child process event\n");

        int status;

        while (waitpid(-1, &status, WNOHANG) > 0)
        {
            printf("[PROCESS] Child cleaned using waitpid()\n");
        }
    }
    else if (event == SIGINT)
    {
        printf("[EVENT] SIGINT -> Shutdown event\n");
    }
    else
    {
        printf("[EVENT] Unknown event received\n");
    }

    printf("[REACTOR] Event dispatched successfully\n");
}
