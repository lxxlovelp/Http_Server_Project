#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "../network/socket.h"
#include "epoll_server.h"





static int add_fd_to_epoll(int epoll_fd, int fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;//设置为边缘触发模式
    ev.data.fd = fd;
    
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {//将文件描述符添加到epoll实例中
        perror("epoll_ctl ADD");
        return -1;
    }
    return 0;
}
