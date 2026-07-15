#ifndef EPOLL_SERVER_H
#define EPOLL_SERVER_H


#define MAX_EVENTS 1024
#define BUFFER_SIZE 4096
int add_fd_to_epoll(int epoll_fd, int listen_fd);
int epoll_wait_loop(int epoll_fd, int listen_fd);

#endif