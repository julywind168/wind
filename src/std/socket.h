#ifndef WIND_STD_SOCKET_H
#define WIND_STD_SOCKET_H

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h> 

extern "C" DLLEXPORT {
    int c_write(int fd, char *buf, ssize_t start, ssize_t n);
    int c_read(int fd, char *buf, size_t start, size_t n);
    int bytes_ready(int fd);
    int do_socket(void);
    int do_bind(int s, int port);
    int do_listen(int s);
    int do_accept(int s);
    int do_connect(int s);
    char* get_error(void);
}


#endif
