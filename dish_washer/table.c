#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <string.h>

const char *path_name = "dishes.txt";
const int TABLE_LIMIT = 2;

int occupied;

typedef struct {
    char *type;
    int time_to_wash, time_to_wipe, size;
} task;

typedef struct {
    long mtype;
    task dish;
} mymsgbuf;

typedef struct {
    long mtype;
    char flag;
} mymsgbuf2;

int init_queue(key_t key)
{
	int msqid = msgget(key, 0666 | IPC_CREAT);
	if (msqid < 0)
	{
		printf("Can't get message queue id T_T\n");
		exit(-1);
	}
	return msqid;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Wrong number of arguments\nRemember, there should be only one\n");
        exit(-1);
    }

    key_t key = ftok(path_name, 0);
    if (key < 0)
	{
		printf("Can't generate key :'(\n");
		exit(-1);
	}

    occupied = 0;

    if (strcmp((char *)argv[1], "-mq") == 0)
    {
        int msqid = msgget(key, 0666 | IPC_CREAT);
        if (msqid < 0)
        {
            printf("Can't get message queue id\n");
            exit(-1);
        }

        mymsgbuf request;
        mymsgbuf2 response;
        while (1) {
            if(msgrcv(msqid, (struct msgbuf *) &request, sizeof request - sizeof(long), 1, 0) < 0)
    		{
    			printf("Can't recieve a message from queu =>_<=\n");
    			exit(-1);
    		}
            response.mtype = 2;
            if (request.dish.size + occupied > TABLE_LIMIT)
                response.flag = 0;
            else
            {
                response.flag = 1;
                occupied += request.dish.size;
            }
            if (msgsnd(msqid, (struct msgbuf *) &response, sizeof response - sizeof(long), 0) < 0)
            {
                printf("Can't send a message to queue *sad girl smile*\n");
                msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
                exit(-1);
            }
            request.mtype = 3;
            if (response.flag == 1)
            {
                if (msgsnd(msqid, (struct msgbuf *) &request, sizeof request - sizeof(long), 0) < 0)
                {
                    printf("Can't send a message to queue *sad girl smile*\n");
                    msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
                    exit(-1);
                }
                occupied -= request.dish.size;
            }
        }
    }
    return 0;
}
