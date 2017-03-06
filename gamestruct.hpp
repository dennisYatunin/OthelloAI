#ifndef __GAMESTRUCT_H__
#define __GAMESTRUCT_H__

#include <cstdint>

#define BLACK 0
#define WHITE 1

typedef struct
{
    // Since a board contains 64-squares, it can be stored using two 64-bit
    // integers, one of which stores the black stones on the board, and one of
    // which stores the white stones on the board.
    uint64_t board[2];

    uint8_t move_num;
} game;

uint64_t num_ones(uint64_t x)
{
    x -= ((x >> 1) & 0x5555555555555555UL);
    x = (x & 0x3333333333333333UL) + ((x >> 2) & 0x3333333333333333UL);
    return (((x + (x >> 4)) & 0xF0F0F0F0F0F0F0FUL) * 0x101010101010101UL) >> 56;
}

#define num_stones(game, color) num_ones(game.board[color])

#endif
