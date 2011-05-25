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
	struct sockaddr_in tmp_client;
	struct pollfd recvpoll;
	int client_size;
	int nclients = 0; // number of clients
	char *data;
	int i=0;
	int n;

	if((server_id = init()) <= 0)
	{
		return -1;
	}

	recvpoll.fd = server_id;
	recvpoll.events = POLLIN;
	recvpoll.revents = 0;

	// allocate space
	clients = malloc(sizeof(*clients)*20);
	data = calloc(sizeof(*data),10240);

	client_size = sizeof(clients[0]);

	for(;;)
	{
		i=n=0;
		if(poll(&recvpoll,1,250) > 0)
		{
			if((n = recvfrom(server_id,data,10240,0,(struct sockaddr *)&tmp_client, &client_size)) < 0)
			{
				perror("recvfrom() failed");
			}
		}
		if(n > 0)
		{
			// find out if client exists in client list
			for(i=0; i < nclients; i++)
			{
				if(clients[i].sin_addr.s_addr == tmp_client.sin_addr.s_addr)
				{
					break;
				}
			}
			// a new client just sent us data
			if(i == nclients)
			{
				clients[i].sin_addr = tmp_client.sin_addr;
				clients[i].sin_family = tmp_client.sin_family;
				clients[i].sin_port = tmp_client.sin_port;
				nclients++;
				printf("New client number %d connected\n",i);
			}
			
			for(i=0; i < nclients; i++)
			{
				// dont return to sender
				if(clients[i].sin_addr.s_addr != tmp_client.sin_addr.s_addr)
				{
					printf("%s",data);
					sendto(server_id,data,strlen(data),0,(struct sockaddr *)&clients[i], (socklen_t)client_size);
					//printf("echoing %s\r",data);
				}
			}
			memset(data,0,strlen(data));
		}
	}

	sleep(5);

	// close server socket
	close(server_id);

	return 0;
}
