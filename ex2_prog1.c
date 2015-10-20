#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	char path_name[] = "ex2_prog1.c";

	struct mymsgbuf
	{
		long mtype;
		int msg_num;
	} mybuf;

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
	int n;
	scanf("%d", &n);
	mybuf.mtype = 1;
	mybuf.msg_num = n;
	if (msgsnd(msqid, (struct msgbuf *) &mybuf, sizeof mybuf - sizeof(long), 0) < 0)
	{
		printf("Can't send message to queue\n");
		msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
		exit(-1);
	}

	if (msgrcv(msqid, (struct msgbuf *) &mybuf, sizeof mybuf - sizeof(long), 2, 0) < 0)
	{
		printf("Can't receive a message from queue\n");
		exit(-1);
	}
	printf("Factorial result: %d\n", mybuf.msg_num);
	msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
	return 0;
}
