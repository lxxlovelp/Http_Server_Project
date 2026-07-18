#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "string.h"

void send_404(int client_fd)
{
    char *response =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<h1>404 Not Found</h1>";

    send(client_fd,
         response,
         strlen(response),
         0);
}
