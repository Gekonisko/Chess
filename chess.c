#include "chess.h"
#include <stdio.h>

void init_board(ChessBoard *board) {
    // Ustawienie początkowej pozycji na szachownicy
    // W celu uproszczenia tylko pionki

    board->board[0][0] = 'T';
    board->board[0][1] = 'H';
    board->board[0][2] = 'J';
    board->board[0][3] = 'Q';
    board->board[0][4] = 'K';
    board->board[0][5] = 'J';
    board->board[0][6] = 'H';
    board->board[0][7] = 'T';

    for (int i = 1; i < BOARD_SIZE - 1; ++i) {
        board->board[1][i] = 'P';
        board->board[2][i] = '.';
        board->board[3][i] = '.';
        board->board[4][i] = '.';
        board->board[5][i] = '.';
        board->board[6][i] = 'p';
    }

    board->board[7][0] = 't';
    board->board[7][1] = 'h';
    board->board[7][2] = 'j';
    board->board[7][3] = 'q';
    board->board[7][4] = 'k';
    board->board[7][5] = 'j';
    board->board[7][6] = 'h';
    board->board[7][7] = 't';
}

int is_valid_move(ChessBoard *board, Move move) {
    // Prosta walidacja ruchu pionków
    if (move.from_y == move.to_y && (move.to_x == move.from_x + 1 || move.to_x == move.from_x - 1)) {
        return 1;
    }
    return 0;
}

void make_move(ChessBoard *board, Move move) {
    // Wykonanie ruchu
    board->board[move.to_x][move.to_y] = board->board[move.from_x][move.from_y];
    board->board[move.from_x][move.from_y] = ' ';
}

void print_board(ChessBoard *board) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            printf("%c ", board->board[i][j]);
        }
        printf("\n");
    }
}
