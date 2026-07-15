#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "network/socket.h"
#include "epoll_server/epoll_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "network/socket.h"
#include "epoll_server/epoll_server.h"

#define SERVER_PORT 12345
int main(int argc, char *argv[])
{
    uint16_t port = SERVER_PORT;//默认端口号
    if (argc > 1) port = (uint16_t)atoi(argv[1]);//如果命令行参数中指定了端口号，则使用指定的端口号

    int listen_fd = init_listen_server(port);
    if (listen_fd < 0) return 1;

    if (set_nonblock(listen_fd) == -1) {
        close(listen_fd);
        return 1;
    }

    int epoll_fd = epoll_create1(0);//创建一个epoll实例，返回epoll文件描述符
    if (epoll_fd == -1) {
        perror("epoll_create1");
        close(listen_fd);
        return 1;
    }

    if (add_fd_to_epoll(epoll_fd, listen_fd) == -1) {
        close(epoll_fd);
        close(listen_fd);
        return 1;
    }

    /* 进入 epoll 主循环（函数在 epoll_server.c 中实现） */
    epoll_wait_loop(epoll_fd, listen_fd);

    close(epoll_fd);
    close(listen_fd);
    return 0;
}
