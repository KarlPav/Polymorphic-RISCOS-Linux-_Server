
/*"Everything is a file" in Unix
A network connection effectively establishes a "file" which receives the input and
our server checks for activity in this "file" and responds accordingly.
Further, the activity of the server (receiving connections for example) itself is kept in a "file"*/

#include "socket_utils_wrapper.h"
#include "server.h"
#include "chat_loop.h"

#define PORT 12345       // Must match RISC OS client's port
#define IDLE_TIMEOUT 30  // 30 seconds
#define MAX_CLIENTS 5    // Maximum number of clients
#define BUFFER_SIZE 1024 // Size of the buffer for incoming data
#define TV_SEC 0         // Timeout in seconds for select()
#define TV_USEC 100000   // Timeout in microseconds for select()

int main()
{
    ServerContext server_context;

    init_server_context(&server_context, PORT, MAX_CLIENTS, IDLE_TIMEOUT, TV_SEC, TV_USEC); // Initialize server context
    initialise_server(&server_context);                                                     // Set up the server

    chat_loop(&server_context, BUFFER_SIZE); // Start the chat loop

    shutdown_server(&server_context); // Clean up and shutdown the server

    return 0;
}