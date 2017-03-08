#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include "common.hpp"
#include "board.hpp"
using namespace std;

#define MAXDEPTH 6

class Player {

private:
    Side side;
    struct board_struct board_stack[MAXDEPTH + 2];
    uint8_t move_stack[MAXDEPTH + 2][64];

public:
    Player(Side side);
    ~Player();

    Move *doMove(Move *opponentsMove, int msLeft);
    int32_t negascout(
        size_t stack_pos, uint8_t depth, int32_t alpha, int32_t beta,
        int8_t multiplier
        );
    int32_t heuristic(size_t stack_pos);

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;
};

#endif
