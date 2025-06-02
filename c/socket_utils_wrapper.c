
#include "socket_utils_wrapper.h"
#include <stdio.h>
#include <sys/select.h>

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

int poly_FD_ISSET(int i, fd_set *fds) // Function to check for key press in non-blocking mode
{
#ifdef __riscos__
    return poly_FD_ISSET_riscos(i, fds);
#else
    return poly_FD_ISSET_posix(i, fds);
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