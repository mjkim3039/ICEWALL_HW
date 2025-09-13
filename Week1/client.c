/*
 * 프로그램 설명:
 *   이 클라이언트는 유닉스 도메인 소켓을 통해
 *   /tmp/mysocket 에 열려 있는 서버에 접속한다.
 *
 * 동작:
 *   1. "ping" 문자열을 서버로 전송한다.
 *   2. 서버가 보내는 응답 메시지를 수신한다.
 *   3. 응답을 표준 출력(stdout)에 출력한다.
 */


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

    // Create socket
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    // Initialize address sturucture
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(1);
    }

    // Send "Ping" to server
    char *msg = "ping";
    write(sockfd, msg, strlen(msg));

    // Read message from server
    char buf[128];
    int n = read(sockfd, buf, sizeof(buf)-1);
    if (n > 0) {
        buf[n] = '\0';
        printf("Response from server: %s\n", buf);
    }

    // End connection
    close(sockfd);
    return 0;
}

