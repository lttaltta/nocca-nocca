
#ifndef BOARD_H_
#define BOARD_H_

#include <cstdio>
#include <vector>
#include <random>
#include "piece.h"
#include "player.h"

extern std::mt19937 mt;

class Cell {
public:
    Cell (): step() {};
    std::vector<Piece> step;

    // セルの一番上のピースを取り除けるかどうか判定
    bool  is_pop_piece_to_cell  () const { return step.size() > 0; }
    // セルにピースを置けるかどうか判定
    bool  is_push_piece_to_cell (Piece piece) const { return step.size() < 3 || piece == FOURTH_BLACK_PIECE; }

    // セルの一番上のピースを取得。存在しない場合は EMPTY_PIECE を返す
    Piece get_top_piece () const { return step.size() ? step[step.size()-1] : EMPTY_PIECE; }

    // セルにピースを置く
    bool push_piece_to_cell (Piece piece)    {
        if ( is_push_piece_to_cell(piece) ) {
            step.push_back(piece);
            return true;
        }
        else {
            return false;
        }
    }

    // セルからピースを取り除く
    bool pop_piece_to_cell () {
        if ( ! step.empty() ) {
            step.pop_back();
            return true;
        }
        else {
            return false;
        }
    }

};

class Board {
public:

    Board (int height, int width): board_height(height), board_width(width), board_elements(height*width) { };

    int get_board_height () const { return board_height;}
    int get_board_width ()  const { return board_width;}

    Cell& board_reference (int y, int x) { return board_elements[y*board_width + x]; }

    void print_board () {

        for(int i = 0; i < board_height; i++ ) {

            for(int j = 0; j < board_width; j++ ) printf("+----"); printf("\n");
            for(int j = 0; j < board_width; j++ ) {
                const std::vector<Piece>& step = board_elements[i*board_width + j].step;
                printf("| %c%c ", (step.size()>=2?step[1]:' '), (step.size()>=4?step[3]:' '));
            }
            printf("|\n");
            for(int j = 0; j < board_width; j++ ) {
                const std::vector<Piece>& step = board_elements[i*board_width + j].step;
                printf("| %c%c ", (step.size()>=1?step[0]:' '), (step.size()>=3?step[2]:' '));
            }
            printf("|\n");
        }
        for(int j = 0; j < board_width; j++ ) printf("+----"); printf("\n");

    }

    // 盤面の評価値
    // 白+ --- -黒
    int board_evaluation () {
        int eval = 0;

        // 駒の位置が自陣に近いほど悪い。相手陣にあるほどよい
        for( int i = 0; i < board_height; i++ ) {
            for( int j = 0; j < board_width; j++ ) {
                const std::vector<Piece>& step = board_elements[i*board_width + j].step;

                // 端のピースは悪い
                if ( (j == 0 || j == board_width-1) && step.size() >= 1 ) eval += 400 * (is_white_piece(step[0])? -1 : 1);
                if ( (j == 0 || j == board_width-1) && step.size() >= 2 ) eval += 400 * (is_white_piece(step[1])? -1 : 1);
                if ( (j == 0 || j == board_width-1) && step.size() >= 3 ) eval += 400 * (is_white_piece(step[2])? -1 : 1);

                if ( i == 0 || i == board_height-1 ) eval += (400 - 160*i) * step.size();
                if ( i == 1 || i == board_height-2 ) eval += (360 - 144*i) * step.size();
                if ( i == 2 || i == board_height-3 ) eval += (300 - 120*i) * step.size();

                // 相手人の特定箇所に駒があると良い
//                if ( i == 1              && 1 <= j && j <= board_width-1 && step.size() >= 1 && is_white_piece(step[0]) ) eval += 300;
//                if ( i == board_height-2 && 1 <= j && j <= board_width-1 && step.size() >= 1 && is_black_piece(step[0]) ) eval -= 300;

                // eval += (500 - 200*i) * step.size();
            }
        }

        // 駒の上に乗ってるとき
        for( int i = 0; i < board_height; i++ ) {
            for( int j = 0; j < board_width; j++ ) {
                const std::vector<Piece>& step = board_elements[i*board_width + j].step;
                if ( step.size() >= 2 ) {
                    if ( is_white_piece(step[0]) && is_black_piece(step[1]) ) eval += -300;
                    if ( is_black_piece(step[0]) && is_white_piece(step[1]) ) eval +=  300;
                    if ( is_white_piece(step[0]) && is_white_piece(step[1]) ) eval += -200;
                    if ( is_black_piece(step[0]) && is_black_piece(step[1]) ) eval +=  200;
                    if ( i == 1              && is_white_piece(step[0]) && is_black_piece(step[1]) ) eval +=  1000;
                    if ( i == 2              && is_black_piece(step[0]) && is_white_piece(step[1]) ) eval +=  500;
                    if ( i == board_height-3 && is_white_piece(step[0]) && is_black_piece(step[1]) ) eval += -500;
                    if ( i == board_height-2 && is_black_piece(step[0]) && is_white_piece(step[1]) ) eval += -1000;

                }
                if ( step.size() >= 3 ) {
                    if ( is_white_piece(step[1]) && is_black_piece(step[2]) ) eval += -300;
                    if ( is_black_piece(step[1]) && is_white_piece(step[2]) ) eval +=  300;
                    if ( is_white_piece(step[0]) && is_white_piece(step[1]) && is_black_piece(step[2]) ) eval += -2000;
                    if ( is_black_piece(step[0]) && is_black_piece(step[1]) && is_white_piece(step[2]) ) eval +=  2000;
                    if ( is_white_piece(step[0]) && is_white_piece(step[1]) && is_white_piece(step[2]) ) eval += -500;
                    if ( is_black_piece(step[0]) && is_black_piece(step[1]) && is_black_piece(step[2]) ) eval +=  500;
                    if ( i == 1 && step.size() == 3 && (is_black_piece(step[0]) || is_black_piece(step[1])) && is_white_piece(step[2]) ) eval +=  2500;
                    if ( i == board_height-2        && (is_white_piece(step[0]) || is_white_piece(step[1])) && is_black_piece(step[2]) ) eval += -2500;

                }
                if ( step.size() >= 4 ) {
                    if ( (is_white_piece(step[0]) || is_white_piece(step[1])) && is_white_piece(step[2]) && is_black_piece(step[3]) ) eval += -2000;
                }

            }
        }

        // 駒の位置関係
        for( int i = 0; i < board_height; i++ ) {
            for( int j = 0; j < board_width; j++ ) {
                const Cell&               cell = board_elements[i*board_width + j];
                const std::vector<Piece>& step = cell.step;

                // 必勝パターン
                if ( i <= 1 && 1 <= j && j <= board_width-1 && step.size() == 3
                       && !is_black_piece(board_elements[i*board_width + j - 1].get_top_piece()) && !is_black_piece(board_elements[i*board_width + j + 1].get_top_piece())
                       && is_white_piece(step[0]) && is_black_piece(step[1]) && is_white_piece(step[2]) ) eval += 5000;
                if ( i <= 1 && step.size() == 3
                       && (( j == 0 && !is_black_piece(board_elements[i*board_width + j + 1].get_top_piece())) || ( j == board_width-1 && !is_black_piece(board_elements[i*board_width + j - 1].get_top_piece())))
                       && is_white_piece(step[0]) && is_black_piece(step[1]) && is_white_piece(step[2]) ) eval += 5000;
                if ( i >= board_height-2 && 1 <= j && j <= board_width-1 && step.size() == 3
                       && !is_black_piece(board_elements[i*board_width + j - 1].get_top_piece()) && !is_black_piece(board_elements[i*board_width + j + 1].get_top_piece())
                       && is_black_piece(step[0]) && is_white_piece(step[1]) && is_black_piece(step[2]) ) eval += -5000;
                if ( i >= board_height-2 && step.size() == 3
                       && (( j == 0 && !is_black_piece(board_elements[i*board_width + j + 1].get_top_piece())) || ( j == board_width-1 && !is_black_piece(board_elements[i*board_width + j - 1].get_top_piece())))
                       && is_black_piece(step[0]) && is_white_piece(step[1]) && is_black_piece(step[2]) ) eval += -5000;


                // 必敗パターン
                if ( i == 0              && step.size() == 3 && is_black_piece(step[0]) && is_white_piece(step[1]) && is_black_piece(step[2]) ) eval +=  4000;
                if ( i == board_height-1 && step.size() == 3 && is_white_piece(step[0]) && is_black_piece(step[1]) && is_white_piece(step[2]) ) eval += -4000;

            }
        }

        // 乱数で毎回違う手を指すようにする
        eval += -25 + mt() % 50;

        return eval;
    }

    // 現在の盤面をエンコード
    std::pair<int64_t, int64_t> encode_board_surface () const {
        int64_t white = 0, black = 0;

        for( int i = 0; i < board_height; i++ ) {
            for( int j = 0; j < board_width; j++ ) {
                const std::vector<Piece>& step = board_elements[i*board_width + j].step;
                for( int k = 0; k < step.size(); k++ ) {
                    if ( is_white_piece(step[k]) ) {
                        white <<= 5; white += i*board_width + j;
                        white <<= 2; white += k;
                    }
                }
            }
        }
        for( int i = 0; i < board_height; i++ ) {
            for( int j = 0; j < board_width; j++ ) {
                const std::vector<Piece>& step = board_elements[i*board_width + j].step;
                for( int k = 0; k < step.size(); k++ ) {
                    if ( is_non_fourth_black_piece(step[k]) ) {
                        black <<= 5; black += i*board_width + j;
                        black <<= 2; black += k;
                    }
                }
            }
        }
        for( int i = 0; i < board_height; i++ ) {
            for( int j = 0; j < board_width; j++ ) {
                const std::vector<Piece>& step = board_elements[i*board_width + j].step;
                for( int k = 0; k < step.size(); k++ ) {
                    if ( is_fourth_black_piece(step[k]) ) {
                        black <<= 5; black += i*board_width + j;
                        black <<= 2; black += k;
                    }
                }
            }
        }

        return std::make_pair(white, black);
    }

private:
    int board_height, board_width;
    std::vector<Cell> board_elements;


};

#endif
