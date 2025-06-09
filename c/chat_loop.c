
#include "server.h"
#include "chat_loop.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "socket_utils_wrapper.h"

void chat_loop(ServerContext *server_context, int buffer_size)
{
    while (1)
    {
        fd_set readfds;                              // File descriptor set for reading, contains the file descriptors to be monitored for readyness for reading
        FD_ZERO(&readfds);                           // Clear the file descriptor set
        FD_SET(server_context->server_fd, &readfds); // Add server_fd to the set
        FD_SET(STDIN_FILENO, &readfds);              // Add standard input (stdin) to the set
        int max_fd = server_context->server_fd;      // Maximum file descriptor value, used in select()
        char buffer[buffer_size];                    // Buffer to store incoming data and storing outgoing data

        for (int i = 0; i < server_context->max_clients; i++) // Add all active clients to readfds
        {
            if (server_context->client_fds[i] > 0)
            {
                FD_SET(server_context->client_fds[i], &readfds);
                if (server_context->client_fds[i] > max_fd)
                    max_fd = server_context->client_fds[i];
            }
        }

        // Now when using readfs, we are checking for activity in both the server_fd and stdin only
        // Select() monitors the file descriptors in readfds for activity
        // If there is activity, it returns the number of file descriptors that are ready for reading
        // If there is no activity, it returns 0
        // If an error occurs, it returns -1
        // If the timeout expires, it returns 0
        int activity = select(max_fd + 1, &readfds, NULL, NULL, &server_context->tv);
        if (activity < 0)
        {
            perror("select error\n");
            continue;
        }

        // FD_ISSET checks if the file descriptor is set in the readfds set, checks if server_fd is set in readfds
        if (poly_FD_ISSET(server_context->server_fd, &readfds))
        {
            // Accept a new client connection
            // Loop through the client_fds array to find an empty slot for the new client
            // If an empty slot is found, accept the connection and store the client file descriptor in that slot
            // If no empty slot is found, reject the connection
            for (int i = 0; i < server_context->max_clients; i++)
            {
                if (server_context->client_fds[i] == -1) // Find an empty slot in the client_fds array
                {
                    server_context->client_fds[i] = accept(server_context->server_fd, (struct sockaddr *)&server_context->client_addr, &server_context->client_len);
                    if (server_context->client_fds[i] < 0)
                    {
                        perror("accept() failed\n");
                        continue; // Skip to next iteration instead of exiting
                    }
                    server_context->active_clients++; // Increment the number of active clients
                    char client_ip[INET_ADDRSTRLEN];
                    poly_inet_ntop(AF_INET, &server_context->client_addr.sin_addr.s_addr, client_ip, INET_ADDRSTRLEN); // Convert IP address to string
                    printf("New client connected from: %s\nTotal clients: %d\n", client_ip, server_context->active_clients);
                    break; // Exit loop after accepting a new client
                }
            }
            // If no empty slot is found, reject the connection
            if (server_context->active_clients == server_context->max_clients)
            {
                printf("Max clients reached. Connection refused.\n");
                close(server_context->client_fds[server_context->max_clients - 1]); // Close the last client
                server_context->client_fds[server_context->max_clients - 1] = -1;   // Reset the last client slot
            }
        }

        for (int i = 0; i < server_context->max_clients; i++)
        {
            if (server_context->client_fds[i] != -1 && FD_ISSET(server_context->client_fds[i], &readfds))
            {
                int bytes_received = recv(server_context->client_fds[i], buffer, sizeof(buffer), 0);
                buffer[bytes_received] = '\0'; // Null-terminate the buffer

                switch (bytes_received)
                {
                case -1:
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        // No data available (non-blocking mode) ? client is still connected
                        printf("Client %d: No data (EAGAIN)\n", server_context->client_fds[i]);
                    }
                    else
                    {
                        // Real error (e.g., ECONNRESET, ENOTCONN)
                        perror("recv failed\n");
                        close(server_context->client_fds[i]);
                        server_context->client_fds[i] = -1;
                        server_context->active_clients--;
                        if (server_context->active_clients == 0)
                        {
                            server_context->last_activity = time(NULL); // Reset last activity time
                        }
                    }
                    break;
                case 0:
                {
                    printf("Client %d disconnected.\n", server_context->client_fds[i]);
                    close(server_context->client_fds[i]);
                    server_context->client_fds[i] = -1; // Mark slot as free
                    server_context->active_clients--;
                    break;
                }
                default:
                    printf("Client said: %s", buffer);
                    send(server_context->client_fds[i], "Hello from server\n", 18, 0);
                    continue;
                }
            }
        }

        if (poly_FD_ISSET(STDIN_FILENO, &readfds))
        {
            poly_read(STDIN_FILENO, buffer, sizeof(buffer));

            if (buffer[0] == '\\' && buffer[1] == '\n') // Check for shutdown command
            {
                printf("Shutdown requested.\n");
                break;
            }
        }

        // Idle timeout check
        if (server_context->active_clients == 0 && (time(NULL) - server_context->last_activity) > server_context->idle_timeout)
        {
            // maybe something that clears the buffer?
            printf("No clients connected for %d seconds. Shutdown? (y/n): ", server_context->idle_timeout);
            char response;
            scanf("%c", &response);
            if (response == 'y')
                break;
            server_context->last_activity = time(NULL); // Reset timer
        }
    }
}