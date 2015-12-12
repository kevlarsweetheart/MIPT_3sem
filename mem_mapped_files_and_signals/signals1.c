#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    int sig;
    sigset_t set, oldest;
    sigemptyset(&set);
    sigemptyset(&oldest);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    sigprocmask(SIG_SETMASK, &set, &oldest);

    pid_t pid = fork();
    if (pid == 0)
    {
	    signal(SIGUSR1, SIG_IGN);
		pid = getppid();
        while(1)
        {
            printf("DCAM\n");
            kill(pid, SIGUSR2);
            sigwait(&set, &sig);
        }
    }
    else
    {
	    signal(SIGUSR2, SIG_IGN);
        while(1)
        {
            sigwait(&set, &sig);
            printf("champion!\n");
            kill(pid, SIGUSR1);
        }
    }
    return 0;
}
