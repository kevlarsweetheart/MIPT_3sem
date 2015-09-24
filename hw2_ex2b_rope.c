#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

void main(void)
{
	int cnt, i;
	scanf("%d", &cnt);
	int status;
	pid_t root_id = getpid(), pid = 0;
	pid_t current_id, parent_id;
	printf("Root process id: %d\n\n", root_id);
	for (i = 0; i < cnt - 1; ++i) {
		if (pid != 0)
			break;
		pid = fork();
		if (pid == 0) {
			sleep(1);
			current_id = getpid();
			parent_id = getppid();
			printf
			    ("Newly created process id: %d\nIts parent process id: %d\n\n",
			     current_id, parent_id);
		}
	}
	if (pid == 0)
		exit(0);
	pid_t child_id = wait(&status);
	current_id = getpid();
	printf("Terminated child process %d with status %d\n", child_id,
	       status);
	exit(0);
}
