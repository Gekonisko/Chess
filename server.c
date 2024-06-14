#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <pthread.h>
#include "chess.h"

#define PORT 8080
#define MAX_CLIENTS 10

typedef struct {
    SOCKET sockfd;
    struct sockaddr_in address;
    int addr_len;
    ChessBoard board;
    int player_turn;
} ClientInfo;

void *handle_client(void *arg) {
    ClientInfo *client = (ClientInfo *)arg;
    char buffer[1024];
    int n;

    while ((n = recv(client->sockfd, buffer, sizeof(buffer), 0)) > 0) {
        Move move;
        memset(&move, 0, sizeof(move));
        buffer[n] = '\0';  // Null-terminate the received buffer for safe string operations

        sscanf(buffer, "%c%c %c%c", &move.from_col, &move.from_row, &move.to_col, &move.to_row);

        if (is_valid_move(&client->board, move)) {
            make_move(&client->board, move);
            client->player_turn = 1 - client->player_turn;
            send(client->sockfd, "Valid move\n", 11, 0);
        } else {
            send(client->sockfd, "Invalid move\n", 13, 0);
        }
    }

    printf("Client disconnected\n");
    closesocket(client->sockfd);
    free(client);
    return NULL;
}

int main() {
    WSADATA wsaData;
    SOCKET sockfd, newsockfd;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    pthread_t tid;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Socket bind failed: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        exit(1);
    }

    if (listen(sockfd, MAX_CLIENTS) == SOCKET_ERROR) {
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        exit(1);
    }

    printf("Server listening on port %d\n", PORT);

    while ((newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len)) != INVALID_SOCKET) {
        ClientInfo *client = malloc(sizeof(ClientInfo));
        if (client == NULL) {
            printf("Failed to allocate memory for client\n");
            closesocket(newsockfd);
            continue;
        }

        client->sockfd = newsockfd;
        client->address = client_addr;
        client->addr_len = client_len;
        init_board(&client->board);
        print_board(&client->board);
        client->player_turn = 0;

        if (pthread_create(&tid, NULL, handle_client, client) != 0) {
            printf("Failed to create thread\n");
            closesocket(newsockfd);
            free(client);
        } else {
            pthread_detach(tid); // Detach thread to handle its own cleanup
        }
    }

    printf("Server shutting down\n");
    closesocket(sockfd);
    WSACleanup();
    return 0;
}
