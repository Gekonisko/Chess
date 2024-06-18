#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <pthread.h>
#include "chess.h"

#define PORT 9090

void login_screen(SOCKET sockfd) {
    char choice;
    char username[50];
    char password[50];
    char buffer[1024];

    printf("Do you want to (R)egister or (L)ogin? ");
    scanf(" %c", &choice);
    getchar();  // to consume the newline character left in the buffer

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
    username[strcspn(username, "\n")] = 0;  // remove newline character

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;  // remove newline character

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

void *receive_notifications(void *arg) {
    SOCKET sockfd = *(SOCKET *)arg;
    char buffer[1024];
    int n;

    while ((n = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[n] = '\0';
        printf("Notification: %s\n", buffer);
    }

    return NULL;
}


int main() {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];
    PlayerMove pMove;
    Move move;
    ChessBoard board;
    pthread_t notification_thread;

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

    login_screen(sockfd);

    // Uruchomienie wątku odbierającego powiadomienia
    pthread_create(&notification_thread, NULL, receive_notifications, &sockfd);

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

