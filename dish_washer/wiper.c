#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <string.h>

const char *path_name = "dishes.txt";

typedef struct {
    char *type;
    int time_to_wash, time_to_wipe, size;
} task;

typedef struct {
    long mtype;
    task dish;
} mymsgbuf;

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

    if (strcmp((char *)argv[1], "-mq") == 0)
    {
        int msqid = msgget(key, 0666 | IPC_CREAT);
    	if (msqid < 0)
    	{
    		printf("Can't get message queue id\n");
    		exit(-1);
    	}
        mymsgbuf request;
        while (1) {
            if (msgrcv(msqid, (struct msgbuf *) &request, sizeof request - sizeof(long), 3, 0) < 0)
            {
                printf("Can't receive a message from queue\n");
                exit(-1);
            }
            sleep(request.dish.time_to_wipe);
            printf("Done!\n");
        }
    }
    return 0;
}
