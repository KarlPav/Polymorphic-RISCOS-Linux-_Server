
#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void init_server_context(ServerContext *ServerContext, int port, int max_clients, int idle_timeout, int tv_sec, int tv_usec)
{
    ServerContext->port = port;
    ServerContext->max_clients = max_clients;                      // Maximum number of clients
    ServerContext->client_fds = malloc(max_clients * sizeof(int)); // Allocate memory for client file descriptors
    if (ServerContext->client_fds == NULL)
    {
        perror("Failed to allocate memory for client_fds\n");
        exit(1);
    }
    ServerContext->idle_timeout = idle_timeout; // Default idle timeout in seconds
    ServerContext->active_clients = 0;
    ServerContext->last_activity = time(NULL);
    ServerContext->client_len = sizeof(ServerContext->client_addr);
    ServerContext->tv.tv_sec = tv_sec;   // Set timeout in seconds
    ServerContext->tv.tv_usec = tv_usec; // Set timeout in microseconds
    for (int i = 0; i < ServerContext->max_clients; i++)
    {
        ServerContext->client_fds[i] = -1; // Initialize all client file descriptors to -1
    }
}

void initialise_server(ServerContext *server_context)
{
    // Create socket - that is, create a file descriptor for the server "server_fd", connections can be found as activity in this file
    server_context->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_context->server_fd < 0)
    {
        perror("socket() failed\n");
        exit(1);
    }

    // Set the behaviour of the socket to allow reuse of the address, opt = 1
    // This is useful when the server is restarted and the port is still in use
    int opt = 1;
    setsockopt(server_context->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configure server
    memset(&server_context->server_addr, 0, sizeof(server_context->server_addr));
    server_context->server_addr.sin_family = AF_INET;                   // IPv4
    server_context->server_addr.sin_addr.s_addr = INADDR_ANY;           // Accept connections from any interface
    server_context->server_addr.sin_port = htons(server_context->port); // Port number (host to network byte order)

    // Bind socket to port - register the details of the server with the file descriptor server_fd, activity on this port should go to this file descriptor
    if (bind(server_context->server_fd, (struct sockaddr *)&server_context->server_addr, sizeof(server_context->server_addr)) < 0)
    {
        perror("bind() failed\n");
        close(server_context->server_fd);
        exit(1);
    }

    printf("Hello World Server\n");

    // Listen for connections - that is, check for activity in the file descriptor "server_fd"
    if (listen(server_context->server_fd, server_context->max_clients) < 0)
    {
        perror("listen() failed\n");
        close(server_context->server_fd);
        exit(1);
    }

    printf("Server listening on port %d\n", server_context->port);
    printf("Press '\\' to shutdown the server\n");

    // initialize the client_fds array to -1
    for (int i = 0; i < server_context->max_clients; i++)
    {
        server_context->client_fds[i] = -1; // Initialize all client file descriptors to -1
    }
}

void shutdown_server(ServerContext *server_context)
{
    // Clean up
    printf("Server shutting down...\n");
    for (int i = 0; i < server_context->max_clients; i++)
    {
        if (server_context->client_fds[i] > 0)
        {
            close(server_context->client_fds[i]);
            server_context->client_fds[i] = -1;
        }
    }

    if (server_context->client_fds != NULL)
    {
        free(server_context->client_fds); // Free the allocated memory for client file descriptors
        server_context->client_fds = NULL;
    }

    close(server_context->server_fd); // Close the server file descriptor
    printf("Server shutdown\n");
}