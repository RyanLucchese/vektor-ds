#include "vektor-ds.h"

int init(void)
{
	int port, i, ssock_opts, reusesocket = 1;
	
	memset(&server_addr,'\0',sizeof(server_addr));
	for(i=0; i <= MAX_CLIENTS; i++)
	{
		memset(&buffer[i],'\0',sizeof(buffer[i]));
		memset(&client_addr[i],'\0',sizeof(client_addr[i]));
	}

	port = 57157;
	client_index = 0;

	if((ssockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "Error creating socket!");
		return -1;
	}
	
	if(setsockopt(ssockfd, SOL_SOCKET, SO_REUSEADDR, &reusesocket, sizeof(int)) == -1)
	{
		fprintf(stderr, "Error setting SO_REUSEADDR socket option!");
		return -1;
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	if(bind(ssockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		fprintf(stderr, "Error binding to socket!\n");
		return -1;
	}

	listen(ssockfd,5);

	ssock_opts = fcntl(ssockfd, F_GETFL, 0);
	fcntl(ssockfd, F_SETFL, ssock_opts | O_NONBLOCK);
	return 0;
}

void new_connection(void)
{
	int clength, s;

	clength = sizeof(client_addr[client_index]);
	if((s = accept(ssockfd, (struct sockaddr *)&client_addr[client_index], &clength)) > 0)
	{
		csockfd[client_index] = s;
		printf("Got connection from %s!\n",inet_ntoa(client_addr[client_index].sin_addr));
		client_index++;
	}
	else
	{
		perror("accept()");
	}
}

void end_connection(int *socket)
{
	if(socket >= 0)
	{
		shutdown(*socket, SHUT_RDWR);
		close(*socket);
		*socket=-1;
	}
}

int main(int argc, char ** argv)
{
	int i,j;
	int buff_avail[MAX_CLIENTS];
	int buff_written[MAX_CLIENTS];

	for(i=0; i <= MAX_CLIENTS; i++)
	{
		csockfd[i] = -1;
		buff_avail[i] = 0;
		buff_written[i] = 0;
	}

	init();

	for(;;)
	{
		int r, nfds = 0;

		FD_ZERO(&rd);
		FD_ZERO(&wr);
		FD_ZERO(&er);
		FD_SET(ssockfd,&rd);
		nfds = max(nfds, ssockfd);

		for(i=0; i<= client_index; i++)
		{
			if(csockfd[i] > 0 && buff_avail[i] < BUF_SIZE)
			{
				FD_SET(csockfd[i],&rd);
				nfds = max(nfds, csockfd[i]);
			}
			for(j=0; j<= client_index; j++)
			{
				if(csockfd[i] > 0 && (buff_avail[j] - buff_written[j]) > 0)
				{
					FD_SET(csockfd[i],&wr);
					nfds = max(nfds, csockfd[i]);
				}
			}
		}

		r=select(nfds+1,&rd,&wr,&er,NULL);

		for(i=0; i <= client_index; i++)
		{
			if(csockfd[i]>0 && FD_ISSET(csockfd[i], &rd))
			{
				memset(buffer[i],0,sizeof(buffer[i]));
				r=read(csockfd[i],buffer[i],BUF_SIZE-buff_avail[i]);
				if(r>0)
				{
					buff_avail[i]+=r;
					printf("From Client %d: %s",i,buffer[i]);
				}
				if(r<0)
				{
					FD_CLR(csockfd[i],&rd);
					FD_CLR(csockfd[i],&wr);
				//}
				//if(r<0)
				//{
					end_connection(&csockfd[i]);
				}
			}
			
			if(csockfd[i]>0 && FD_ISSET(csockfd[i], &wr))
			{
				for(j=0; j <= client_index; j++)
				{
					if(i!=j && strlen(buffer[j])>0)
					{
						r=write(csockfd[i],buffer[j],buff_avail[j]-buff_written[j]);
						if(r>0)
						{
							buff_written[j]+=r;
							printf("To Client %d: %s",i,buffer[j]);
						}
						if(r<0)
						{
							FD_CLR(csockfd[i],&rd);
							FD_CLR(csockfd[i],&wr);
						//}
						//if(r<0)
						//{
							end_connection(&csockfd[i]);
						}
					}
				}
			}
			if(buff_written[i]==buff_avail[i])
			{
				memset(buffer[i],0,sizeof(buffer[i]));
				buff_written[i] = buff_avail[i] = 0;
			}
			if(FD_ISSET(ssockfd, &rd))
			{
				new_connection();
			}
		}
	}

	sleep(5);

	close(ssockfd);
	for(i=0; i <= client_index; i++)
	{
		close(csockfd[i]);
	}

	return 0;
}
