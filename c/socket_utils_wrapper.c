
#include <unistd.h>

#include "socket_utils_wrapper.h"

// Include the correct platform header
#ifdef __riscos__
#include "riscos/socket_utils_riscos.h"
#else
#include "posix/socket_utils_posix.h"
#endif

// Single implementation that delegates to platform-specific function
void poly_inet_ntop(int i, uint32_t *ip, char *buf, size_t len) // Convert IP address to string
{
#ifdef __riscos__
    return poly_inet_ntop_riscos(i, ip, buf, len);
#else
    return poly_inet_ntop_posix(i, ip, buf, len);
#endif
}

// Single implementation that delegates to platform-specific function
int poly_FD_ISSET(const int file_no, const fd_set *fds)
{
#ifdef __riscos__

    if (file_no == STDIN_FILENO) // Special case for STDIN_FILENO
    {
        return poly_FD_ISSET_riscos(file_no, fds);
    }
    else
    {
        return FD_ISSET(file_no, fds); // Use standard FD_ISSET for other file descriptors}
    }

#else
    return poly_FD_ISSET_posix(file_no, fds);
#endif
}

// Single implementation that delegates to platform-specific function
int poly_read(const int file_no, char *buffer, const int buffer_size)
{
#ifdef __riscos__
    return poly_read_riscos(file_no, buffer, buffer_size);
#else
    return poly_read_posix(file_no, buffer, buffer_size);
#endif
}

// General function implementation that delegates to platform-specific function
void poly_gen_func()
{
#ifdef __riscos__
    return poly_gen_func_riscos();
#else
    return poly_gen_func_posix();
#endif
}