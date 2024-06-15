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

typedef struct {
    char username[50];
    char password[50];
} User;

User users[MAX_CLIENTS];
int user_count = 0;

void *handle_client(void *arg) {
    ClientInfo *client = (ClientInfo *)arg;
    char buffer[1024];
    int n;

    while ((n = recv(client->sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        PlayerMove pMove;
        memset(&pMove, 0, sizeof(pMove));
        buffer[n] = '\0';  // Null-terminate the received buffer for safe string operations

        // Parsing the move from the buffer
        if (sscanf(buffer, "%c%c %c%c", &pMove.from_col, &pMove.from_row, &pMove.to_col, &pMove.to_row) == 4) {

            Move move = convert_to_move(pMove);

            printf("\n(%c%c) - (%c%c) = (%d %d) - (%d %d)\n",pMove.from_col,pMove.from_row,pMove.to_col,pMove.to_row, move.from_col,move.from_row,move.to_col,move.to_row);

            if (is_valid_move(&client->board, move, All)) {
                make_move(&client->board, move);
                client->player_turn = 1 - client->player_turn;
                send(client->sockfd, "Valid move\n", 11, 0);
            } else {
                send(client->sockfd, "Invalid move\n", 13, 0);
            }
        } else {
            send(client->sockfd, "Invalid input format\n", 21, 0);
        }
        memset(buffer, 0, sizeof(buffer));
    }

    printf("Client disconnected\n");
    closesocket(client->sockfd);
    free(client);
    return NULL;
}

int validate_user(const char *username, const char *password) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            return 1; // Valid user
        }
    }
    return 0; // Invalid user
}

int register_user(const char *username, const char *password) {
    if (user_count >= MAX_CLIENTS) {
        return 0; // Registration failed, user limit reached
    }
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return 0; // Username already exists
        }
    }
    strcpy(users[user_count].username, username);
    strcpy(users[user_count].password, password);
    user_count++;
    return 1; // Registration successful
}

void handle_login_register(SOCKET sockfd) {
    char buffer[1024];
    char choice[1024];
    char username[50];
    char password[50];
    int n;

    // Receive choice from client
    n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        return;
    }
    buffer[n] = '\0';

    strcpy(choice, buffer);

    // printf("\n%.*s",1024,choice);

    // Receive credentials from client
    n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        return;
    }
    buffer[n] = '\0';

    sscanf(buffer, "%[^:]:%s", username, password);

    if (strcmp(choice, "REGISTER") == 0) {
        if (register_user(username, password)) {
            send(sockfd, "Registration successful\n", 24, 0);
        } else {
            send(sockfd, "Registration failed\n", 20, 0);
            closesocket(sockfd);
            pthread_exit(NULL);
        }
    } else if (strcmp(choice, "LOGIN") == 0) {
        if (validate_user(username, password)) {
            send(sockfd, "Login successful\n", 17, 0);
        } else {
            send(sockfd, "Login failed\n", 13, 0);
            closesocket(sockfd);
            pthread_exit(NULL);
        }
    } else {
        send(sockfd, "Invalid choice\n", 15, 0);
        closesocket(sockfd);
        pthread_exit(NULL);
    }
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

        printf("Client connected\n");

        // login or register handle
        handle_login_register(newsockfd);

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
