
#ifndef PIECE_H_
#define PIECE_H_

#include "player.h"

enum Piece {
    EMPTY_PIECE = ' ',
    WHITE_PIECE = 'O',
    BLACK_PIECE = '@',
    FOURTH_BLACK_PIECE = '#',
};

inline bool is_white_piece (Piece piece) { return piece == WHITE_PIECE;}
inline bool is_black_piece (Piece piece) { return piece == BLACK_PIECE || piece == FOURTH_BLACK_PIECE;}
inline bool is_non_fourth_black_piece (Piece piece) { return piece == BLACK_PIECE;}
inline bool is_fourth_black_piece (Piece piece) { return piece == FOURTH_BLACK_PIECE;}


#endif
