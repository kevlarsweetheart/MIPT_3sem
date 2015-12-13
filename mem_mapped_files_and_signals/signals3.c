#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

const int max_len = 100;

void send_symbol(char symb, pid_t pid, int *sig, sigset_t *set)
{
    char bit;
    int i, bound = (sizeof symb) * 8;
    for (i = 0; i < bound; ++i)
    {
        bit = 1 & symb;
	/*
	 * Тернарный оператор - довольно удобная штука. Можно писать
	 * kill(pid, bit ? SIGUSR1 : SIGUSR2);
	 * вместо ваших 4х строк.
	 */
        if (bit == 1)
            kill(pid, SIGUSR1);
        else
            kill(pid, SIGUSR2);
        symb = symb >> 1;
        sigwait(set, sig);
    }
}

char get_symbol(int *sig, sigset_t *set, pid_t pid)
{
    char symb = 0;
    int i, bound = (sizeof symb) * 8;
    for (i = 0; i < bound; ++i)
    {
        sigwait(set, sig);
        if (*sig == SIGUSR1)
            symb = symb | (1 << i);
        else if (*sig == SIGUSR2)
            symb = symb & ~(1 << i);
        kill(pid, SIGUSR1);
    }
    return symb;
}

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
        pid = getppid();
        char msg[max_len];
        printf("Type a message u wanna send\n");
        gets(msg);
        int len = strlen(msg);
        send_symbol((char)len, pid, &sig, &set);
        int i;
        for (i = 0; i < len; ++i)
            send_symbol(msg[i], pid, &sig, &set);
    }
    else
    {
        char msg[max_len];
        int i, len = get_symbol(&sig, &set, pid);
        for (i = 0; i < len; ++i)
          msg[i] = get_symbol(&sig, &set, pid);
         printf("Got the message %s!\n", msg);
         wait(NULL);
    }
    return 0;
}
