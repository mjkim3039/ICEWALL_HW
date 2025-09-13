#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/mysocket"

int main() {
    int sockfd;
    struct sockaddr_un addr;

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(1);
    }

    char *msg = "ping";
    write(sockfd, msg, strlen(msg));

    char buf[128];
    int n = read(sockfd, buf, sizeof(buf)-1);
    if (n > 0) {
        buf[n] = '\0';
        printf("Response from server: %s\n", buf);
    }

    close(sockfd);
    return 0;
}

