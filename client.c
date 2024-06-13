#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include "chess.h"

#define PORT 8080

int main() {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];
    Move move;
    ChessBoard board;

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection to server failed: %d\n", WSAGetLastError());
        exit(1);
    }

    init_board(&board);
    while (1) {
        print_board(&board);
        printf("Enter your move (from_x from_y to_x to_y): ");
        scanf("%d %d %d %d", &move.from_x, &move.from_y, &move.to_x, &move.to_y);

        sprintf(buffer, "%d %d %d %d", move.from_x, move.from_y, move.to_x, move.to_y);
        send(sockfd, buffer, strlen(buffer), 0);

        recv(sockfd, buffer, sizeof(buffer), 0);
        printf("Server: %s", buffer);

        if (strncmp(buffer, "Valid move", 10) == 0) {
            make_move(&board, move);
        }
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
