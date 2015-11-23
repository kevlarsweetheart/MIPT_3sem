#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

//Местами очень странное форматирование - результат работы indent -linux

const int max_msg_len = 1024;
const int port = 51000;

struct client {
	struct sockaddr_in client_data;
	char *client_name;
};

char *form_msg(char *name, char *msg)
{
	char *formed = (char *)malloc(max_msg_len * sizeof(char));
	strcpy(formed, name);
	strcat(formed, ": ");
	strcat(formed, msg);
	return formed;
}

char *sender_search(struct sockaddr_in sender, struct client *users, int *num,
		    char *name)
{
	int ind = 0;
	while (ind < *num && !
	       (users[ind].client_data.sin_addr.s_addr == sender.sin_addr.s_addr
		&& users[ind].client_data.sin_port == sender.sin_port))
		ind++;
	if (ind == *num) {
		users =
		    (struct client *)realloc(users,
					     (ind +
					      1) * sizeof(struct client));
		users[ind].client_name =
		    (char *)malloc(strlen(name) * sizeof(char) + 1);
		strcpy(users[ind].client_name, name);
		users[ind].client_data = sender;
		*num = ind + 1;
		return name;
	} else {
		return users[ind - 1].client_name;
	}
}

int main(){
	int sockfd;
	int clilen, n;
	char *line = (char *)malloc(max_msg_len * sizeof(char));
	struct sockaddr_in servaddr, cliaddr;
	int num = 0;
	struct client *users = NULL;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror(NULL);
		exit(1);
	} else {
		printf("Sock fd:%d\n", sockfd);
	}

	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror(NULL);
		close(sockfd);
		exit(1);
	}

	while (1) {
		bzero(line, sizeof line);
		clilen = sizeof cliaddr;
		if ((n = recvfrom(sockfd, line, max_msg_len - 1, 0,
				  (struct sockaddr *)&cliaddr, &clilen)) < 0) {
			perror(NULL);
			close(sockfd);
			exit(1);
		}

		cliaddr.sin_port = port + 1;
		char *user = sender_search(cliaddr, users, &num, line);
		line = form_msg(user, line);
		int i;
		for (i = 0; i < num; ++i) {
			if (sendto(sockfd, line, strlen(line), 0,
				   (struct sockaddr *)&(users[i].client_data),
				   clilen) < 0) {
				perror(NULL);
				close(sockfd);
				exit(1);
			}
		}
	}
	return 0;
}
