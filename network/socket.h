#ifndef SOCKET_H
#define SOCKET_H

#include <stdint.h>
// 创建监听socket
int init_listen_server(uint16_t port);
// 设置fd非阻塞
int set_nonblock(int fd);


#endif  