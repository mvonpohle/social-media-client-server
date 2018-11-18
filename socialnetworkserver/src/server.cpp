/*
 * server.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: pournami
 */

#include "func_lib.h"

using namespace std;

#define QUEUE_LEN	15
#define DEBUG	printf

int serverInit(string port);
int acceptConnections(int master_fd);
void terminateClient(int slave_fd);

/**
 * serverInit() - Create server socket, bind to port and listen on socket for incoming connections
 * port: server port
 * return: master socket file descriptor
 */
int serverInit(string port)
{
	int master_fd, sock_bind, sock_listen, slave_fd, sock_close;
	struct sockaddr_in serv_addr;
	int opt = 1;

	/* Create Socket */
	master_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (master_fd < 0)
	{
		printf("Error (socket): %s\n", strerror(errno));
		return -1;
	}
	DEBUG("socket()\n");
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_port			= htons(stoi(port));
	serv_addr.sin_addr.s_addr	= htonl(INADDR_ANY);
	setsockopt(master_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	/* Bind Socket */
	sock_bind = bind(master_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (sock_bind < 0)
	{
		printf("Error (bind): %s\n", strerror(errno));
		return -1;
	}
	DEBUG("bind()\n");
	/* Listen on the socket */
	sock_listen = listen(master_fd, QUEUE_LEN);
	if (sock_listen < 0)
	{
		printf("Error (listen): %s\n", strerror(errno));
		return -1;
	}
	DEBUG("listen()\n");
	return master_fd;
}

/*
 * acceptConnections() - accept client connections
 * master_fd: file descriptor of master socket
 * return -1(Error) else stay infinitely
 */
int acceptConnections(int master_fd)
{
	struct sockaddr_in client_addr;
	unsigned int client_len = sizeof(client_addr);
	pthread_t thread;
	pthread_attr_t attr;
	int create_thrd, slave_fd;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	while(1)
	{
		memset(&client_addr, 0, sizeof(client_addr));
		/* Accept client requests */
		slave_fd = accept(master_fd, (struct sockaddr *)&client_addr, &client_len);
		if (slave_fd < 0)
		{
			if (errno == EINTR)
				continue;
			printf("Error (accept): %s\n", strerror(errno));
			return -1;
		}
		DEBUG("accept()\n");
		/* Create thread for each clients */
		create_thrd = pthread_create(&thread, &attr, (void * (*) (void *)) handleClient, (void *)((long) slave_fd));
		if (create_thrd < 0)
		{
			printf("Error (pthread_create): %s\n", strerror(errno));
			return -1;
		}
	}
}

void terminateClient(int slave_fd)
{
	close(slave_fd);
	pthread_exit(NULL);
}
