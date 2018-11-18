/*
 * networking.cpp
 *
 *  Created on: Nov 8, 2018
 *      Author: pan
 */
#include "networking.h"

#define LISTEN_QUEUE_LENGTH 15
#define DEFAULT_PORT 5354

int create_socket(bool server) {
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	int portNumber = DEFAULT_PORT;	//
	
	if(socketfd < 0) {
		fprintf(stderr, "Failed to create Listening Socket; Error Message: %s\n", gai_strerror(errno));
		return -1;
	}

	if(!server) return socketfd;	//not server socket, just return?

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr)); //Clear structure
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(portNumber);

	if(bind(socketfd,(struct sockaddr *)&serverAddr , sizeof(serverAddr)) < 0) {
		fprintf(stderr, "Failed to Bind; Error Message: %s\n", gai_strerror(errno));
		return -2;
	}

	/* Listen on the socket */
	if(listen(socketfd, LISTEN_QUEUE_LENGTH) < 0) {
		fprintf(stderr, "Failed to Listen; Error Message: %s\n", gai_strerror(errno));
		return -3;
	}

	return socketfd;
}

int destroy_socket(int socketfd) {
	if(close(socketfd) < 0) {
		fprintf(stderr, "Failed to Close Socket; Error Message: %s\n", gai_strerror(errno));
		return -1;
	}
	return 0;
}

int write(int socketfd, struct packet &pkt) {

}

int read(int socketfd, struct packet &pkt) {
	
}
