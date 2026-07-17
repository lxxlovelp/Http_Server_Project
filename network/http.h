#ifndef HTTP_H
#define HTTP_H
#include <stdio.h>
#include <stdlib.h>
int recv_request(int client_fd, char *buffer, size_t buffer_size);
void http_route(const char *method, const char *path, const char *body);
int handle_http_request(const char *request);
#endif