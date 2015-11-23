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

const int max_msg_len = 1024;
const int port = 51000;

void *recieve(void *dummy)
{
	sleep(1);
	int sockfd = *(int *)dummy;
	char *msg = (char *)malloc(max_msg_len * sizeof(char));
	while (1) {
		bzero(msg, sizeof msg);
		int err_flag =
		    recvfrom(sockfd, msg, max_msg_len, 0,
			     (struct sockaddr *)NULL, NULL);
		if (err_flag < 0) {
			perror(NULL);
			close(sockfd);
			exit(1);
		}
		printf("%s", msg);
	}
}

int main(int argc, char **argv)
{
	int sockfd;
	int n, len;
	char sendline[max_msg_len];
	struct sockaddr_in servaddr, cliaddr;

	if (argc != 2) {
		printf("Usage: ./a.out <IP address>\n");
		exit(1);
	}
	if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror(NULL);
		exit(1);
	}

	bzero(&cliaddr, sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(0);
	cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0) {
		perror(NULL);
		close(sockfd);
		exit(1);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	if (inet_aton(argv[1], &servaddr.sin_addr) == 0) {
		printf("Invalid IP address\n");
		close(sockfd);
		exit(1);
	}

	pthread_t thread;
	pthread_create(&thread, (pthread_attr_t *) NULL, recieve, &sockfd);

	printf("Type your name below\n");
	scanf("%s", &sendline);
	int err_flag =
	    sendto(sockfd, sendline, strlen(sendline) + 1, 0,
		   (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (err_flag < 0) {
		perror(NULL);
		close(sockfd);
		exit(1);
	}
	while (1) {
		fgets(sendline, max_msg_len, stdin);
		err_flag =
		    sendto(sockfd, sendline, strlen(sendline) + 1, 0,
			   (struct sockaddr *)&servaddr, sizeof servaddr);
		if (err_flag < 0) {
			perror(NULL);
			close(sockfd);
			exit(1);
		}
	}
	return 0;
}
