#ifndef SERVER_H
#define SERVER_H

#include <sys/select.h>
#include <netinet/in.h>
#include <time.h>

typedef struct
{
    int server_fd;
    int *client_fds; // Array of client file descriptors
    int port;
    int max_clients;
    int idle_timeout; // Timeout for idle clients
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    time_t last_activity;
    int active_clients; // Number of active clients
    fd_set readfds;     // File descriptor set for reading, contains the file descriptors to be monitored for readyness for reading
    struct timeval tv;  // Timeout value for select()
} ServerContext;

void init_server_context(ServerContext *ServerContext, int port, int max_clients, int idle_timeout, int tv, int u_tv);
void initialise_server(ServerContext *server_context);
void shutdown_server(ServerContext *server_context);

#endif