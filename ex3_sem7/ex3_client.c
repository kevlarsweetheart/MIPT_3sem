#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "my_types.h"

int main()
{
	
	char path_name[] = "ex3_client.c";
	
	struct mymsgbuf req_mes;
	printf("Please, enter your request\n");
	scanf("%d%d", &req_mes.x, &req_mes.y);
	req_mes.pid = getpid();
	req_mes.mtype = 1;

	struct mymsgbuf2 outdata;

	key_t key = ftok(path_name, 0);
	if (key < 0)
	{
		printf("Can't generate key\n");
		exit(-1);
	}
	int msqid = msgget(key, 0666 | IPC_CREAT);
	if (msqid < 0)
	{
		printf("Can't get msqid\n");
		exit(-1);
	}
	if (msgsnd(msqid, (struct msgbuf *) &req_mes, sizeof req_mes - sizeof(long), 0) < 0)
	{
		printf("Can't send message to queue\n");
		msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
		exit(-1);
	}

	if (msgrcv(msqid, (struct msgbuf *) &outdata, sizeof outdata - sizeof(long), req_mes.pid, 0) < 0)
	{
		printf("Can't receive a message from queue\n");
		exit(-1);
	}
	printf("Multiplication result: %d\n", outdata.res);
	return 0;
}
