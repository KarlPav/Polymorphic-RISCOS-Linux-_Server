
/*"Everything is a file" in Unix or for us UnixLib.
A network connection effectively establishes a "file" which receives the input and
our server checks for activity in this "file" and responds accordingly.
Further, the activity of the server (receiving connections for example) itself is kept in a "file"*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <swis.h>
#include <sys/socket.h> //Main socket header
#include <netinet/in.h> //sockaddr_in
#include <arpa/inet.h>  //inet_ntoa
#include <unistd.h>     //close

#define PORT 12345
#define BUFFER_SIZE 256

/* Function prototypes */
void report_error(const char *context);
void format_ip_as_str(uint32_t ip, char *buf, size_t len); // Reformats IP address to a string, this is a missing POSIX function (inet_ntop/inet_ntoa)

int main(int argc, char *argv[])
{
    int listen_sock, client_sock = 0;            // Listening socket and client socket/file descriptor
    struct sockaddr_in server_addr, client_addr; // Server and client address structures
    socklen_t client_len;                        // Length of client address
    char buffer[BUFFER_SIZE];                    // Buffer for sending data
    int bytes_sent;                              // Number of bytes sent
    int bytes_received;                          // Number of bytes received

    printf("RISC OS TCP Hello World Server\n");

    /* Setting up to listen on the port */

    /* Create socket - that is, create a file descriptor for the server "listen_sock", connections can be found as activity in this file */
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0)
    {
        report_error("socket creation");
    }

    /* Set up server address */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;         // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    server_addr.sin_port = htons(PORT);       // Port number

    /* Bind socket to port - register the details of the server with the file descriptor listen_sock, activity on this port should go to this file descriptor */
    if (bind(listen_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        report_error("bind");
    }

    /* Listen for connections - that is, check for activity in the file descriptor "listen_sock" */
    if (listen(listen_sock, 5) < 0)
    {
        report_error("listen");
    }

    printf("Server listening on port %d\n", PORT);
    printf("Press Escape to quit\n");

    /* Main loop */
    /* Wait for incoming connections */
    /* When a connection is accepted send a message and then disconnect */
    /* Keep the server running */
    while (1)
    {
        /* Check for Escape key */
        if (_kernel_osbyte(129, 0, 0) == 27)
        {
            printf("Escape pressed, shutting down...\n");
            break;
        }

        /* Accept incoming connection */
        client_len = sizeof(client_addr);
        /* Accept a connection - when a new connection is added to the server file descriptor, "listen_sock"
        fill the client address details into a sockaddr struct* and associate this with a client file descriptor, "client_sock" */
        client_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &client_len);

        /* If accept() fails, it returns -1 and sets errno */
        if (client_sock < 0)
        {
            report_error("accept() failed");
            continue;
        }

        // printf("Connection accepted from %s\n", inet_ntoa(client_addr.sin_addr));
        char str_client_addr[16];
        format_ip_as_str(client_addr.sin_addr.s_addr, str_client_addr, sizeof(str_client_addr));
        printf("Connection accepted from %s\n", str_client_addr);

        /* Send greeting */
        strcpy(buffer, "Hello World from RISC OS!\r\n");
        /* Copies the bytes in the buffer into client_sock which is the file that represents the client, returns the number of bytes sent */
        bytes_sent = send(client_sock, buffer, strlen(buffer), 0);
        if (bytes_sent < 0)
        {
            report_error("send() failed");
        }
        else
        {
            printf("Sent %d bytes\n", bytes_sent);
        }

        /* Wait for a message from the client */
        /* Receive data from Client */
        /* Copy the bytes recieved in client_sock, the file representing the client into buffer, returns the number of bytes received */
        bytes_received = recv(client_sock, buffer, sizeof(buffer), 0);
        if (bytes_received > 0)
        {
            buffer[bytes_received] = '\0';
            printf("Received: %s\n", buffer);
        }

        /*Send reponse*/
        const char *response = "Reply from RISC OS!\n"; // strncpy(buffer, "Hello from RISC OS!\n"));
        if (send(client_sock, response, strlen(response), 0) < 0)
        {
            perror("send() failed");
            break;
        }

        /* Close client connection */
        close(client_sock);
    }

    /* Clean up */
    printf("Server shutting down...\n");
    /* Close the files representing the server and client, these are the files referred to by the file descriptors client_sock and listen_sock */
    close(client_sock);
    close(listen_sock);
    printf("Server shutdown\n");

    return 0;
}

/* Error reporting function */
void report_error(const char *context)
{
    _kernel_oserror *error = _kernel_last_oserror();
    printf("Error in %s: %s (%d)\n", context, error->errmess, error->errnum);
    exit(1);
}

/* format the IP address to a string */
/* This is a missing POSIX function (inet_ntop/inet_ntoa) */
void format_ip_as_str(uint32_t ip, char *buf, size_t len)
{
    unsigned char *bytes = (unsigned char *)&ip;
    snprintf(buf, len, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
}