#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "http.h"




int recv_request(int client_fd, char *buffer, size_t buffer_size){
	
	ssize_t ret = recv(client_fd, buffer, buffer_size, 0);//接收客户端请求报文
	//ret 返回值为接收到的字节数，如果返回值为0，表示客户端关闭了连接，如果返回值为-1，表示接收数据失败
	if (ret < 0) {
		perror("recv");
		return -1;
	}
	return ret;
}

int extract_post_data(char *buffer, char *post_data, size_t post_data_size){

	char *post_data_start = strstr(buffer, "\r\n\r\n");//查找请求报文中POST数据的起始位置
	if (post_data_start == NULL) {
		fprintf(stderr, "No POST data found in the request.\n");
		return -1;
	}
	post_data_start += 4; // Skip the "\r\n\r\n" part

	strncpy(post_data, post_data_start, post_data_size - 1);//将POST数据复制到post_data缓冲区中
	post_data[post_data_size - 1] = '\0'; //确保字符串以空字符结尾

	return 0;

	
}
