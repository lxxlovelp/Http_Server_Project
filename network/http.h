#ifndef HTTP_H
#define HTTP_H
#include <stdio.h>
#include <stdlib.h>
extern char buffer[1024];
extern char post_data[512];
int recv_request(int client_fd, char *buffer, size_t buffer_size);
int parse_http_request(char *buffer);
int extract_post_data(char *buffer, char *post_data, size_t post_data_size);
#endif