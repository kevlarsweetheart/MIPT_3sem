#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

const int max_msg_len = 1024;
const int port = 51000;
const int max_clients = 10;

struct client {
    struct sockaddr_in client_data;
    char *client_name;
};

char *form_msg(char *name, char *msg)
{
    char *formed = (char *) malloc(max_msg_len * sizeof(char));
    strcpy(formed, name);
    strcat(formed, ": ");
    strcat(formed, msg);
    return formed;
}

int main()
{
    int sockfd;
    int clilen, n;
    char *line = (char *) malloc(max_msg_len * sizeof(char));
    struct sockaddr_in servaddr, cliaddr;
    int num = 0;
    struct client *users =
	(struct client *) malloc(max_clients * sizeof(struct client));

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

    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
	perror(NULL);
	close(sockfd);
	exit(1);
    }

    while (1) {
	bzero(line, sizeof line);
	clilen = sizeof cliaddr;
	if ((n = recvfrom(sockfd, line, max_msg_len - 1, 0,
			  (struct sockaddr *) &cliaddr, &clilen)) < 0) {
	    perror(NULL);
	    close(sockfd);
	    exit(1);
	}
	//printf("Addr: %d\n", cliaddr.sin_addr.s_addr);
	
	if (line[0] != '#') {
	    int i = 0;
	    for (i = 0; i < num; ++i) {
		if (users[i].client_data.sin_addr.s_addr ==
		    cliaddr.sin_addr.s_addr
		    && users[i].client_data.sin_port == cliaddr.sin_port)
		    break;
	    }
	    strcpy(line, form_msg(users[i].client_name, line));
	    //printf("i = %d\n", i);
	    //printf("%s\n", line);
	    for (i = 0; i < num; ++i) {
		if (sendto
		    (sockfd, line, strlen(line), 0,
		     (struct sockaddr *) &(users[i].client_data),
		     clilen) < 0) {
		    perror(NULL);
		    close(sockfd);
		    exit(1);
		}
	    }
	} else {
	    users[num].client_name = (char *)malloc(max_msg_len * sizeof(char));
	    strcat(users[num].client_name, line);
	    users[num].client_data.sin_addr.s_addr = cliaddr.sin_addr.s_addr;
	    users[num].client_data.sin_port = cliaddr.sin_port;;
	    num++;
	    printf("Freshman in the thread!\n");
	}
	/*printf("num = %d ", num);
	int counter = 0;
	for(counter; counter < num; counter++)
		printf("%s:%d:%d\t", users[counter].client_name, users[counter].client_data.sin_addr.s_addr, users[counter].client_data.sin_port);
	printf("\n");*/
    }
    return 0;
}
