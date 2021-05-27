
#ifndef PLAYER_H_
#define PLAYER_H_

enum Turn {
    FIRST_MOVE,
    SECOND_MOVE,
};

struct Player {
    Turn turn;
    bool is_computer;

};

#endif
