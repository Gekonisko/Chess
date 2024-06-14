#include "chess.h"
#include <stdio.h>
#include <math.h>


void init_board(ChessBoard *board) {
    char firstRow[8] = {'T', 'H', 'B', 'Q', 'K', 'B', 'H', 'T'};

    for (int i = 0; i < BOARD_SIZE; ++i) {
        board->board[0][i] = firstRow[i];
    }

    for (int i = 0; i < BOARD_SIZE; ++i) {
        board->board[1][i] = 'P';
        board->board[2][i] = '.';
        board->board[3][i] = '.';
        board->board[4][i] = '.';
        board->board[5][i] = '.';
        board->board[6][i] = 'p';
    }

    for (int i = 0; i < BOARD_SIZE; ++i) {
        board->board[7][i] = firstRow[i] + 32;
    }
}

Move convert_to_move(PlayerMove playerMove) {
    return (Move){
            .from_col = playerMove.from_col - 'a',
            .from_row = 8 - (playerMove.from_row - '0'),
            .to_col = playerMove.to_col - 'a',
            .to_row = 8 - (playerMove.to_row - '0')
    };
}

int sign(int a, int b)
{
    int r = a - b;
    if(r < 0) return -1;
    if(r > 0) return 1;
    return 0;
}

int can_beat_figure(ChessBoard *board, Move move, char figure) {
    if((figure >= 97 && figure <= 122) &&
    (board->board[move.to_row][move.to_col] >= 65 && board->board[move.to_row][move.to_col] <= 90))
        return 1;
    if((figure >= 65 && figure <= 90) &&
       (board->board[move.to_row][move.to_col] >= 97 && board->board[move.to_row][move.to_col] <= 122))
        return 1;
    return 0;
}

int is_empty(ChessBoard *board, int row, int col) {
    return board->board[row][col] == '.';
}

int is_valid_move(ChessBoard *board, Move move, enum FiguresColor color) {

    char figure = board->board[move.from_row][move.from_col];

    if((figure >= 65 && figure <= 90) && color == White)
        return 0;

    if((figure >= 97 && figure <= 122) && color == Black)
        return 0;

    if(figure == 'P' || figure == 'p')
        return can_pawn_move(board, move, figure);
    if(figure == 'T' || figure == 't')
        return can_tower_move(board, move, figure);
    if(figure == 'H' || figure == 'h')
        return can_horse_move(board, move, figure);
    if(figure == 'B' || figure == 'b')
        return can_bishop_move(board, move, figure);
    if(figure == 'Q' || figure == 'q')
        return can_queen_move(board, move, figure);
    if(figure == 'K' || figure == 'k')
        return can_king_move(board, move, figure);
    return 0;
}

int can_pawn_move(ChessBoard *board, Move move, char figure) {
    if(figure == 'P' && move.from_col == move.to_col && move.from_row + 1 == move.to_row &&
            is_empty(board, move.to_row, move.to_col))
        return 1;
    if(figure == 'P' && (move.from_col + 1 == move.to_col || move.from_col - 1 == move.to_col) &&
            move.from_row + 1 == move.to_row && can_beat_figure(board, move, figure))
        return 1;

    if(figure == 'p' && move.from_col == move.to_col && move.from_row - 1 == move.to_row &&
       is_empty(board, move.to_row, move.to_col))
        return 1;
    if(figure == 'p' && (move.from_col + 1 == move.to_col || move.from_col - 1 == move.to_col) &&
       move.from_row - 1 == move.to_row && can_beat_figure(board, move, figure))
        return 1;

    return 0;
}

int can_tower_move(ChessBoard *board, Move move, char figure)
{
    if(move.from_row == move.to_row || move.from_col == move.to_col)
    {
        for (int i = move.from_row + 1; i < move.to_row; ++i)
            if (is_empty(board, i, move.from_col) == 0)
                return 0;
        for (int i = move.from_row - 1; i > move.to_row; --i)
            if (is_empty(board, i, move.from_col) == 0)
                return 0;
        for (int i = move.from_col + 1; i < move.to_col; ++i)
            if (is_empty(board, move.from_row, i) == 0)
                return 0;
        for (int i = move.from_col - 1; i > move.to_col; --i)
            if (is_empty(board, move.from_row, i) == 0)
                return 0;

        if(can_beat_figure(board, move, figure) || is_empty(board, move.to_row, move.to_col))
            return 1;
    }

    return 0;
}

int can_horse_move(ChessBoard *board, Move move, char figure)
{
    if(abs(move.from_row - move.to_row) == 2 && abs(move.from_col - move.to_col) == 1)
        return 1;
    if(abs(move.from_row - move.to_row) == 1 && abs(move.from_col - move.to_col) == 2)
        return 1;

    return 0;
}

int can_bishop_move(ChessBoard *board, Move move, char figure)
{
    if(abs(move.from_row - move.to_row) == abs(move.from_col - move.to_col))
    {
        int colSign = sign(move.to_col, move.from_col);
        int rowSign = sign(move.to_row, move.from_row);
        int distance = abs(move.from_row - move.to_row);

        for (int i = 1; i < distance; ++i)
            if (is_empty(board, move.from_row + (rowSign * i), move.from_col + (colSign * i)) == 0)
                return 0;

        if(can_beat_figure(board, move, figure) || is_empty(board, move.to_row, move.to_col))
            return 1;
    }

    return 0;
}

int can_queen_move(ChessBoard *board, Move move, char figure)
{
    if(can_tower_move(board, move, figure))
        return 1;
    if(can_bishop_move(board, move, figure))
        return 1;

    return 0;
}

int can_king_move(ChessBoard *board, Move move, char figure)
{
    if(abs(move.from_row - move.to_row) <= 1 && abs(move.from_col - move.to_col) <= 1)
    {
        if(is_position_under_attack(board, move, figure) == 0 &&
                (can_beat_figure(board, move, figure) || is_empty(board, move.to_row, move.to_col)))
            return 1;
    }

    return 0;
}

int is_position_under_attack(ChessBoard *board, Move move, char figure)
{
    return 0;
}

void make_move(ChessBoard *board, Move move) {
    board->board[move.to_row][move.to_col] = board->board[move.from_row][move.from_col];
    board->board[move.from_row][move.from_col] = '.';

    if(board->board[move.to_row][move.to_col] == 'p' && move.to_row == 0)
        board->board[move.to_row][move.to_col] = 'q';

    if(board->board[move.to_row][move.to_col] == 'P' && move.to_row == 7)
        board->board[move.to_row][move.to_col] = 'Q';
}

void print_board(ChessBoard *board) {
    printf("   ");
    for (int i = 0; i < BOARD_SIZE; ++i) {
        printf("%c ", i +'a');
    }
    printf("\n");

    printf("  ");
    for (int i = 0; i < BOARD_SIZE; ++i) {
        printf("%c", '-');
        printf("%c", '-');
    }
    printf("--\n");

    for (int i = 0; i < BOARD_SIZE; ++i) {
        printf("%c |", '8' - i);
        for (int j = 0; j < BOARD_SIZE; ++j) {
            printf("%c ", board->board[i][j]);
        }
        printf("|\n");
    }

    printf("  ");
    for (int i = 0; i < BOARD_SIZE; ++i) {
        printf("%c", '-');
        printf("%c", '-');
    }
    printf("--\n");
}
