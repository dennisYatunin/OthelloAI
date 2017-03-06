#include "board.hpp"


inline uint64_t downward_moves(
    uint64_t this_side, uint64_t other_side, uint64_t edge_spaces,
    uint64_t empty_spaces, uint8_t shift_amount
    )
{
    // Ignore the other side's stones that lie on the edge.
    other_side &= edge_spaces;

    // As an example, an edge_spaces value of 0xFEFEFEFEFEFEFE00 would represent
    // the following board:
    //
    // 00000000
    // 01111111
    // 01111111
    // 01111111
    // 01111111
    // 01111111
    // 01111111
    // 01111111

    // Set this_side to be all of this side's stones that, when shifted downward
    // by the specified amount, overlap with the other side's stones.
    this_side = (this_side << shift_amount) & other_side;

    // Shift this side's stones downward 5 more times, and find which ones
    // overlap with the other side's stones. Keep track of all the overlapping
    // stones with bitwise-OR.
    this_side |= (this_side << shift_amount) & other_side;
    this_side |= (this_side << shift_amount) & other_side;
    this_side |= (this_side << shift_amount) & other_side;
    this_side |= (this_side << shift_amount) & other_side;
    this_side |= (this_side << shift_amount) & other_side;

    // Shift all the overlapping stones downward once more, and find which ones
    // fall on empty spaces.
    return (this_side << shift_amount) & empty_spaces;
}


inline uint64_t upward_moves(
    uint64_t this_side, uint64_t other_side, uint64_t empty_spaces,
    uint64_t edge_spaces, uint8_t shift_amount
    )
{
    other_side &= edge_spaces;

    this_side = (this_side >> shift_amount) & other_side;

    this_side |= (this_side >> shift_amount) & other_side;
    this_side |= (this_side >> shift_amount) & other_side;
    this_side |= (this_side >> shift_amount) & other_side;
    this_side |= (this_side >> shift_amount) & other_side;
    this_side |= (this_side >> shift_amount) & other_side;

    return (this_side >> shift_amount) & empty_spaces;
}


inline uint64_t all_moves(uint64_t this_side, uint64_t other_side)
{
    uint64_t empty_spaces = !(this_side | other_side);

    return
    // right
    downward_moves(this_side, other_side, empty_spaces, 0x7F7F7F7F7F7F7F7F, 1) |
    // down-left
    downward_moves(this_side, other_side, empty_spaces, 0x00FEFEFEFEFEFEFE, 7) |
    // down
    downward_moves(this_side, other_side, empty_spaces, 0x00FFFFFFFFFFFFFF, 8) |
    // down-right
    downward_moves(this_side, other_side, empty_spaces, 0x007F7F7F7F7F7F7F, 9) |
    // left
    upward_moves  (this_side, other_side, empty_spaces, 0xFEFEFEFEFEFEFEFE, 1) |
    // up-right
    upward_moves  (this_side, other_side, empty_spaces, 0x7F7F7F7F7F7F7F00, 7) |
    // up
    upward_moves  (this_side, other_side, empty_spaces, 0xFFFFFFFFFFFFFF00, 8) |
    // up-left
    upward_moves  (this_side, other_side, empty_spaces, 0xFEFEFEFEFEFEFE00, 9);
}


// <--------------------------------------------------------------------------->


inline uint64_t num_ones(uint64_t x)
{
    // According to Wikipedia, this is the Hamming Weight algorithm which uses
    // the fewest arithmetic operations: 1 multiplication and 11 additions
    // (addition, subtraction, bitwise-AND, and bitshift each require 1
    // processor cycle, while multiplication can require more than 1 cycle).
    x -= ((x >> 1) & 0x5555555555555555);
    x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
    return (((x + (x >> 4)) & 0xF0F0F0F0F0F0F0F) * 0x101010101010101) >> 56;
}


// <--------------------------------------------------------------------------->


void set_board(char *data, Board &board)
{
    for (int i = 0; i < 64; i++)
    {
        if (data[i] == 'w')
            set_stone(board, i / 8, i % 8, WHITE);

        else if (data[i] == 'b')
            set_stone(board, i / 8, i % 8, BLACK);
    }
}
