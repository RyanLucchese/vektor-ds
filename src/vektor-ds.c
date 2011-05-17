#include "vektor-ds.h"

int init(void)
{
	int server_id; // server's socket
	struct sockaddr_in server_addr;
	int port = 57157;
	int reusesocket = 1;
	

	// create a UDP server socket to get data on
	if((server_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) <= 0)
	{
		perror("socket() failed");
		return -1;
	}
	
	// i'm not sure if this is still useful for UDP?
	if(setsockopt(server_id, SOL_SOCKET, SO_REUSEADDR, &reusesocket, sizeof(reusesocket)) < 0)
	{
		perror("setsockopt() failed setting SO_REUSEADDR");
		return -1;
	}
	
	// server socket address info
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	// bind that shit
	if(bind(server_id, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("bind() failed");
		return -1;
	}

	return server_id;
}

int main(int argc, char ** argv)
{
	int server_id;
	struct sockaddr_in *clients;
	int client_size;
	int nclients = 0; // number of clients
	char *data;
	int i;

	if((server_id = init()) <= 0)
	{
		return -1;
	}

	// allocate space
	clients = malloc(sizeof(*clients)*20);
	data = calloc(sizeof(*data),10240);

	client_size = sizeof(clients[0]);

	for(;;)
	{
		if((recvfrom(server_id,data,10240,0,(struct sockaddr *)&clients[nclients], &client_size)) < 0)
		{
			perror("recvfrom() failed");
		}
		printf("%s\r",data);
	}

	sleep(5);

	// close server socket
	close(server_id);

	return 0;
}
