#include <stdio.h>
#include <signal.h>

void *handler(int sig) {
    printf("Signals are fun!\n");
}

int main()
{
    (void)signal(SIGINT, SIG_IGN);
    (void)signal(SIGQUIT, handler);
    while (1);
    return 0;
}
