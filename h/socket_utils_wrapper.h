
#ifndef SOCKET_UTILS_WRAPPER_H
#define SOCKET_UTILS_WRAPPER_H

#include <stddef.h> // For size_t
#include <stdint.h> // For uint32_t
#include <sys/select.h> // For fd_set

// Public declaration only
void poly_inet_ntop(int i, uint32_t *ip, char *buf, size_t len); // Convert IP address to string
int poly_FD_ISSET(int i, fd_set *fds); // Function to check for key press in non-blocking mode
void poly_gen_func();

#endif