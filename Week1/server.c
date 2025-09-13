/*
 * 프로그램 설명:
 *   이 서버는 유닉스 도메인 소켓을 통해
 *   /tmp/mysocket 경로에서 클라이언트의 접속을 기다린다.
 *
 * 동작:
 *   1. /tmp/mysocket 에 소켓을 생성하고 클라이언트의 연결 요청을 수락한다.
 *   2. 클라이언트로부터 문자열을 수신한다.
 *   3. 수신한 문자열이 "ping"이면 "Minjun"을, 그렇지 않으면 "unknown"을 클라이언트에 전송한다.
 *   4. SIGINT 또는 SIGTERM 시 서버 소켓을 닫고 소켓 파일을 삭제한 뒤 종료한다.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#define SOCKET_PATH "/tmp/mysocket"

int server_fd;

/*
* cleanup function
* - SIGINT, SIGTERM 시 호출
* - Close server socket, delete socket file, and terminate program
*/
void cleanup(int signo) {
    close(server_fd);
    unlink(SOCKET_PATH);
    exit(0);
}

int main() {
    struct sockaddr_un addr;

    // Clean up if SIGINT or SIGTERM
    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);

    // Create server socket
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    // Initialize socket address structure, set the path
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    // Remove any existing socket file, then bind the socket
    unlink(SOCKET_PATH);
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    // Wait for client access
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Server started, listening on %s\n", SOCKET_PATH);

    // Handle client's request
    while (1) {
        // Accept client connection request
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        // Receive message from client
        char buf[128];
        int n = read(client_fd, buf, sizeof(buf)-1);
        if (n > 0) {
            buf[n] = '\0';
            printf("Received: %s\n", buf);

            // If "ping" is received, send name to client
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

