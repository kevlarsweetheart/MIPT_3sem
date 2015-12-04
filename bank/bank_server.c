#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sem.h>

/*
 * Засчитано. Посмотрите на замечания.
 */

const int initial_sum = 1000000;
const int max_request = 12;
const int port = 51000;
const int max_depth = 5;
const char *ok_msg = "Your request's been executed successfully\n";
const char *err_msg = "Not enough money in ATM to execute your request\n";

int curr_sum;

struct msg {
	int sem_id, sock;
	struct sockaddr_in cliaddr;
};

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

void change_sem(int sem_id, int sem, int val)
{
	struct sembuf my_buf;
	my_buf.sem_op = val;
	my_buf.sem_num = sem;
	my_buf.sem_flg = 0;
	semop(sem_id, &my_buf, 1);
}

void *handle_request(void *data)
{
	char *recieved = (char *)malloc(max_request * sizeof(char));
	struct msg message;
	message = *(struct msg *)data;
	change_sem(message.sem_id, 1, 1);
	change_sem(message.sem_id, 0, -1);
	/*
	 * Критическая секция - это работа с глобальной переменной curr_sum.
	 * Все остальное можно выполнять параллельно.
	 * Чем меньше размер критической секции, тем лучше.
	 * Т.е. lock следовало бы сделать, например, после int sum = atoi(recieved);
	 */
	int err_flag = read(message.sock, recieved, max_request);
	if (err_flag < 0)
	{
		printf("Can't read your request\n");
		close(message.sock);
		exit(1);
	}
	int sum = atoi(recieved);
	if (sum > 0)
	{
		curr_sum += sum;
		/*
		 * У вас 3 раза написан почти один и тот же кусок кода.
		 * Это совсем не здорово.
		 */
		err_flag = write(message.sock, ok_msg, strlen(ok_msg) + 1);
		if (err_flag < 0)
		{
			perror(NULL);
			close(message.sock);
			exit(1);
		}
		close(message.sock);
	}
	else
	{
		if (curr_sum + sum < 0)
		{
			err_flag = write(message.sock, err_msg, strlen(err_msg) + 1);
			if (err_flag < 0)
			{
				perror(NULL);
				close(message.sock);
				exit(1);
			}
			close(message.sock);
		}
		else
		{
			curr_sum += sum;
			err_flag = write(message.sock, ok_msg, strlen(ok_msg) + 1);
			if (err_flag < 0)
			{
				perror(NULL);
				close(message.sock);
				exit(1);
			}
			close(message.sock);
		}
	}
	change_sem(message.sem_id, 0, 1);
}

int main()
{
	curr_sum = initial_sum;
	char pathname[] = "bank_client.c";
	key_t key = ftok(pathname, 0);
	int sockfd;
	struct sockaddr_in servaddr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror(NULL);
		exit(1);
	}
	bzero(&servaddr, sizeof servaddr);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof servaddr) < 0)
	{
		perror(NULL);
		close(sockfd);
		exit(1);
	}
	if (listen(sockfd, max_depth) < 0)
	{
		perror(NULL);
		close(sockfd);
		exit(1);
	}
	struct msg data;
	data.sem_id = init_semaphore(key);
	change_sem(data.sem_id, 0, max_depth);
	while (1) {
		int clilen = sizeof data.cliaddr;
		if ((data.sock = accept(sockfd, (struct sockaddr *)&(data.cliaddr), &clilen)) < 0)
		{
			perror(NULL);
			close(sockfd);
			exit(1);
		}
		pthread_t thread;
		int exec_res = pthread_create(&thread, (pthread_attr_t *)NULL, handle_request, &data);
		change_sem(data.sem_id, 1, -1);
	}
	return 0;
}
