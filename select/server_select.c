//����ˣ�
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <errno.h>
 
 
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8000
 
 
#define MAX_RECV_LEN 1024
#define MAX_CLIENT_NUM 30
#define BACK_LOG 20
 
 
static int running = 1;
 
 
int main(int argc, char *argv[])
{
	int sock_fd = -1;
	int ret = -1;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	socklen_t serv_addr_len = 0;
	socklen_t cli_addr_len = 0;
	int client_fd[MAX_CLIENT_NUM];
	char recv_buf[MAX_RECV_LEN];
	int new_conn_fd = -1;
	int i = 0;
	int max_fd = -1;
	int num = -1;
	struct timeval timeout;
 
 
	fd_set read_set;
	fd_set write_set;
	fd_set select_read_set;
 
 
	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	FD_ZERO(&select_read_set);
 
	for (i = 0; i < MAX_CLIENT_NUM; i++)
	{
		client_fd[i] = -1;
	} 
 
	memset(&serv_addr, 0, sizeof(serv_addr));
	memset(&cli_addr, 0, sizeof(cli_addr));
 
 
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0)
	{
		perror("Fail to socket");
		exit(1);
	}
 
 
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVER_PORT);
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
 
 
	unsigned int value = 1;
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR,
				(void *)&value, sizeof(value)) < 0)
	{
		perror("Fail to setsockopt");
		exit(1);
	}
 
 
	serv_addr_len = sizeof(serv_addr);
	if (bind(sock_fd, (struct sockaddr*)&serv_addr, serv_addr_len) < 0)
	{
		perror("Fail to bind");
		exit(1);
	}
	if (listen(sock_fd, BACK_LOG) < 0)
	{
		perror("Fail to listen");
		exit(1);
	}
 
	char buf[1024];
	max_fd = sock_fd;
	int len;
	FD_SET(sock_fd, &read_set);
	while (running)
	{
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
 
 
		max_fd = sock_fd;
		for (i = 0; i < MAX_CLIENT_NUM; i++)
		{
			if (max_fd < client_fd[i])
			{
				max_fd = client_fd[i];
			}
		}
 
 
		select_read_set = read_set;
		ret = select(max_fd + 1, &select_read_set, NULL, NULL, &timeout);
		if (ret == 0)
		{
			printf("timeout\n");
		}
		else if (ret < 0)
		{
			printf("error occur\n");
		}
		else
		{
			if (FD_ISSET(sock_fd, &select_read_set))
			{
				printf("new client comes\n");
				len = sizeof(cli_addr);
				new_conn_fd = accept(sock_fd, (struct sockaddr*)&cli_addr, &len);
				if (new_conn_fd < 0)
				{
					perror("Fail to accept");
					exit(1);
				}
				else
				{
					for (i = 0; i < MAX_CLIENT_NUM; i++)
					{
						if (client_fd[i] == -1)
						{
							client_fd[i] = new_conn_fd;
							FD_SET(new_conn_fd, &read_set);
							break;
						}
						if (max_fd < new_conn_fd)
						{
							max_fd = new_conn_fd;
						}
					}
				}
			}
			else
			{
				for (i = 0; i < MAX_CLIENT_NUM; i++)
				{
					if (-1 == client_fd[i]) {
						continue;
					}
					memset(recv_buf, 0, MAX_RECV_LEN);
					if (FD_ISSET(client_fd[i], &select_read_set))
					{
						num = read(client_fd[i], recv_buf, MAX_RECV_LEN);
						if (num < 0)
						{
							printf("Client(%d) left\n", client_fd[i]);
							FD_CLR(client_fd[i], &read_set);
							close(client_fd[i]);
							client_fd[i] = -1;
						}
						else if (num > 0)
						{
							recv_buf[num] = '\0';
							printf("Recieve client(%d) data\n", client_fd[i]);
							printf("Data: %s\n\n", recv_buf);
						} if (num == 0)
						{
							printf("Client(%d) exit\n", client_fd[i]);
							FD_CLR(client_fd[i], &read_set);
							close(client_fd[i]);
							client_fd[i] = -1;
						}
					}
				}
			}
		}
	}
	return 0;
}
//