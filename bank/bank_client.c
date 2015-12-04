#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

const int max_request = 12;
const int max_response = 512;
const int port = 51000;

int main(int argc, char **argv)
{
	char *request = (char *)malloc(max_request * sizeof(char));
	bzero(request, max_request);
	printf("Please, enter your request\n");
	gets(request);
	int sockfd, n;
	char *recieved = (char *)malloc(max_response * sizeof(char));
	struct sockaddr_in servaddr;
	if (argc != 2)
	{
		printf("You've forgotten IP address\n");
		exit(1);
	}
	bzero(recieved, max_response);
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror(NULL);
		exit(1);
	}
	bzero(&servaddr, sizeof servaddr);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	if (inet_aton(argv[1], &servaddr.sin_addr) == 0)
	{
		printf("Invalid IP address\n");
		close(sockfd);
		exit(1);
	}

	if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		perror("Can't connect\n");
		close(sockfd);
		exit(1);
	}

	if ((n = write(sockfd, request, strlen(request) + 1)) < 0)
	{
		perror("Can't write\n");
		close(sockfd);
		exit(1);
	}

	if ((n = read(sockfd, recieved, max_response)) < 0)
	{
		perror("Can't read\n");
		close(sockfd);
		exit(1);
	}
	printf("%s", recieved);
	free(request);
	free(recieved);
	return 0;
}
