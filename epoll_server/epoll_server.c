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



struct epoll_event events[MAX_EVENTS];  //事件类型 + 之前保存的data




 int add_fd_to_epoll(int epoll_fd, int listen_fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;//设置为边缘触发模式+监听可读事件
    ev.data.fd = listen_fd;// 保存文件描述符，事件发生时返回
    
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev) == -1) {//将文件描述符添加到epoll实例中
        perror("epoll_ctl ADD");
        return -1;
    }
    return 0;
}



int epoll_wait_loop(int epoll_fd, int listen_fd)
{
    for (;;) {//无限循环等待事件发生
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);//等待事件发生，返回发生事件的数量
        if (n == -1) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            return -1;
        }

        for (int i = 0; i < n; ++i) {//遍历所有发生的事件
            int fd = events[i].data.fd;
            uint32_t ev = events[i].events;

            if (fd == listen_fd) {
                /* 有新连接，边缘触发需循环 accept 直到 EAGAIN */
                while (1) {
                    struct sockaddr_in cli_addr;
                    socklen_t cli_len = sizeof(cli_addr);
                    int client_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_len);//接受新连接，返回客户端套接字描述符
                    if (client_fd == -1) {//如果accept返回-1，说明没有新的连接或者发生错误
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        perror("accept");
                        break;
                    }

                    if (set_nonblock(client_fd) == -1) {
                        perror("set_nonblocking");
                        close(client_fd);
                        continue;
                    }

                    if (add_fd_to_epoll(epoll_fd, client_fd) == -1) {
                        close(client_fd);
                        continue;
                    }
                }
            } else {
                /* 客户端事件：处理可读，处理错误/关闭 */
                if (ev & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    continue;
                }

                if (ev & EPOLLIN) {//如果事件是可读事件，读取数据并回显
                    char buf[4096];
                    while (1) {
                        ssize_t cnt = recv(fd, buf, sizeof(buf), 0);//读取数据
                        if (cnt > 0) {
                         
                            write(fd, buf, cnt);//回显数据
                        } else if (cnt == 0) {
                            /* 对端关闭 */
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                            close(fd);
                            break;
                        } else {
                            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                                break;
                            }
                            perror("read");
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                            close(fd);
                            break;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

