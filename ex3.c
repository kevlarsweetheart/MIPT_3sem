#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int arr[3] = {0, 0, 0};

const int bound = 10000000;

void change_sem(int sem_id, int val)
{
	struct sembuf my_buf;
	my_buf.sem_op = val;
	my_buf.sem_num = 0;
	my_buf.sem_flg = 0;
	semop(sem_id, &my_buf, 1);
}

void *thread_1(void *dummy)
{
	int sem_id = *(int *)dummy;
	pthread_t thread_id;
	sleep(1);
	thread_id = pthread_self();
	int i, tmp = 0;
	for (i = 0; i < bound; ++i)
	{
		arr[0]++;
		tmp++;
		if ((i + 1) % 100 == 0)
		{
			change_sem(sem_id, -1);
			arr[1] = tmp;
			change_sem(sem_id, 1);
		}
	}
	printf("Thread id: %d\nResult: arr[0] = %d, arr[1] = %d\n\n", thread_id, arr[0], arr[1]);
	return NULL;
}

void *thread_2(void *dummy)
{
	int sem_id = *(int *)dummy;
	pthread_t thread_id;
	sleep(1);
	thread_id = pthread_self();
	int i, tmp = 0;
	for (i = 0; i < bound; ++i)
	{
		tmp++;
		if ((i + 1) % 100 == 0)
		{
			change_sem(sem_id, -1);
			arr[1] = tmp;
			change_sem(sem_id, 1);
		}
		arr[2]++;
	}
	printf("Thread id: %d\nResult: arr[1] = %d, arr[2] = %d\n\n", thread_id, arr[1], arr[2]);
	return NULL;
}

int main()
{
	char pathname[] = "ex3.c";
	key_t key = ftok(pathname, 0);
	int sem_id = semget(key, 1, IPC_CREAT | 0777);
	change_sem(sem_id, 1);

	pthread_t thread_id1, thread_id2, root_thread_id;
	int result = pthread_create(&thread_id1, (pthread_attr_t *)NULL, thread_1, &sem_id);
	if (result)
	{
		printf("Can't create thread 1, returned value: %d :(\n", result);
		exit(-1);
	}
	int res = pthread_create(&thread_id2, (pthread_attr_t *)NULL, thread_2, &sem_id);
	if (res)
	{
		printf("Can't create thread 2, returned value: %d :(\n", res);
		exit(-1);
	}
	pthread_join(thread_id1, (void **)NULL);
	pthread_join(thread_id2, (void **)NULL);
	printf("%d %d %d %d\n", arr[0], arr[1], arr[0] + arr[1], arr[2]);
	return 0;
}
