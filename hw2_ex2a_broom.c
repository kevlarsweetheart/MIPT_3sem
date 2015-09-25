#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

void main()
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
/*
 * Ожидатся, что раз дочерний процесс сделал exit(i), то родительский выведет в качестве статуса завершения ребёнка именно i.
 */
	}
}
