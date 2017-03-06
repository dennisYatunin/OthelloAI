#ifndef __BOARD_H__
#define __BOARD_H__

#include <cstdint>
#include "common.hpp"


// Since a board contains 64 squares, the stones on it can be stored using two
// 64-bit integers, the first of which stores the white side's stones, and the
// second of which stores the black side's stones.
//
// The least significant bit of each integer determines the state of the upper-
// lefthand corner of the board, and the most significant bit determines the
// state of the bottom-righthand corner.
//
// For example, a standard game of Othello begins with the following board
// configuration:
//
// 00000000
// 00000000
// 00000000
// 000WB000
// 000BW000
// 00000000
// 00000000
// 00000000
//
// This configuration would be stored as follows:
//
// board.bits = {0x0000001008000000, 0x0000000810000000}

typedef struct
{
    uint64_t bits[2];
} Board;

// Returns the stones of the specified side on the board.
#define get_stones(board, side) ((board).bits[side])

// Returns all the stones on the board.
#define all_stones(board) ((board).bits[WHITE] | (board).bits[BLACK])

// Places a stone that belongs to the specified side at the given position on
// the board.
#define set_stone(board, row, col, side) \
((board).bits[side] |= 1 << (8 * (row) + (col)))


// <--------------------------------------------------------------------------->


// Finds the moves available to this side in the direction specified by
// shift_amount. This direction must be "downward"; i.e., right, down-left,
// down, or down-right.
inline uint64_t downward_moves(
    uint64_t this_side, uint64_t other_side, uint64_t edge_spaces,
    uint64_t empty_spaces, uint8_t shift_amount
    );

// Finds the moves available to this side in the direction specified by
// shift_amount. This direction must be "upward"; i.e., left, up-right, up, or
// up-left.
inline uint64_t upward_moves(
    uint64_t this_side, uint64_t other_side, uint64_t empty_spaces,
    uint64_t edge_spaces, uint8_t shift_amount
    );

// Finds all moves available to this side, given the locations of all the stones
// that belong to either side.
inline uint64_t all_moves(uint64_t this_side, uint64_t other_side);

// Returns all the moves available for the specified side.
#define get_moves(board, side) all_moves(board.bits[side], board.bits[!(side)])

// Checks whether there is a move available at the specified position.
#define is_move(moves, row, col) (((moves) >> (8 * (row) + (col))) & 1)


// <--------------------------------------------------------------------------->


// Finds the Hamming Weight of x. (https://en.wikipedia.org/wiki/Hamming_weight)
inline uint64_t num_ones(uint64_t x);

// Returns the number of stones that belong to the specified side.
#define num_stones(board, side) num_ones((board).bits[side])

// Returns whether the board is full.
#define is_full(board) (num_ones(all_stones(board)) == 64)

// Returns the number of moves available.
#define num_moves(moves) num_ones(moves)


// <--------------------------------------------------------------------------->


void set_board(char *data, Board &board);

#endif
