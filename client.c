#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <pthread.h>
#include "chess.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9090

typedef struct {
    SOCKET sockfd;
    ChessBoard board;
    enum FiguresColor color;
    PlayerMove lastMove;
    int canMove;
} ClientInfo;

ClientInfo client_info;

void login_screen(SOCKET sockfd) {
    char choice;
    char username[50];
    char password[50];
    char buffer[1024];

    printf("Do you want to (R)egister or (L)ogin? ");
    scanf(" %c", &choice);
    getchar();

    if (choice == 'R' || choice == 'r') {
        strcpy(buffer, "REGISTER");
    } else if (choice == 'L' || choice == 'l') {
        strcpy(buffer, "LOGIN");
    } else {
        printf("Invalid choice\n");
        exit(1);
    }

    send(sockfd, buffer, strlen(buffer), 0);

    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    snprintf(buffer, sizeof(buffer), "%s:%s", username, password);
    send(sockfd, buffer, strlen(buffer), 0);

    memset(buffer, 0, sizeof(buffer));
    recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    printf("Server: %s\n", buffer);

    if (strncmp(buffer, "Registration failed\n", 16) == 0 || strncmp(buffer, "Login failed\n", 16) == 0 ) {
        printf("Registration/Login failed. Exiting...\n");
        exit(1);
    }
}

void update_board(ClientInfo *client, PlayerMove pMove) {
    Move move = convert_to_move(pMove);
    make_move(&client->board, move);
}

void *receive_messages(void *arg) {
    SOCKET sockfd = *(SOCKET *)arg;
    char buffer[1024];
    int n;

    while ((n = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[n] = '\0';
        printf("Received: %s", buffer);

        if (strstr(buffer, "Game started. You are White.") != NULL) {
            client_info.canMove = 1;
        }

        if (strstr(buffer, "Game started. You are Black.") != NULL) {
            print_board(&client_info.board);
            printf("Waiting for enemy move ... \n");
        }

        if (strstr(buffer, "Valid move") != NULL) {
            update_board(&client_info, client_info.lastMove);
            print_board(&client_info.board);
            printf("Waiting for enemy move ... \n");
        }

        if (strstr(buffer, "Invalid move") != NULL) {
            client_info.canMove = 1;
        }

        if (strstr(buffer, "Opponent moved:") != NULL) {
            PlayerMove pMove;
            memset(&pMove, 0, sizeof(pMove));
            if (sscanf(buffer, "Opponent moved: %c%c %c%c\n", &pMove.from_col, &pMove.from_row, &pMove.to_col, &pMove.to_row) == 4) {
                update_board(&client_info, pMove);
                client_info.canMove = 1;
            }
        }

        memset(buffer, 0, sizeof(buffer));
    }

    return NULL;
}

void send_move(SOCKET sockfd, char from_col, char from_row, char to_col, char to_row) {
    char message[1024];
    snprintf(message, sizeof(message), "%c%c %c%c\n", from_col, from_row, to_col, to_row);
    send(sockfd, message, strlen(message), 0);
}

int main() {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in server_addr;

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
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Socket connect failed: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        exit(1);
    }

    login_screen(sockfd);

    client_info.sockfd = sockfd;
    client_info.canMove = 0;
    init_board(&client_info.board);

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, &sockfd);

    printf("Each player controls sixteen pieces of six types on a chessboard. "
           "Each type of piece moves in a distinct way. The object of the game is to checkmate the opponent's king, "
           "checkmate occurs when a king is threatened with capture and has no escape. "
           "A game can end in various ways besides checkmate: a player can resign, "
           "and there are several ways a game can end in a draw.\n\n"
           "White pieces:\n"
           "- p = Pawn\n"
           "- t = Rock(Tower)\n"
           "- h = Knight(Horse)\n"
           "- b = Bishop\n"
           "- q = Queen\n"
           "- k = King\n\n"
           "Black pieces:\n"
           "- P = Pawn\n"
           "- T = Rock(Tower)\n"
           "- H = Knight(Horse)\n"
           "- B = Bishop\n"
           "- Q = Queen\n"
           "- K = King\n\n");


    printf("Waiting for other player ... \n");


    while (1) {
        if(client_info.canMove == 0) continue;

        print_board(&client_info.board);
        printf("Enter move (e.g., e2 e4): ");
        if (scanf(" %c%c %c%c", &client_info.lastMove.from_col, &client_info.lastMove.from_row,
                  &client_info.lastMove.to_col, &client_info.lastMove.to_row) != 4) {
            printf("Invalid input format. Try again.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        send_move(sockfd, client_info.lastMove.from_col, client_info.lastMove.from_row,
                  client_info.lastMove.to_col, client_info.lastMove.to_row);
        client_info.canMove = 0;
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
