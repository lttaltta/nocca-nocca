#include <iostream>
#include <string>
#include "game.h"
#include "cmdline.h"


int main (int argc, char *argv[]) {

    int board_height = 6;
    int board_width  = 5;



    cmdline::parser args;

    args.add<std::string>("first-player" ,         'f', "first-player" ,            false, "computer", cmdline::oneof<std::string>("human", "computer"));
    args.add<std::string>("second-player",         's', "second-player",            false, "computer", cmdline::oneof<std::string>("human", "computer"));
    args.add<int>(        "fourth-black"  ,        'b', "fourth-black piece index", false,          0, cmdline::range(0,100));
    args.add<int>(        "max-search-sec",        't', "max search seconds",       false,         30, cmdline::range(1,999999));
    args.add<int>(        "max-search-candidates", 'c', "max search candidates",    false,       9999, cmdline::range(1,999999));

    args.add<std::string>("load-board-file",  'l', "load-board-file",  false, "" );
    args.add<std::string>("next-turn-player", 'n', "netx-turn-player", false, "first-player", cmdline::oneof<std::string>("first-player", "second-player") );


    args.parse_check(argc, argv);


    Game game(board_height, board_width);

    game.set_max_search_sec( args.get<int>("max-search-sec") );
    game.set_max_search_candidates( args.get<int>("max-search-candidates") );


    if ( args.get<std::string>("load-board-file") != "" ) {
        game.load_board_file( args.get<std::string>("load-board-file") );
        game.set_game(args.get<std::string>("first-player"), args.get<std::string>("second-player"));
        game.print_board();

        game.battle(args.get<std::string>("next-turn-player") == "second-player");

    }
    else {
        game.init_game(args.get<std::string>("first-player"), args.get<std::string>("second-player"), args.get<int>("fourth-black"));
        game.print_board();

        game.battle();

    }



    printf("aa");
    int t;
    std::cin >> t;
    return 0;
}