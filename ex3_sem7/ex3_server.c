#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>

const int max_clients = 2;

struct thread_data
{
	int msqid, sem_id, x, y;
	pid_t pid;
};

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

int init_semaphore(key_t key)
{
	int sem_id = semget(key, 1, 0666 | IPC_CREAT);
	if (sem_id < 0)
	{
		printf("Can't get semaphore id >_<\n");
		exit(-1);
	}
	return sem_id;
}

void change_sem(int sem_id, int val)
{
	struct sembuf my_buf;
	my_buf.sem_op = val;
	my_buf.sem_num = 0;
	my_buf.sem_flg = 0;
	semop(sem_id, &my_buf, 1);
}

void *create_thread(void *dummy)
{
	struct thread_data data;
	data = *(struct thread_data *)dummy;
	change_sem(data.sem_id, -1);
	struct mymsgbuf2
	{
		long mtype;
		int res;
	} outdata;
	outdata.mtype = data.pid;
	outdata.res = data.x * data.y;
	if (msgsnd(data.msqid, (struct msgbuf *) &outdata, sizeof outdata - sizeof(long), 0) < 0)
	{
		printf("Can't send a message to queue *sad girl smile*\n");
		msgctl(data.msqid, IPC_RMID, (struct msqid_ds *)NULL);
		exit(-1);
	}
	change_sem(data.sem_id, 1);
}

int main()
{
	char pathname[] = "ex3_client.c";
	key_t key = ftok(pathname, 0);
	if (key < 0)
	{
		printf("Can't generate key :'(\n");
		exit(-1);
	}

	struct thread_data data;
	data.msqid = init_queue(key);
	data.sem_id = init_semaphore(key);
	struct mymsgbuf
	{
		long mtype;
		pid_t pid;
		int x, y;
	} req_mes;

	change_sem(data.sem_id, max_clients);

	while (1)
	{
		if(msgrcv(data.msqid, (struct msgbuf *) &req_mes, sizeof req_mes - sizeof(long), 1, 0) < 0)
		{
			printf("Can't recieve a message from queu =>_<=\n");
			exit(-1);
		}
		data.pid = req_mes.pid;
		data.x = req_mes.x;
		data.y = req_mes.y;
		pthread_t thread;
    /*
     * У вас главная нить и дочерние нити после создания живут своей жизнью.
     * В общем случае ничего не препятствует ситуации, когда главная нить получит следующее сообщение и перезапишет структуру data, в то время как
     * дочерняя ещё не успеет сохранить её в свою локальную переменную:
     *  struct thread_data data;
        data = *(struct thread_data *)dummy;
       Вы можете спорить по этому поводу, но надо будет показывать, что в независимости от алгоритма планирования задач и прочих внешних факторов, такой ситуации не произойдёт.
       Лучше же просто ввести дополнительный семафор, гарантирующий, что структура data в основной нити перезапишется только после того, как в ф-и нити она сохранится в локальную переменную.
     */
		int exec_res = pthread_create(&thread, (pthread_attr_t *)NULL, create_thread, &data);
	}
	return 0;
}
