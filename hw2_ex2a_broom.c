#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

void main(void)
{
	int cnt, i, status;
	pid_t ret_val;
	scanf("%d", &cnt);
	for (i = 0; i < cnt; ++i) {
		pid_t pid = fork();
		if (pid == 0) {
			sleep(1);
			exit(i);
		}
	}
	for (i = 0; i < cnt; ++i) {
		ret_val = wait(&status);
		printf("returned child id: %d\nstatus: %d\n", ret_val, status);
	}
}
