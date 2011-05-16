#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_CLIENTS 10
#define BUF_SIZE 1024
#define max(x,y) ((x) > (y) ? (x) : (y))

char buffer[MAX_CLIENTS][BUF_SIZE];

int client_index;
int ssockfd;
int csockfd[MAX_CLIENTS];
fd_set rd, wr, er;
struct sockaddr_in server_addr;
struct sockaddr_in client_addr[MAX_CLIENTS];

int init(void);
int socket_isready(int sockfd);
void new_connection(void);
void end_connection(int *sockfd);
int rw_data(int client_id);
