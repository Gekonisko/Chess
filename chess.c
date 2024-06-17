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
            .from.col = playerMove.from_col - 'a',
            .from.row = 8 - (playerMove.from_row - '0'),
            .to.col = playerMove.to_col - 'a',
            .to.row = 8 - (playerMove.to_row - '0')
    };
}

int sign(int a, int b)
{
    int r = a - b;
    if(r < 0) return -1;
    if(r > 0) return 1;
    return 0;
}

int can_beat_figure(char myFigure, char targetFigure) {
    if((myFigure >= 97 && myFigure <= 122) &&
        (targetFigure >= 65 && targetFigure <= 90))
        return 1;
    if((myFigure >= 65 && myFigure <= 90) &&
        (targetFigure >= 97 && targetFigure <= 122))
        return 1;
    return 0;
}

int is_empty(ChessBoard *board, int row, int col) {
    return board->board[row][col] == '.';
}

int is_valid_move(ChessBoard *board, Move move, enum FiguresColor color) {

    char figure = board->board[move.from.row][move.from.col];

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
    if(figure == 'P' && move.from.col == move.to.col && move.from.row + 1 == move.to.row &&
            is_empty(board, move.to.row, move.to.col))
        return 1;
    if(figure == 'P' && move.from.col == move.to.col && move.from.row == 1 && move.from.row + 2 == move.to.row &&
       is_empty(board, move.to.row, move.to.col))
        return 1;
    if(figure == 'P' && (move.from.col + 1 == move.to.col || move.from.col - 1 == move.to.col) &&
            move.from.row + 1 == move.to.row && can_beat_figure(figure, board->board[move.to.row][move.to.col]))
        return 1;

    if(figure == 'p' && move.from.col == move.to.col && move.from.row - 1 == move.to.row &&
       is_empty(board, move.to.row, move.to.col))
        return 1;
    if(figure == 'p' && move.from.col == move.to.col && move.from.row == 6 && move.from.row - 2 == move.to.row &&
       is_empty(board, move.to.row, move.to.col))
        return 1;
    if(figure == 'p' && (move.from.col + 1 == move.to.col || move.from.col - 1 == move.to.col) &&
       move.from.row - 1 == move.to.row && can_beat_figure(figure, board->board[move.to.row][move.to.col]))
        return 1;

    return 0;
}

int can_tower_move(ChessBoard *board, Move move, char figure)
{
    if(move.from.row == move.to.row || move.from.col == move.to.col)
    {
        for (int i = move.from.row + 1; i < move.to.row; ++i)
            if (is_empty(board, i, move.from.col) == 0)
                return 0;
        for (int i = move.from.row - 1; i > move.to.row; --i)
            if (is_empty(board, i, move.from.col) == 0)
                return 0;
        for (int i = move.from.col + 1; i < move.to.col; ++i)
            if (is_empty(board, move.from.row, i) == 0)
                return 0;
        for (int i = move.from.col - 1; i > move.to.col; --i)
            if (is_empty(board, move.from.row, i) == 0)
                return 0;

        if(can_beat_figure(figure, board->board[move.to.row][move.to.col]) || is_empty(board, move.to.row, move.to.col))
            return 1;
    }

    return 0;
}

int can_horse_move(ChessBoard *board, Move move, char figure)
{
    if(abs(move.from.row - move.to.row) == 2 && abs(move.from.col - move.to.col) == 1)
        return 1;
    if(abs(move.from.row - move.to.row) == 1 && abs(move.from.col - move.to.col) == 2)
        return 1;

    return 0;
}

int can_bishop_move(ChessBoard *board, Move move, char figure)
{
    if(abs(move.from.row - move.to.row) == abs(move.from.col - move.to.col))
    {
        int colSign = sign(move.to.col, move.from.col);
        int rowSign = sign(move.to.row, move.from.row);
        int distance = abs(move.from.row - move.to.row);

        for (int i = 1; i < distance; ++i)
            if (is_empty(board, move.from.row + (rowSign * i), move.from.col + (colSign * i)) == 0)
                return 0;

        if(can_beat_figure(figure, board->board[move.to.row][move.to.col]) || is_empty(board, move.to.row, move.to.col))
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
    if(abs(move.from.row - move.to.row) <= 1 && abs(move.from.col - move.to.col) <= 1)
    {
        if(is_position_under_attack(board, move.to, figure) == 0 &&
                (can_beat_figure(figure, board->board[move.to.row][move.to.col]) || is_empty(board, move.to.row, move.to.col)))
            return 1;
    }

    return 0;
}

int is_figure(char figureToCheck, enum Figures figureKind)
{
    if (figureKind == Pawn && (figureToCheck == 'p' || figureToCheck == 'P'))
        return 1;
    if (figureKind == Tower && (figureToCheck == 't' || figureToCheck == 'T'))
        return 1;
    if (figureKind == Horse && (figureToCheck == 'h' || figureToCheck == 'H'))
        return 1;
    if (figureKind == Bishop && (figureToCheck == 'b' || figureToCheck == 'B'))
        return 1;
    if (figureKind == Queen && (figureToCheck == 'q' || figureToCheck == 'Q'))
        return 1;
    if (figureKind == King && (figureToCheck == 'k' || figureToCheck == 'K'))
        return 1;
    return 0;
}

int can_be_beaten_by(char myFigure, char targetFigure, enum Figures figure)
{
    return can_beat_figure(myFigure, targetFigure) && is_figure(targetFigure, figure);
}

int is_position_under_attack(ChessBoard *board, Position pos, char figure)
{
    // Pawns
    if(board->board[pos.row - 1][pos.col + 1] == 'P' || board->board[pos.row - 1][pos.col - 1] == 'P')
        return 1;
    if(board->board[pos.row + 1][pos.col + 1] == 'p' || board->board[pos.row + 1][pos.col - 1] == 'p')
        return 1;

    // Towers
    for (int i = pos.row + 1; i < BOARD_SIZE; ++i){
        if (is_empty(board, i, pos.col) == 0){
            if(can_be_beaten_by(figure, board->board[i][pos.col], Tower) ||
                can_be_beaten_by(figure, board->board[i][pos.col], Queen))
                return 1;
            else break;
        }
    }
    for (int i = pos.row - 1; i >= 0; --i){
        if (is_empty(board, i, pos.col) == 0){
            if(can_be_beaten_by(figure, board->board[i][pos.col], Tower) ||
                can_be_beaten_by(figure, board->board[i][pos.col], Queen))
                return 1;
            else break;
        }
    }
    for (int i = pos.col + 1; i < BOARD_SIZE; ++i) {
        if (is_empty(board, pos.row, i) == 0){
            if(can_be_beaten_by(figure, board->board[pos.row][i], Tower) ||
                can_be_beaten_by(figure, board->board[pos.row][i], Queen))
                return 1;
            else break;
        }
    }
    for (int i = pos.col - 1; i >= 0; --i) {
        if (is_empty(board, pos.row, i) == 0){
            if(can_be_beaten_by(figure, board->board[pos.row][i], Tower) ||
             can_be_beaten_by(figure, board->board[pos.row][i], Queen))
                return 1;
            else break;
        }
    }

    // Bishop
    int topRightDistance = MIN(BOARD_SIZE - pos.col - 1, pos.row);
    int topLeftDistance = MIN(pos.col, pos.row);
    int bottomRightDistance = MIN(BOARD_SIZE - pos.col - 1, BOARD_SIZE - pos.row - 1);
    int bottomLeftDistance = MIN(pos.col, BOARD_SIZE - pos.row - 1);
    for (int i = 1; i <= topRightDistance; ++i) {
        if (is_empty(board, pos.row - i, pos.col + i) == 0) {
            if (can_be_beaten_by(figure, board->board[pos.row - i][pos.col + i], Bishop) ||
                can_be_beaten_by(figure, board->board[pos.row - i][pos.col + i], Queen))
                return 1;
            else break;
        }
    }
    for (int i = 1; i <= topLeftDistance; ++i) {
        if (is_empty(board, pos.row - i, pos.col - i) == 0) {
            if (can_be_beaten_by(figure, board->board[pos.row - i][pos.col - i], Bishop) ||
                can_be_beaten_by(figure, board->board[pos.row - i][pos.col - i], Queen))
                return 1;
            else break;
        }
    }
    for (int i = 1; i <= bottomRightDistance; ++i) {
        if (is_empty(board, pos.row + i, pos.col + i) == 0) {
            if (can_be_beaten_by(figure, board->board[pos.row + i][pos.col + i], Bishop) ||
                can_be_beaten_by(figure, board->board[pos.row + i][pos.col + i], Queen))
                return 1;
            else break;
        }
    }
    for (int i = 1; i <= bottomLeftDistance; ++i) {
        if (is_empty(board, pos.row + i, pos.col - i) == 0) {
            if (can_be_beaten_by(figure, board->board[pos.row + i][pos.col - i], Bishop) ||
                can_be_beaten_by(figure, board->board[pos.row + i][pos.col - i], Queen))
                return 1;
            else break;
        }
    }

    // King
    for(int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y < 1; ++y) {
            if (x == 0 && y == 0) continue;

            if (is_empty(board, pos.row + x, pos.col + y) == 0 &&
                can_be_beaten_by(figure, board->board[pos.row + x][pos.col + y], King))
                return 1;
        }
    }

    return 0;
}

void make_move(ChessBoard *board, Move move) {
    board->board[move.to.row][move.to.col] = board->board[move.from.row][move.from.col];
    board->board[move.from.row][move.from.col] = '.';

    if(board->board[move.to.row][move.to.col] == 'p' && move.to.row == 0)
        board->board[move.to.row][move.to.col] = 'q';

    if(board->board[move.to.row][move.to.col] == 'P' && move.to.row == 7)
        board->board[move.to.row][move.to.col] = 'Q';
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
