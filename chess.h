#ifndef CHESS_H
#define CHESS_H

#define BOARD_SIZE 8

typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
} ChessBoard;

typedef struct {
    int from_x, from_y;
    int to_x, to_y;
} Move;

void init_board(ChessBoard *board);
int is_valid_move(ChessBoard *board, Move move);
void make_move(ChessBoard *board, Move move);
void print_board(ChessBoard *board);

#endif // CHESS_H
