CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = epoll_server
SRC = epoll_server.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
