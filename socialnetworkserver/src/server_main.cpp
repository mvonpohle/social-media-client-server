/*
 * main.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: pournami
 */
#include <stdio.h>
#include <string>
#include <iostream>
#include "func_lib.h"
#include "networking.h"
#include "mysql_lib.h"

#define SERVER_URL "tcp://127.0.0.1:3306"
#define SERVER_USERNAME "root"
#define SERVER_PASSWORD "asdfgh"
#define SERVER_DATABASE "SocialNetwork"
#define DEBUG printf
using namespace std;

pthread_cond_t notify_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t notify_mutex = PTHREAD_MUTEX_INITIALIZER;

MySQLDatabaseDriver databaseDriver;
MySQLDatabaseInterface database(&databaseDriver, SERVER_URL, SERVER_USERNAME,
								SERVER_PASSWORD, SERVER_DATABASE);

int main(int argc, char *argv[])
{
	int port = 5354;
	int master_fd, accept_conn;
	pthread_t notifyThread, clientThread;
	pthread_attr_t attr;
	int create_thrd, slave_fd;
	int ret;

	switch (argc)
	{
	case 1:
			break;
	case 2:
			port = stoi(argv[1]);
			break;
	default:
			printf("Error: Usage is ./<executable> [port]\n");
			return -1;
	}
	/*
	master_fd = serverInit(port);
	if (master_fd < 0)
	{
		printf("Error (serverInit): Server Initialization failed\n");
		return -1;
	}
	*/
	DEBUG("calling create socket\n");
	master_fd = create_server_socket(port);
	if (master_fd < 0)
	{
		printf("Error (create_server_socket): Socket creation error\n");
		return -1;
	}
	DEBUG("socket created\n");
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	ret = pthread_create(&notifyThread, &attr, (void * (*) (void *)) processNotification, NULL);
	if (ret < 0)
	{
		printf("Error (pthread_create): %s\n", strerror(errno));
		return -1;
	}
	DEBUG("notify thread created\n");
	while(1)
	{
		//memset(&client_addr, 0, sizeof(client_addr));
		/* Accept client requests */
		slave_fd = accept_socket(master_fd);
		/*
		slave_fd = accept(master_fd, (struct sockaddr *)&client_addr, &client_len);
		*/
		if (slave_fd < 0)
		{
			if (slave_fd == EINTR)
				continue;
			printf("Error (accept): %s\n", strerror(errno));
			return -1;
		}
		DEBUG("accept()\n");
		/* Create thread for each clients */
		create_thrd = pthread_create(&clientThread, &attr, (void * (*) (void *)) handleClient, (void *)((long) slave_fd));
		if (create_thrd < 0)
		{
			printf("Error (pthread_create): %s\n", strerror(errno));
			return -1;
		}
	}
	return 0;
}
