// Zaktualizowany plik server.c
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <pthread.h>
#include "chess.h"

#define PORT 9090
#define MAX_CLIENTS 10
#define CREDENTIALS_FILE "cred.txt"

typedef struct {
    SOCKET sockfd;
    struct sockaddr_in address;
    int addr_len;
    ChessBoard board;
    int paired;
    char username[50];
    enum FiguresColor color;
    struct ClientInfo *opponent;
    PlayerMove moves[100];  // Array to store moves
    int num_moves;          // Number of moves made
} ClientInfo;

char user_nick[50];
int user_count = 0;

ChessBoard server_board;

void init_server_board() {
    init_board(&server_board);
}

void notify_opponent(ClientInfo *opponent, const char *message) {
    send(opponent->sockfd, message, strlen(message), 0);
}

void *handle_client(void *arg) {
    ClientInfo *client = (ClientInfo *)arg;
    char buffer[1024];
    int n;

    ClientInfo *opponent = client->opponent;  // Assign the opponent

    while ((n = recv(client->sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        PlayerMove pMove;
        memset(&pMove, 0, sizeof(pMove));
        buffer[n] = '\0';  // Null-terminate the received buffer for safe string operations

        // Parsing the move from the buffer
        if (sscanf(buffer, "%c%c %c%c", &pMove.from_col, &pMove.from_row, &pMove.to_col, &pMove.to_row) == 4) {
            Move move = convert_to_move(pMove);

            // Validate the move
            if (is_valid_move(&client->board, move, client->color)) {
                // Store the move in client's move history
                client->moves[client->num_moves++] = pMove;

                // Make the move on the board
                make_move(&client->board, move);

                // Notify opponent about the move made
                if (opponent) {
                    char notify_msg[1024];
                    snprintf(notify_msg, sizeof(notify_msg), "Opponent moved: %c%c %c%c\n", pMove.from_col, pMove.from_row, pMove.to_col, pMove.to_row);
                    send(opponent->sockfd, notify_msg, strlen(notify_msg), 0);
                }

                // Check for check conditions
                if (is_black_king_check(&client->board))
                    notify_opponent(client, "Black King Check\n");
                else if (is_white_king_check(&client->board))
                    notify_opponent(client, "White King Check\n");
                else
                    notify_opponent(client, "Valid move\n");
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

    // Clean up resources
    if (opponent) {
        opponent->opponent = NULL;
        opponent->paired = 0;
        notify_opponent(opponent, "Opponent disconnected\n");
    }

    free(client);
    return NULL;
}


int validate_user(const char *username, const char *password) {
    FILE *file = fopen(CREDENTIALS_FILE, "r");
    if (!file) {
        return 0; // Failed to open file
    }

    char file_username[50], file_password[50];
    while (fscanf(file, "%49s %49s", file_username, file_password) == 2) {
        if (strcmp(file_username, username) == 0 && strcmp(file_password, password) == 0) {
            fclose(file);
            return 1; // Valid user
        }
    }

    fclose(file);
    return 0; // Invalid user
}

int register_user(const char *username, const char *password) {
    FILE *file = fopen(CREDENTIALS_FILE, "a+");
    if (!file) {
        return 0; // Failed to open file
    }

    char file_username[50], file_password[50];
    while (fscanf(file, "%49s %49s", file_username, file_password) == 2) {
        if (strcmp(file_username, username) == 0) {
            fclose(file);
            return 0; // Username already exists
        }
    }

    fprintf(file, "%s %s\n", username, password);
    fclose(file);
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
            strcpy(user_nick, username);
        } else {
            send(sockfd, "Registration failed\n", 20, 0);
            closesocket(sockfd);
            // return;
            pthread_exit(NULL);
        }
    } else if (strcmp(choice, "LOGIN") == 0) {
        if (validate_user(username, password)) {
            send(sockfd, "Login successful\n", 17, 0);
            strcpy(user_nick, username);
        } else {
            send(sockfd, "Login failed\n", 13, 0);
            closesocket(sockfd);
            // return;
            pthread_exit(NULL);
        }
    } else {
        send(sockfd, "Invalid choice\n", 15, 0);
        closesocket(sockfd);
        // return;
        pthread_exit(NULL);
    }
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
ClientInfo *waiting_client = NULL;

void pair_clients(ClientInfo *client1, ClientInfo *client2, int game_id) {
    client1->paired = 1;
    client2->paired = 1;

    // Assign opponents
    client1->opponent = client2;
    client2->opponent = client1;

    // Copy the server's initialized board to both clients
    memcpy(&client1->board, &server_board, sizeof(ChessBoard));
    memcpy(&client2->board, &server_board, sizeof(ChessBoard));

    client1->color = White;
    client2->color = Black;

    printf("Game ID: %d, Player 1 username: %s, Player 2 username: %s\n", game_id, client1->username, client2->username);

    send(client1->sockfd, "Game started. You are White.\n", 30, 0);
    send(client2->sockfd, "Game started. You are Black.\n", 30, 0);

    pthread_create(&(pthread_t){0}, NULL, handle_client, client1);
    pthread_create(&(pthread_t){0}, NULL, handle_client, client2);
}

int main() {
    WSADATA wsaData;
    SOCKET sockfd, newsockfd;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    int i = 0;

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
        printf("Socket listen failed: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        exit(1);
    }

    printf("Server listening on port %d\n", PORT);

    init_server_board();

    while ((newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len)) != INVALID_SOCKET) {
        ClientInfo *client_info = (ClientInfo *)malloc(sizeof(ClientInfo));
        client_info->sockfd = newsockfd;
        client_info->addr_len = client_len;
        client_info->paired = 0;
        client_info->num_moves = 0;

        handle_login_register(newsockfd);

        strcpy(client_info->username, user_nick);

        pthread_mutex_lock(&mutex);
        if (waiting_client == NULL) {
            waiting_client = client_info;
            printf("Waiting for another player to join...\n");
        } else {
            pair_clients(waiting_client, client_info, ++i);
            waiting_client = NULL;
        }
        pthread_mutex_unlock(&mutex);
    }

    if (newsockfd == INVALID_SOCKET) {
        printf("Socket accept failed: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        exit(1);
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
