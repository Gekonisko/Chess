#ifndef CHESS_H
#define CHESS_H

#define BOARD_SIZE 8

typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
} ChessBoard;

typedef struct {
    char from_col;
    char from_row;
    char to_col;
    char to_row;
} Move;

void init_board(ChessBoard *board);
int is_valid_move(ChessBoard *board, Move move);
void make_move(ChessBoard *board, Move move);
void print_board(ChessBoard *board);

int is_valid_pawn_move(ChessBoard *board, Move move, char sign);
int is_valid_tower_move(ChessBoard *board, Move move, char sign);
int is_valid_horse_move(ChessBoard *board, Move move, char sign);
int is_valid_bishop_move(ChessBoard *board, Move move, char sign);
int is_valid_queen_move(ChessBoard *board, Move move, char sign);
int is_valid_king_move(ChessBoard *board, Move move, char sign);

#endif // CHESS_H
