#ifndef CHESS_H
#define CHESS_H

#define BOARD_SIZE 8

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
} ChessBoard;

typedef struct {
    int col;
    int row;
} Position;

typedef struct {
    char from_row;
    char from_col;
    char to_row;
    char to_col;
} PlayerMove;

typedef struct {
    Position from;
    Position to;
} Move;



enum FiguresColor{ White, Black, All };
enum Figures{ Pawn, Tower, Horse, Bishop, Queen, King };

void init_board(ChessBoard *board);
int is_valid_move(ChessBoard *board, Move move, enum FiguresColor color);
void make_move(ChessBoard *board, Move move);
void print_board(ChessBoard *board);
Move convert_to_move(PlayerMove playerMove);

int can_beat_figure(char myFigure, char targetFigure);
int can_be_beaten_by(char myFigure, char targetFigure, enum Figures figure);
int is_empty(ChessBoard *board, int row, int col);

int is_figure(char figureToCheck, enum Figures figureKind);

int sign(int a, int b);

int can_pawn_move(ChessBoard *board, Move move, char figure);
int can_tower_move(ChessBoard *board, Move move, char figure);
int can_horse_move(ChessBoard *board, Move move, char figure);
int can_bishop_move(ChessBoard *board, Move move, char figure);
int can_queen_move(ChessBoard *board, Move move, char figure);
int can_king_move(ChessBoard *board, Move move, char figure);

int is_position_under_attack(ChessBoard *board, Position move, char figure);

#endif // CHESS_H
