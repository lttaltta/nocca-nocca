
#ifndef GAME_H_
#define GAME_H_

#include <iostream>
#include <random>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <utility>
#include <cassert>
#include <map>
#include "player.h"
#include "board.h"
#include "candidate.h"
#include "csv.h"


std::random_device rnd;
std::mt19937       mt((int)time(0));

using EvalCandidate = std::pair<int, Candidate>;

class Game {
public:

    Game (int height, int width) : board(height, width) {
        max_search_sec        = 30;
        max_search_candidates = 999999;
    }

    void print_board () { board.print_board(); }

    void set_max_search_sec       (int n) { max_search_sec = n; }
    void set_max_search_candidates(int n) { max_search_candidates = n; }

    // ゲームの初期化
    void init_game ( std::string first_player_type, std::string second_player_type, int fourth_black_piece_x = 0 ) {

        // 千日手チェックを初期化
        board_surface.clear();

        // 初期配置に設置
        for (int i = 0; i < board.get_board_width(); i++ ) {
            board.board_reference(board.get_board_height()-1, i).push_piece_to_cell(WHITE_PIECE);
            board.board_reference(0, i).push_piece_to_cell(BLACK_PIECE);
        }

        // FOURTH_BLACK_PIECE を配置
        if ( fourth_black_piece_x == 0 ) {
            // 0 のときは乱数で配置
            fourth_black_piece_x = mt() % board.get_board_width() + 1;
        }
        fourth_black_piece_x -= 1;
        board.board_reference(0, fourth_black_piece_x).pop_piece_to_cell();
        board.board_reference(0, fourth_black_piece_x).push_piece_to_cell(FOURTH_BLACK_PIECE);


        first_player.turn         = FIRST_MOVE;
        first_player.is_computer  = first_player_type == "computer";
        second_player.turn        = SECOND_MOVE;
        second_player.is_computer = second_player_type == "computer";

    }


    bool load_board_file (const std::string filename) {
        CSV csv(filename);

        for ( int i = 0; i < board.get_board_height(); i++ ) {
            for ( int j = 0; j < board.get_board_width(); j++ ) {
                for ( int k = 0; k < csv[i][j].size(); k++ ) {
                    board.board_reference(i, j).push_piece_to_cell( (Piece)csv[i][j][k] );
                }
            }
        }

        return true;
    }


    bool set_game ( std::string first_player_type, std::string second_player_type ) {

        // 千日手チェックを初期化
        board_surface.clear();

        // 先行後攻
        first_player.turn         = FIRST_MOVE;
        first_player.is_computer  = first_player_type == "computer";
        second_player.turn        = SECOND_MOVE;
        second_player.is_computer = second_player_type == "computer";

        return true;
    }


    // 指せる候補手を返す
    std::vector<Candidate> get_candidates (const Player &player) {
        std::vector<Candidate> candidates;

        for (int i = 0; i < board.get_board_height(); i++ ) {
            for (int j = 0; j < board.get_board_width(); j++ ) {
                Cell& src_cell = board.board_reference(i, j);
                if ( is_match_player_and_piece(player, src_cell.get_top_piece()) ) {
                    for(int k = 0; k < sizeof(candidate_dy)/sizeof(int); k++ ) {
                        Candidate candidate = {i, j, i+candidate_dy[k], j+candidate_dx[k]};
                        if ( is_move_piece(player, candidate) ) {
                            candidates.push_back( candidate );
                        }
                    }
                }
            }
        }

        if ( player.turn == SECOND_MOVE ) std::reverse( candidates.begin(), candidates.end() );

        return candidates;
    }


    // 指せる候補手を返す
    std::vector<Candidate> get_candidates_order (const Player &player) {
        std::vector<Candidate> candidates;

        for (int i = 0; i < board.get_board_height(); i++ ) {
            for (int j = 0; j < board.get_board_width(); j++ ) {
                Cell& src_cell = board.board_reference(i, j);
                if ( is_match_player_and_piece(player, src_cell.get_top_piece()) ) {
                    for(int k = 0; k < sizeof(candidate_dy)/sizeof(int); k++ ) {
                        Candidate candidate = {i, j, i+candidate_dy[k], j+candidate_dx[k]};
                        if ( is_move_piece(player, candidate) ) {
                            candidates.push_back( candidate );
                        }
                    }
                }
            }
        }

        std::vector<EvalCandidate> candidates_order;
        for( auto candidate : candidates ) {
            move_piece(player, candidate);
            candidates_order.push_back( std::make_pair( board.board_evaluation(), candidate) );
            move_piece(player, Candidate{candidate.dst_y, candidate.dst_x, candidate.src_y, candidate.src_x});
        }
        assert( candidates.size() == candidates_order.size() );
        std::stable_sort( candidates_order.begin(), candidates_order.end() );

        for( int i = 0; i < candidates_order.size(); i++ ) {
            candidates[i] = candidates_order[i].second;
        }

        if ( player.turn == FIRST_MOVE ) std::reverse( candidates.begin(), candidates.end() );

        return candidates;
    }



    // 対戦
    void battle ( bool next_turn_second_player = false ) {


        while (1) {

            //
            // 先手
            //

            if ( ! next_turn_second_player ) {
                next_turn_second_player = false;

                // 手の候補を取得
                std::vector<Candidate> candidate_first_player = get_candidates(first_player);

                // 詰み確認
                if (is_checkmate(first_player) ) {
                    printf("Winner first player\n");
                    return;
                }
                else if ( candidate_first_player.empty() ) {
                    printf("Winner second player\n");
                    return;
                }


                if ( ! first_player.is_computer ) search(first_player, second_player);


                // 手を選択し、移動
                printf("\n\nFirst move\n");
                Candidate select_move_first_player = select_move(first_player, second_player, candidate_first_player);
                move_piece(first_player, select_move_first_player); print_board();
                printf("Eval : %d\n\n", board.board_evaluation());


                // 千日手チェック
                if ( is_repetition_of_moves() ) {
                    printf("Repetition of moves!\nDraw game.\n");
                    return;
                }
            }

            //
            // 後手
            //

            if (1) {
                next_turn_second_player = false;

                // 手の候補を取得
                std::vector<Candidate> candidate_second_player = get_candidates(second_player);

                // 詰み確認
                if ( is_checkmate(second_player) ) {
                    printf("Winner second player\n");
                    return;
                }
                else if ( candidate_second_player.empty() ) {
                    printf("Winner first player\n");
                    return;
                }


                if ( ! second_player.is_computer ) search(second_player, first_player);


                // 手を選択し、移動
                printf("\n\nSecond move\n");
                Candidate select_move_second_player = select_move(second_player, first_player, candidate_second_player);
                move_piece(second_player, select_move_second_player); print_board();
                printf("Eval : %d\n\n", board.board_evaluation());


                // 千日手チェック
                if ( is_repetition_of_moves() ) {
                    printf("Repetition of moves!\nDraw game.\n");
                    return;
                }
            }

        }



    }

private:

    Player first_player, second_player;
    Board board;
    std::map<std::pair<int64_t,int64_t>,int> board_surface;
    std::map<std::pair<int64_t,int64_t>,int> search_board_surface;

    std::chrono::system_clock::time_point  start_time, end_time;

    const int EVAL_MAX              = 999999;
    int       max_search_sec        = 3;
    int       max_search_candidates = 50;

    int cut_repetition_of_moves;
    int node_count;
    int lower_select_order;


    // ピースを移動させる
    bool move_piece (const Player& player, Candidate candidate) {

        // 移動できるか
        if ( ! is_move_piece(player, candidate) ) return false;

        // 移動元セルのピースを取得
        Cell& src_cell = board.board_reference(candidate.src_y, candidate.src_x);
        Cell& dst_cell = board.board_reference(candidate.dst_y, candidate.dst_x);
        Piece target_piece = src_cell.get_top_piece();

        // 移動
        src_cell.pop_piece_to_cell();
        dst_cell.push_piece_to_cell(target_piece);

        // 移動できた
        return true;
    }

    // プレイヤーとピースが一致しているかどうか
    bool is_match_player_and_piece (const Player &player, Piece piece) {
        if ( player.turn == FIRST_MOVE && is_white_piece(piece) ) {
            return true;
        }

        if ( player.turn == SECOND_MOVE && is_black_piece(piece) ) {
            return true;
        }

        return false;
    }

    // ピースを移動できるかどうか
    bool is_move_piece (const Player& player, Candidate candidate) {

        int src_y = candidate.src_y;
        int src_x = candidate.src_x;
        int dst_y = candidate.dst_y;
        int dst_x = candidate.dst_x;

        // インデックスがはみ出していないか
        if ( ! (0 <= src_y && src_y < board.get_board_height() && 0 <= src_x && src_x < board.get_board_width()
             && 0 <= dst_y && dst_y < board.get_board_height() && 0 <= dst_x && dst_x < board.get_board_width() ) ) {
            return false;
        }

        Cell& src_cell = board.board_reference(candidate.src_y, candidate.src_x);
        Cell& dst_cell = board.board_reference(candidate.dst_y, candidate.dst_x);
        Piece target_piece;

        // 移動元セルにピースがあるかどうか
        if ( ! src_cell.is_pop_piece_to_cell() ) return false;

        // 移動元セルのピースを取得
        target_piece = src_cell.get_top_piece();

        // 移動元セルのピースと、プレイヤーが一致しているかどうか
        if ( ! is_match_player_and_piece(player, target_piece) ) return false;

        // 移動先セルにピースを移動できるかどうか
        if ( ! dst_cell.is_push_piece_to_cell(target_piece) ) return false;

        return true;
    }

    // 手を選択
    Candidate select_move (const Player& current_player, const Player& opponet_player, const std::vector<Candidate>& candidates) {

        // コンピュータの場合、探索
        if ( current_player.is_computer ) {
            return (search(current_player, opponet_player)).second;
        }
        // 人間の場合、選択
        else {
            // 手を選択
            for ( int i  = 0; i < candidates.size(); i++ ) {
                printf("%d : (%d,%d) -> (%d,%d)\n", i, candidates[i].src_y+1, candidates[i].src_x+1, candidates[i].dst_y+1, candidates[i].dst_x+1);
            }
            int select;
            std::cin >> select;
            return candidates[select];
        }

    }

    // 詰みかどうか判定
    bool is_checkmate (const Player& player) {

        // 先手の時、詰みかどうか判定
        if ( player.turn == FIRST_MOVE )  {
            for (int i = 0; i < board.get_board_width(); i++ ) {
                if ( is_white_piece( board.board_reference(0, i).get_top_piece() ) ) {
                    return true;
                }
            }

            return false;
        }


        // 後手の時、詰みかどうか判定
        if ( player.turn == SECOND_MOVE )  {
            for (int i = 0; i < board.get_board_width(); i++ ) {
                if ( is_black_piece( board.board_reference(board.get_board_height()-1, i).get_top_piece() ) ) {
                    return true;
                }
            }

            return false;
        }

        return false;
    }

    // 探索
    EvalCandidate search(const Player& current_player, const Player& opponet_player) {

        int max_depth = 30;

        // 時間計測
        start_time = std::chrono::system_clock::now();

        EvalCandidate ret_candidate;
        for (int depth = 1; depth <= max_depth; depth++ ) {

            // 初期化
            search_board_surface.clear();
            cut_repetition_of_moves = 0;
            node_count = 0;
            lower_select_order = 0;

            // アルファベータ法で探索
            EvalCandidate tmp;
            tmp = search_alphabeta(current_player, opponet_player, depth, -EVAL_MAX*2, EVAL_MAX*2);


            // 読み切ったら強制終了
            if ( tmp.first >= EVAL_MAX || tmp.first <= -EVAL_MAX ) {
                printf("Depth : %2d, Eval : %5d, Cuts : %6d, Nodes : %8d, Lower : %3.1f\%, (%d,%d) -> (%d,%d)\n", depth, tmp.first, cut_repetition_of_moves, node_count, 100.0 * lower_select_order / node_count, tmp.second.src_y+1, tmp.second.src_x+1, tmp.second.dst_y+1, tmp.second.dst_x+1);
                ret_candidate = tmp;
                break;
            }

            // 指定の時間を超えたら強制終了
            end_time = std::chrono::system_clock::now();
            if ( std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count() > max_search_sec ) {
                printf("Search discontinuation\n");
                break;
            }

            // depth での探索結果
            printf("Depth : %2d, Eval : %5d, Cuts : %6d, Nodes : %8d, Lower : %3.1f\%, (%d,%d) -> (%d,%d)\n", depth, tmp.first, cut_repetition_of_moves, node_count, 100.0 * lower_select_order / node_count, tmp.second.src_y+1, tmp.second.src_x+1, tmp.second.dst_y+1, tmp.second.dst_x+1);
            ret_candidate = tmp;

        }

        return ret_candidate;
    }

    // アルファベータ法
    EvalCandidate search_alphabeta (const Player& current_player, const Player& opponet_player, int depth, int alpha, int beta) {

        // 詰みなら評価値最大で返す
        if ( is_checkmate(current_player) ) {
            return std::make_pair((EVAL_MAX+depth) * (current_player.turn == FIRST_MOVE ? 1 : -1), Candidate() );
        }

        // 手の候補を取得
        std::vector<Candidate> candidates = get_candidates_order(current_player);

        // 手の候補がない場合、負けなので評価値最低で返す
        if ( candidates.empty() ) {
            return std::make_pair((EVAL_MAX+depth) * (current_player.turn == FIRST_MOVE ? -1 : 1), Candidate() );
        }

        // 深さが0になった
        if (depth == 0) return std::make_pair(board.board_evaluation(), Candidate() );

        // // 同じ盤面は探索しない
        // if ( (search_board_surface[board.encode_board_surface()] += 1) >= 4 ) {
        //     cut_repetition_of_moves ++;
        //     return std::make_pair((EVAL_MAX+depth) * (current_player.turn == FIRST_MOVE ? -1 : 1), Candidate() );
        // }

        // 指定の時間を超えたら強制終了
        end_time = std::chrono::system_clock::now();
        if ( std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count() > max_search_sec ) {
            return std::make_pair(0, Candidate() );
        }

        int select_order;

        // 各手を検証
        if ( current_player.turn == FIRST_MOVE ) {

            EvalCandidate alpha_pair = std::make_pair(alpha, Candidate() );
            for ( int i = 0; i < candidates.size() && i < max_search_candidates; i++ ) {
                Candidate& candidate = candidates[i];

                move_piece(current_player, candidate);

                // if ( (search_board_surface[board.encode_board_surface()] += 1) <= 4 ) {
                    EvalCandidate tmp = search_alphabeta(opponet_player, current_player, depth-1, alpha_pair.first, beta);
                    if ( alpha_pair.first < tmp.first ) {
                        alpha_pair.first  = tmp.first;
                        alpha_pair.second = candidate;

                        select_order = i;
                    }
                // }
                // else {
                //     cut_repetition_of_moves ++;
                // }

                move_piece(current_player, Candidate{candidate.dst_y, candidate.dst_x, candidate.src_y, candidate.src_x});

                // ベータカット
                if ( alpha_pair.first >= beta ) break;
            }

            node_count ++;
//            if ( (double)select_order / candidates.size() <= 0.75 ) lower_select_order ++;
            if ( select_order < 30 ) lower_select_order ++;

            return alpha_pair;
        }
        else {

            EvalCandidate beta_pair = std::make_pair(beta, Candidate() );
            for ( int i = 0; i < candidates.size() && i < max_search_candidates; i++ ) {
                Candidate& candidate = candidates[i];

                move_piece(current_player, candidate);

                // if ( (search_board_surface[board.encode_board_surface()] += 1) <= 4 ) {
                    EvalCandidate tmp = search_alphabeta(opponet_player, current_player, depth-1, alpha, beta_pair.first);
                    if ( beta_pair.first > tmp.first ) {
                        beta_pair.first  = tmp.first;
                        beta_pair.second = candidate;

                        select_order = i;
                    }
                // }
                // else {
                //     cut_repetition_of_moves ++;
                // }

                move_piece(current_player, Candidate{candidate.dst_y, candidate.dst_x, candidate.src_y, candidate.src_x});

                // ベータカット
                if ( alpha >= beta_pair.first ) break;
            }

            node_count ++;
//            if ( (double)select_order / candidates.size() <= 0.75 ) lower_select_order ++;
            if ( select_order < 30 ) lower_select_order ++;

            return beta_pair;
        }

    }

    // 千日手かどうかチェック
    bool is_repetition_of_moves () {

        const int max_repetition_of_move_times = 4;

        std::pair<int64_t, int64_t> encoded_board_surface = board.encode_board_surface();

        board_surface[encoded_board_surface] = board_surface[encoded_board_surface] + 1;

        return board_surface[encoded_board_surface] >= max_repetition_of_move_times;
    }
};

#endif
