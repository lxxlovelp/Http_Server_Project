
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "http.h"
#include "../Tool/json.h"
#include"../Tool/send.h"
#include<errno.h>
#include <unistd.h>
#include"../Tool/static_html_handler.h"
#include"../Tool/no_find.h"
#include"../Tool/process_create.h"


int recv_request(int client_fd, char *buffer, size_t buffer_size){
	
	ssize_t ret = recv(client_fd, buffer, buffer_size, 0);//接收客户端请求报文
	//ret 返回值为接收到的字节数，如果返回值为0，表示客户端关闭了连接，如果返回值为-1，表示接收数据失败
	if (ret < 0) {//如果接收数据失败，打印错误信息并返回-1
        if(errno!=EAGAIN&&errno!=EWOULDBLOCK){

            perror("recv");
        }
		return -1;
	}
	return ret;
}
 

// HTTP路由表
void http_route(const char *method, const char *path, const char *body,int fd)
{

    // POST /upload
    if(strcmp(method, "POST") == 0 &&
       strcmp(path, "/upload") == 0)
    {
        
        int ret=parse_json_data(body);
        if(ret==0){
            Send_Data_Quene();//发送到队列存储
        }
        else{
            perror("parse fail");
        }
        return;
    }


    // GET /
    if(strcmp(method, "GET") == 0 &&
       strncmp(path, "/static/", 8) == 0)
    {
        handle_static(fd, path);
        printf("-----------------------------静态资源请求-------------------------------\n");
        return;
    }



     if(strcmp(method, "GET") == 0 &&
       strcmp(path, "/sensor/sensor.cgi") == 0)
    {
        process_create();
          //子进程执行
             if (process_pid == 0) {
                printf("child process running...\n");
                int ret = execl("/home/xingxinliao/Project/CGI/test", "test", fd);	//execl之后后面函数不执行了
                if (ret==-1) {
                perror("execl");
                exit(5);
                }

             }
        printf("动态资源请求成功\n");
        return;
    }


    // 404
    printf("404 Not Found: %s %s\n",
           method,
           path);
}



//处理http请求
int handle_http_request(const char *request,int fd) {
    // 复制一份，因为 strtok 会修改字符串
    char *request_copy = strdup(request);
    if (!request_copy) {
        perror("strdup");
        return -1;
    }

    // 找到第一行结束位置（以 \r\n 或 \n 结尾）
    char *line_end = strstr(request_copy, "\r\n");
    if (!line_end) {
        line_end = strchr(request_copy, '\n');
    }

    if (!line_end) {
        // 没有换行，可能只有一行
        line_end = request_copy + strlen(request_copy);
    } else {
        // 截断第一行
        *line_end = '\0';
    }

    // 现在 request_copy 只包含请求行
    char *method = strtok(request_copy, " ");// 按空行分隔
    char *path = strtok(NULL, " ");

    // 第三个 token 是 HTTP 版本，我们不需要

    if (method == NULL || path == NULL) {
        fprintf(stderr, "Invalid request line\n");
        free(request_copy);
        return -1;
    }
    // 如果请求有主体（body），需要从原始请求中提取
    // 简单的做法：找到空行（\r\n\r\n 或 \n\n）后的内容
    const char *body = NULL;
    char *body_start = strstr(request, "\r\n\r\n");
    if (!body_start) {
        body_start = strstr(request, "\n\n");
    }
    if (body_start) {
        body = body_start + (body_start[0] == '\r' ? 4 : 2); // 跳过空行
    } else {
        body = ""; // 无主体
    }

    // 调用现有的路由函数
    http_route(method, path, body,fd);

    free(request_copy);
    return 0;
}