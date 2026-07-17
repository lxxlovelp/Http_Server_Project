CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = epoll_server_project
SRCS = main.c network/socket.c epoll_server/epoll_server.c network/http.c Tool/json.c  Tool/send.c 
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)
