#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include "common.hpp"
#include "board.hpp"
using namespace std;

#define MAXDEPTH 7

#define STONEIMB_MULT_START  1000
#define STONEIMB_MULT_END    4000
#define MOBILITY_MULT_START  1200
#define PMOBILITY_MULT_START 1200
#define CORNERS_MULT_START   3000
#define PCORNERS_MULT_START  1800
#define SAFETY_MULT_START    2400

// (approximately 537 MB)
#define MAX_TABLE_SIZE 33554432

class Player {

private:
    Side side;
    Board board;
    Movelist movelist;

    struct board_struct board_stack[MAXDEPTH + 2];
    struct movelist_struct movelist_stack[MAXDEPTH + 1];

    TableEntry table;
    size_t table_size;

public:
    Player(Side side);
    ~Player();

    Move *doMove(Move *opponentsMove, int msLeft);
    int32_t negascout(
        Board cur_board, Movelist cur_movelist, Side cur_side,
        int32_t alpha, int32_t beta, uint8_t depth
        );
    int32_t heuristic(Board board);

    Board get_board() { return board; }
};

#endif
