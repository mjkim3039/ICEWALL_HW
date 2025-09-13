#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#define SOCKET_PATH "/tmp/mysocket"

int server_fd;

void cleanup(int signo) {
    close(server_fd);
    unlink(SOCKET_PATH);
    exit(0);
}

int main() {
    struct sockaddr_un addr;

    // SIGINT, SIGTERM 시 정리
    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    unlink(SOCKET_PATH);
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Server started, listening on %s\n", SOCKET_PATH);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        char buf[128];
        int n = read(client_fd, buf, sizeof(buf)-1);
        if (n > 0) {
            buf[n] = '\0';
            printf("Received: %s\n", buf);

            if (strcmp(buf, "ping") == 0) {
                write(client_fd, "Minjun", 6);
            } else {
                write(client_fd, "unknown", 7);
            }
        }
        close(client_fd);
    }

    cleanup(0);
    return 0;
}

