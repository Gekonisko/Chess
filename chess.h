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
} PlayerMove;

typedef struct {
    int from_col;
    int from_row;
    int to_col;
    int to_row;
} Move;

typedef struct {
    int col;
    int row;
} Position;

enum FiguresColor{ White, Black, All };

void init_board(ChessBoard *board);
int is_valid_move(ChessBoard *board, Move move, enum FiguresColor color);
void make_move(ChessBoard *board, Move move);
void print_board(ChessBoard *board);
Move convert_to_move(PlayerMove playerMove);

int can_beat_figure(ChessBoard *board, Move move, char figure);
int is_empty(ChessBoard *board, int row, int col);

int sign(int a, int b);

int can_pawn_move(ChessBoard *board, Move move, char figure);
int can_tower_move(ChessBoard *board, Move move, char figure);
int can_horse_move(ChessBoard *board, Move move, char figure);
int can_bishop_move(ChessBoard *board, Move move, char figure);
int can_queen_move(ChessBoard *board, Move move, char figure);
int can_king_move(ChessBoard *board, Move move, char figure);

int is_position_under_attack(ChessBoard *board, Move move, char figure);

#endif // CHESS_H
