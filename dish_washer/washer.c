#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const int IP_LEN = 16;
const int DSCRPT_LEN = 64;
const int NUM_OF_TASKS = 1;
const int MAX_RESPONSE = 128;
const char *delimiters = "\t\n ";
const char *path_name = "dishes.txt";

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

void split(char *string, const char *delimiters, char ***tokens,
	   int *tokens_count)
{
	char *token;
	int tokens_size = 0, next_size = 4, cnt = 0;
	char **buff_tokens = *tokens;
	for (token = strtok(string, delimiters); token != NULL;
	     token = strtok(NULL, delimiters)) {
		if (cnt == tokens_size) {
			tokens_size = next_size;
			next_size *= 2;
			buff_tokens =
			    (char **)realloc(buff_tokens,
					     tokens_size * sizeof(char *));
		}
		buff_tokens[cnt++] = token;
	}
	*tokens = buff_tokens;
	*tokens_count = cnt;
}

void get_tasks(FILE * file, int num, task ** tasks)
{
	int i;
	char **buff_args = (char **)malloc(5 * sizeof(char *));
	for (i = 0; i < num; ++i) {
        int cnt = 0;
		char *current_task =
		    (char *)malloc(DSCRPT_LEN * sizeof(char));
		fgets(current_task, DSCRPT_LEN, file);
		split(current_task, delimiters, &(buff_args),
		      &cnt);
		(*tasks)[i].time_to_wash = atoi(buff_args[1]);
		(*tasks)[i].time_to_wipe = atoi(buff_args[2]);
		(*tasks)[i].type = buff_args[0];
        (*tasks)[i].size = atoi(buff_args[3]);
	}
	free(buff_args);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Wrong number of arguments\nRemember, there should be only one\n");
        exit(-1);
    }

    FILE *infile = fopen("dishes.txt", "r");
    task *tasks = (task *)malloc(NUM_OF_TASKS * sizeof(task));
    get_tasks(infile, NUM_OF_TASKS, &tasks);
    key_t key = ftok(path_name, 0);
    if (key < 0)
    {
        printf("Can't generate key\n");
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
        int i;
        for (i = 0; i < NUM_OF_TASKS; ++i)
        {
            printf("Washing a %s...\n", tasks[i].type);
            sleep(tasks[i].time_to_wash);
            mymsgbuf mes;
            mes.mtype = 1;
            mes.dish = tasks[i];
            if (msgsnd(msqid, (struct msgbuf *) &mes, sizeof mes - sizeof(long), 0) < 0)
        	{
        		printf("Can't send message to queue\n");
        		msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
        		exit(-1);
        	}

            mymsgbuf2 outdata;
            if (msgrcv(msqid, (struct msgbuf *) &outdata, sizeof outdata - sizeof(long), 2, 0) < 0)
        	{
        		printf("Can't receive a message from queue\n");
        		exit(-1);
        	}
            if (outdata.flag == 1)
                printf("The %s's been successfully washed and sent to the table\n", tasks[i].type);
            else
            {
                printf("\nThe table is completely occupied already\n");
                printf("And honestly, I have no idea how it could happen :/\n");
                printf("Now only Mulder and Scully can help us find the truth which is still out there\n\n");
                exit(-1);
            }
        }
    }
    return 0;
}
