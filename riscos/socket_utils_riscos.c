
#include "riscos/socket_utils_riscos.h"
#include <arpa/inet.h> // For htonl()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <swis.h>   // For OS_Byte
#include <kernel.h> // For _kernel_swi_regs and _kernel_swi

void poly_inet_ntop_riscos(int i, uint32_t *ip, char *buf, size_t len) // Convert IP address to string
{
    unsigned char *bytes = (unsigned char *)ip;
    snprintf(buf, len, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
}

int poly_FD_ISSET_riscos(const int file_no, const fd_set *fds)
{
    _kernel_swi_regs regs;
    int flag = 0;
    int start_time, current_time;

    regs.r[0] = 152; // OS_Byte function number for buffer status
    regs.r[1] = 0;   // R1 = 0 for keyboard buffer status
    regs.r[2] = 0;   // R2 = 0

    _kernel_swi_c(OS_Byte, &regs, &regs, &flag);

    _kernel_swi(OS_ReadMonotonicTime, &regs, &regs);
    start_time = regs.r[0];

    // Wait until 100 centiseconds (1 second) pass
    // Without this it causes read() to return too early
    do
    {
        _kernel_swi(OS_ReadMonotonicTime, &regs, &regs);
        current_time = regs.r[0];
    } while (current_time - start_time < 10);

    if (flag == 1) // Buffer empty, return 0
    {
        return 0;
    }
    else
    {
        return 1;
    };
}

int poly_read_riscos(const int file_no, char *buffer, const int buffer_size)
{
    _kernel_swi_regs regs;
    int bytes_read;

    regs.r[0] = (int)buffer; // Pointer to buffer
    regs.r[1] = buffer_size; // Max length (including NULL terminator), returns the number of characters read on exit, excluding the NULL terminator
    regs.r[2] = 0;           // Default (none)

    _kernel_swi(OS_ReadLine, &regs, &regs); // Calls OS_ReadLine
    bytes_read = regs.r[1] + 1;             // Get the number of characters read

    buffer[regs.r[1]] = '\n'; // Ensure \n

    return bytes_read; // Return the length of the input
}

void poly_gen_func_riscos()
{
    // This function is a placeholder for any RISC OS-specific functionality.
    // It can be replaced with actual implementation as needed.
    printf("RISC OS-specific functionality executed.\n");
}