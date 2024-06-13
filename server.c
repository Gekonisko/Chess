#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
        sscanf(buffer, "%d %d %d %d", &move.from_x, &move.from_y, &move.to_x, &move.to_y);

        if (is_valid_move(&client->board, move)) {
            make_move(&client->board, move);
            client->player_turn = 1 - client->player_turn;
            send(client->sockfd, "Valid move\n", 11, 0);
        } else {
            send(client->sockfd, "Invalid move\n", 13, 0);
        }
    }

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

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Socket bind failed: %d\n", WSAGetLastError());
        exit(1);
    }

    if (listen(sockfd, MAX_CLIENTS) == SOCKET_ERROR) {
        printf("Listen failed: %d\n", WSAGetLastError());
        exit(1);
    }

    while ((newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len)) != INVALID_SOCKET) {
        ClientInfo *client = malloc(sizeof(ClientInfo));
        client->sockfd = newsockfd;
        client->address = client_addr;
        client->addr_len = client_len;
        init_board(&client->board);
        client->player_turn = 0;

        pthread_create(&tid, NULL, handle_client, client);
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
