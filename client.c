#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "chess.h"

#define PORT 8080

int main() {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];
    PlayerMove pMove;
    Move move;
    ChessBoard board;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
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
        printf("Enter your move (e.g., a2 a3): ");

        // Read user input
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Failed to read input\n");
            continue;
        }

        // Parse the move from the buffer
        if (sscanf(buffer, "%c%c %c%c", &pMove.from_col, &pMove.from_row, &pMove.to_col, &pMove.to_row) != 4) {
            printf("Invalid input format\n");
            continue;
        }

        // Format the move into the buffer
        snprintf(buffer, sizeof(buffer), "%c%c %c%c", pMove.from_col, pMove.from_row, pMove.to_col, pMove.to_row);

        // Send the move to the server
        send(sockfd, buffer, strlen(buffer), 0);

        // Receive the server response
        memset(buffer, 0, sizeof(buffer));
        recv(sockfd, buffer, sizeof(buffer) - 1, 0);  // Leave space for null terminator
        buffer[sizeof(buffer) - 1] = '\0';  // Ensure null termination
        printf("Server: %s\n", buffer);

        // Check if the move is valid and make the move on the local board
        if (strncmp(buffer, "Valid move", 10) == 0) {
            make_move(&board, convert_to_move(pMove));
        }
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
