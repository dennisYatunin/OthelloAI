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
    uint64_t hash;
} *Board;

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

/*
 * Corners:
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
#define CORNERS 0x8100000000000081


// <--------------------------------------------------------------------------->


// Returns the position of a stone on the bitboard.
uint8_t stone_position(uint64_t stone);

// Manually sets the bits in the board struct, updating the hash appropriately.
void set_bits(Board board, uint64_t whites, uint64_t blacks);

// Returns all the stones belonging to the given side.
#define get_stones(board, side) ((board)->bits[side])

// Returns a 64-bit integer which represents a stone at the given position.
#define new_stone(x, y) (1ULL << (8 * (y) + (x)))


// <--------------------------------------------------------------------------->


// Finds the Hamming Weight of x. (https://en.wikipedia.org/wiki/Hamming_weight)
uint8_t num_ones(uint64_t x);

// Returns the number of corner stones in the given bitboard.
#define num_corners(bitboard) num_ones((bitboard) & CORNERS)

// Finds the number of empty spaces next to at least one of the stones that
// belong to this side.
uint8_t num_spaces(uint64_t this_side_stones, uint64_t other_side_stones);


// <--------------------------------------------------------------------------->


// Modifies the board so that it contains the given stone belonging to the
// specified side, flipping the other side's stones and updating the hash
// appropriately.
void add_stone(Board board, Side side, uint64_t stone);

// Creates a copy of the board, modified so that it contains the given stone
// belonging to the specified side, flipping the other side's stones and
// updating the hash appropriately. Stores the copy in the location board + 1.
void add_stone_copy(Board board, Side side, uint64_t stone);


// <--------------------------------------------------------------------------->


// Returns a bitboard containing all the moves available to this side.
uint64_t move_bitboard(uint64_t this_side_stones, uint64_t other_side_stones);

// Finds the number of stones belonging to this side that are safe for at least
// one move.
uint8_t num_safe(
    uint64_t this_side_stones, uint64_t other_side_stones,
    uint64_t other_side_moves
    );


// <--------------------------------------------------------------------------->


typedef struct table_entry_struct
{
    Board board;
    bool in_use;
    uint8_t depth;
    int32_t score;
    uint64_t best_move, second_best_move, third_best_move;
    struct table_entry_struct* next;

    table_entry_struct() { board = nullptr; in_use = false; next = nullptr; };
    table_entry_struct(Board board) { this->board = board; in_use = false; next = nullptr; }
    ~table_entry_struct() { if (this->next) { delete this->next; } }
} *TableEntry;

// Retrieves an entry from the transposition table for the given board.
TableEntry get_entry(Board board, TableEntry table, size_t table_size);


// <--------------------------------------------------------------------------->


/*
 * The maximum number of moves available to a player in a given turn will never
 * exceed 32 (if it did, that would mean that over half the board consisted of
 * legal moves). So, we can store a list of possible moves as an array of 32
 * integers, where each integer encodes a stone that can be played.
 */

typedef struct movelist_struct
{
    uint8_t num_moves;
    uint64_t moves[32];
} *Movelist;

// Returns the specified move in the movelist.
#define get_move(movelist, move_num) ((movelist)->moves[(move_num)])

// Places all the moves available to the specified side in the given movelist,
// as well as the number of moves available.
void get_moves(Board board, Side side, Movelist movelist);

// Optimally sorts the moves in the movelist based on the transposition table
// entry.
void sort_moves(Movelist movelist, TableEntry entry);


// <--------------------------------------------------------------------------->


// Sets the board state according to a 64-character array where 'w' indicates a
// white stone and 'b' indicates a black stone. (Used for testing purposes.)
void set_board(char *data, Board board);

// Prints the board state to cerr, so that 'w' indicates a white stone and 'b'
// indicates a black stone. (Used for testing purposes.)
void print_board(Board board);

#endif
