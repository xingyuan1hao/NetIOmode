//
//¿Í»§¶Ë£º
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
 
 
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8000
 
 
#define MAX_RECV_LEN 1024
 
 
static int running = 1;
 
 
int main(int argc, char *argv[])
{
	int sock_fd = -1;
	int ret = -1;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	socklen_t serv_addr_len = 0;
 
	memset(&serv_addr, 0, sizeof(serv_addr));
 
 
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0)
	{
		perror("Fail to socket");
		exit(1);
	}
 
 
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVER_PORT);
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
 
 
	serv_addr_len = sizeof(serv_addr);
	if (connect(sock_fd, (struct sockaddr *)&serv_addr, serv_addr_len) < 0)
	{
		perror("Fail to connect");
		exit(1);
	}
	char buf[1024];
	int num = 0;
	while (running)
	{
		num = read(STDIN_FILENO, buf, MAX_RECV_LEN);
		if (num > 0)
		{
			buf[num] = '\0';
			printf("buf: %s\n", buf);
			num = write(sock_fd, buf, num);
			if (num < 0)
			{
				printf("write failed\n");
				exit(1);
			}
 
 
			if (strncmp(buf, "exit", strlen("exit")) == 0)
			{
				printf("Client exit\n");
				close(sock_fd);
				return 0;
			} 
		}
	}
	return 0;
}
