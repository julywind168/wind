#include "socket.h"

/* c_write attempts to write the entire buffer, pushing through
   interrupts, socket delays, and partial-buffer writes */
int c_write(int fd, char *buf, ssize_t start, ssize_t n) {
    ssize_t i, m;

    buf += start;
    m = n;
    while (m > 0) {
        if ((i = write(fd, buf, m)) < 0) {
            if (errno != EAGAIN && errno != EINTR)
                return i;
        } else {
            m -= i;
            buf += i;
        }
    }
    return n;
}

/* c_read pushes through interrupts and socket delays */
int c_read(int fd, char *buf, size_t start, size_t n) {
    int i;

    buf += start;
    for (;;) {
        i = read(fd, buf, n);
        if (i >= 0) return i;
        if (errno != EAGAIN && errno != EINTR) return -1;
    }
}

/* bytes_ready(fd) returns true if there are bytes available
   to be read from the socket identified by fd */
int bytes_ready(int fd) {
    int n;

    (void) ioctl(fd, FIONREAD, &n);
    return n;
}

/* socket support */

/* do_socket() creates a new AF_INET socket */
int do_socket(void) {
    return socket(AF_INET, SOCK_STREAM, 0);
}

/* do_bind(s, name) binds name to the socket s */
int do_bind(int s, int port) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons(port);
    return bind(s, (struct sockaddr*)(&address), sizeof(address));
}

int do_listen(int s) {
    return listen(s, 5);
}

/* do_accept accepts a connection on socket s */
int do_accept(int s) {
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    return accept(s, (struct sockaddr*)(&address), (socklen_t*)&addrlen);
}

/* do_connect initiates a socket connection */
int do_connect(int s) {
    return -1;
}

/* get_error returns the operating system's error status */
char* get_error(void) {
    extern int errno;
    return strerror(errno);
}

