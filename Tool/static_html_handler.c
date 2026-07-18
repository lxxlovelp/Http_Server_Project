#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h> 
#include <sys/socket.h>
#include"no_find.h"
#include "static_html_handler.h"



void handle_static(int client_fd,const char *path)
{
    char filepath[256];

    // 【新增第1步】防黑客
    if (strstr(path, "..") != NULL) {
        send_404(client_fd);
        return;
    }

    /*
        path: /static/test.html
        去掉 /static/ 得到: test.html
    */
    sprintf(filepath, "/home/xingxinliao/Project/Resource/%s", path + strlen("/static/"));
    printf("open file: %s\n", filepath);

    int fd = open(filepath, O_RDONLY);
    
    if (fd < 0) {
        send_404(client_fd);
        printf("open-----------------");
        return;
    }

    // 先发送HTTP头
    char header[512];
    sprintf(header,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
    );
    send(client_fd, header, strlen(header), 0);

    // 发送文件内容
    char buffer[1024];  // 顺手把缓存调大了一点点
    int n;
    while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
        send(client_fd, buffer, n, 0);
    }

    close(fd);
}