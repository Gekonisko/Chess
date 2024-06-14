#include "chess.h"
#include <stdio.h>

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

int is_valid_move(ChessBoard *board, Move move) {
    int from_col = move.from_col - 'a';
    int from_row = 8 - (move.from_row - '0');

    int to_col = move.to_col - 'a';
    int to_row = 8 - (move.to_row - '0');

    printf("(%c%c) - (%c%c) = (%d %d) - (%d %d)\n",move.from_col,move.from_row,move.to_col,move.to_row, from_row, from_col, to_row, to_col);


    char figure = board->board[from_row][from_col];

    if(figure == 'P' || figure == 'p')
        return is_valid_pawn_move(board, move, figure);
//    if(figure == 'T' || figure == 't')
//        return is_valid_tower_move(board, move, figure);
//    if(figure == 'H' || figure == 'h')
//        return is_valid_horse_move(board, move, figure);
//    if(figure == 'B' || figure == 'b')
//        return is_valid_bishop_move(board, move, figure);
//    if(figure == 'Q' || figure == 'q')
//        return is_valid_queen_move(board, move, figure);
//    if(figure == 'K' || figure == 'k')
//        return is_valid_king_move(board, move, figure);
    return 0;
}

int is_valid_pawn_move(ChessBoard *board, Move move, char figure)
{
    int from_col = move.from_col - 'a';
    int from_row = 8 - (move.from_row - '0');

    int to_col = move.to_col - 'a';
    int to_row = 8 - (move.to_row - '0');

    printf("(%d %d) - (%d %d)", from_row, from_col, to_row, to_col);


    if(figure == 'P' && from_col == to_col && from_row - 1 == to_row &&
            (board->board[to_row][to_col] == '.' || (board->board[to_row][to_col] >= 97 && board->board[to_row][to_col] <= 122)))
        return 1;
    if(figure == 'p' && from_col == to_col && from_row + 1 == to_row &&
       (board->board[to_row][to_col] == '.' || (board->board[to_row][to_col] >= 65 && board->board[to_row][to_col] <= 90)))
        return 1;
    return 0;
}

void make_move(ChessBoard *board, Move move) {
    int from_col = move.from_col - 'a';
    int from_row = 8 - (move.from_row - '0');

    int to_col = move.to_col - 'a';
    int to_row = 8 - (move.to_row - '0');

    board->board[to_row][to_col] = board->board[from_row][from_col];
    board->board[from_row][from_col] = '.';
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
