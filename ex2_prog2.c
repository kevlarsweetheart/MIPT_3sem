#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

int factorial(int n)
{
	int res = 1, i;
	for (i = 2; i <= n; i++)
		res *= i;
	return res;
}

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
	if (msgrcv(msqid, (struct msgbuf *) &mybuf, sizeof mybuf - sizeof(long), 1, 0) < 0)
	{
		printf("Can't receive a message from queue\n");
		exit(-1);
	}
	mybuf.mtype = 2;
	mybuf.msg_num = factorial(mybuf.msg_num);
	if (msgsnd(msqid, (struct msgbuf *) &mybuf, sizeof mybuf - sizeof(long), 0) < 0)
	{
		printf("Can't send a message to queue\n");
		msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
		exit(-1);
	}
	return 0;
}
