#ifndef __BOARD_H__
#define __BOARD_H__

#include <cstdint>
#include <iostream>
#include "common.hpp"
using namespace std;

/*
 * Since a board contains 64 squares, the stones on it can be stored using two
 * 64-bit integers, the first of which stores the white side's stones, and the
 * second of which stores the black side's stones.
 *
 * The least significant bit of each integer determines the state of the upper-
 * lefthand corner of the board, and the most significant bit determines the
 * state of the bottom-righthand corner.
 *
 * For example, a standard game of Othello begins with the following board
 * configuration:
 *
 * 00000000
 * 00000000
 * 00000000
 * 000WB000
 * 000BW000
 * 00000000
 * 00000000
 * 00000000
 *
 * This configuration would be stored as follows:
 *
 * board->bits = {0x0000001008000000, 0x0000000810000000}
 */

typedef struct board_struct
{
    uint64_t bits[2];
} *Board;

// Manually sets the bits in the board struct.
void set_bits(Board board, uint64_t whites, uint64_t blacks);

// Returns all the stones on the board.
#define all_stones(board) ((board)->bits[WHITE] | (board)->bits[BLACK])

// Returns a 64-bit integer which represents a stone at the given position.
#define new_stone(row, col) (1ULL << (8 * (row) + (col)))

/*
 * Right Edge:
 *
 * 11111110
 * 11111110
 * 11111110
 * 11111110
 * 11111110
 * 11111110
 * 11111110
 * 11111110
 */
#define R_EDGE 0x7F7F7F7F7F7F7F7F

/*
 * Left Edge:
 *
 * 01111111
 * 01111111
 * 01111111
 * 01111111
 * 01111111
 * 01111111
 * 01111111
 * 01111111
 */
#define L_EDGE 0xFEFEFEFEFEFEFEFE


// <--------------------------------------------------------------------------->


// Finds the Hamming Weight of x. (https://en.wikipedia.org/wiki/Hamming_weight)
uint8_t num_ones(uint64_t x);

// Returns the number of stones that belong to the specified side.
#define num_stones(board, side) num_ones((board)->bits[(side)])

// Returns whether or not the board is full.
#define is_full(board) (num_ones(all_stones(board)) == 64)


// <--------------------------------------------------------------------------->


/*
 * Category 1:
 *
 * 10000001
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 10000001
 */
#define CAT1 0x8100000000000081

/*
 * Category 2:
 *
 * 01000010
 * 11000011
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 11000011
 * 01000010
 */
#define CAT2 0x42C300000000C342

/*
 * Category 3:
 *
 * 00100100
 * 00100100
 * 11100111
 * 00000000
 * 00000000
 * 11100111
 * 00100100
 * 00100100
 */
#define CAT3 0x2424E70000E72424


// Returns the number of stones that belong to the specified side which are
// in Category 1 (corner positions).
#define num_cat1(board, side) num_ones((board)->bits[(side)] & CAT1)


// Returns the number of stones that belong to the specified side which are
// in Category 2 (adjacent to corner positions).
#define num_cat2(board, side) num_ones((board)->bits[(side)] & CAT2)


// Returns the number of stones that belong to the specified side which are
// in Category 3 (adjacent to positions adjacent to corner positions).
#define num_cat3(board, side) num_ones((board)->bits[(side)] & CAT3)


// <--------------------------------------------------------------------------->


// Modifies the board so that it contains a stone that belongs to the specified
// side at the given position. Flips the other side's stones appropriately.
void add_stone(Board board, Side side, uint8_t row, uint8_t col);

// Creates a copy of the board, modified so that it contains a stone that
// belongs to the specified side at the given position. Flips the other side's
// stones appropriately.
void add_stone_copy(
    Board board, Side side, uint8_t row, uint8_t col, Board destination
    );


// <--------------------------------------------------------------------------->


// Returns the number of moves available to the specified side.
uint8_t num_moves(Board board, Side side);

// Places all the moves available to the specified side in the specified array,
// setting the first element of the array to the number of moves available.
void get_moves(Board board, Side side, uint8_t *array);


// <--------------------------------------------------------------------------->


// Sets the board state according to a 64-character array where 'w' indicates a
// white stone and 'b' indicates a black stone. (Used for testing purposes.)
void set_board(char *data, Board board);

// Prints the board state to cerr, so that 'w' indicates a white stone and 'b'
// indicates a black stone. (Used for testing purposes.)
void print_board(uint64_t whites, uint64_t blacks);

#endif
