
#ifndef SOCKET_UTILS_POSIX_H
#define SOCKET_UTILS_POSIX_H

#include <stddef.h>     // For size_t
#include <stdint.h>     // For uint32_t
#include <sys/select.h> // For fd_set

void poly_inet_ntop_posix(int i, uint32_t *ip, char *buf, size_t len); // Convert IP address to string
int poly_FD_ISSET_posix(const int file_no, const fd_set *fds);         // Function to check for key press in non-blocking mode
int poly_read_posix(const int file_no, char *buffer, const int buffer_size);
void poly_gen_func_posix();

#endif