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

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    uint16_t port = (uint16_t)atoi(argv[1]);
    int listen_fd = init_listen_server(port);
    if (listen_fd == -1) {
        return EXIT_FAILURE;
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        close(listen_fd);
        return EXIT_FAILURE;
    }

    if (add_fd_to_epoll(epoll_fd, listen_fd) == -1) {
        close(listen_fd);
        close(epoll_fd);
        return EXIT_FAILURE;
    }

    struct epoll_event events[MAX_EVENTS];

    while (1) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (n == -1) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;

            if (fd == listen_fd) {
                while (1) {
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(listen_fd,
                                           (struct sockaddr *)&client_addr,
                                           &client_len);
                    if (client_fd == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break;
                        }
                        perror("accept");
                        break;
                    }

                    if (set_nonblock(client_fd) == -1) {
                        close(client_fd);
                        continue;
                    }

                    if (add_fd_to_epoll(epoll_fd, client_fd) == -1) {
                        close(client_fd);
                        continue;
                    }

                    char client_ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
                    printf("accepted connection from %s:%u, fd=%d\n",
                           client_ip, ntohs(client_addr.sin_port), client_fd);
                }
            } else if (events[i].events & EPOLLIN) {
                while (1) {
                    char buf[BUFFER_SIZE];
                    ssize_t count = recv(fd, buf, sizeof(buf), 0);
                    if (count == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break;
                        }
                        perror("recv");
                        close(fd);
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                        break;
                    } else if (count == 0) {
                        printf("client closed fd=%d\n", fd);
                        close(fd);
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                        break;
                    } else {
                        ssize_t written = write(STDOUT_FILENO, buf, count);
                        if (written == -1) {
                            perror("write");
                        }
                    }
                }
            }
        }
    }

    close(epoll_fd);
    close(listen_fd);
    return EXIT_SUCCESS;
}
