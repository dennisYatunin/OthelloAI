#include "board.hpp"

void set_bits(Board board, uint64_t whites, uint64_t blacks)
{
    board->bits[WHITE] = whites;
    board->bits[BLACK] = blacks;
}


// <--------------------------------------------------------------------------->


uint8_t num_ones(uint64_t x)
{
    // According to Wikipedia, this is the Hamming Weight algorithm which uses
    // the fewest arithmetic operations: 1 multiplication and 11 additions
    // (addition, subtraction, bitwise-AND, and bitshift each require 1
    // processor cycle, while multiplication can require more than 1 cycle).
    x -= ((x >> 1) & 0x5555555555555555);
    x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
    return
    (uint8_t)((((x + (x >> 4)) & 0xF0F0F0F0F0F0F0F) * 0x101010101010101) >> 56);
}


// <--------------------------------------------------------------------------->


// Finds the other side's stones that need to be flipped in the direction
// specified by shift_amount when this side places a new stone in the given
// position. The direction must be "downward"; i.e., right, down-left, down, or
// down-right.
inline uint64_t downward_flips(
    uint64_t this_side, uint64_t other_side, uint64_t stone,
    uint8_t shift_amount
    )
{
    // Set stone to be the other side's stone that overlaps with the new
    // stone when the new stone is shifted downward by the specified amount.
    stone = (stone << shift_amount) & other_side;

    // Shift the new stone downward 5 more times, and find which of the other
    // side's stones overlap with it. Keep track of all the overlapping stones
    // with bitwise-OR.
    stone |= (stone << shift_amount) & other_side;
    stone |= (stone << shift_amount) & other_side;
    stone |= (stone << shift_amount) & other_side;
    stone |= (stone << shift_amount) & other_side;
    stone |= (stone << shift_amount) & other_side;

    // Check whether there is a stone from this side which, when moved in the
    // opposite direction, overlaps with the last previously-found stone.
    if ((this_side >> shift_amount) & stone)
        return stone;

    return 0;
}

// Finds the other side's stones that need to be flipped in the direction
// specified by shift_amount when this side places a new stone in the given
// position. The direction must be "upward"; i.e., left, up-right, up, or
// up-left.
inline uint64_t upward_flips(
    uint64_t this_side, uint64_t other_side, uint64_t stone,
    uint8_t shift_amount
    )
{
    // Identical algorithm to downward_flips(), but with left bitshift instead
    // of right bitshift, and vice-versa.

    stone = (stone >> shift_amount) & other_side;

    stone |= (stone >> shift_amount) & other_side;
    stone |= (stone >> shift_amount) & other_side;
    stone |= (stone >> shift_amount) & other_side;
    stone |= (stone >> shift_amount) & other_side;
    stone |= (stone >> shift_amount) & other_side;

    if ((this_side << shift_amount) & stone)
        return stone;

    return 0;
}

// Finds all the other side's stones that need to be flipped when this side
// places a new stone in the given position.
inline uint64_t all_flips(
    uint64_t this_side, uint64_t other_side, uint64_t stone
    )
{
    /*
     * Any stones that lie on the left edge should not be moved to the left, and
     * any stones that lie on the right edge should not be moved to the right.
     * Otherwise, the following problem could arise:
     *
     * 00000000            00000000
     * 00000000            00000000
     * 00000000            00000001 <- This 1 should still be a 0,
     * 11000000    >> 1    10000000    since the >> 1 operation was
     * 00000000 =========> 00000000    meant to shift all the stones
     * 00000000            00000000    on the board to the left.
     * 00000000            00000000
     * 00000000            00000000
     */

    return
    // right
    downward_flips(this_side & L_EDGE, other_side & R_EDGE, stone & R_EDGE, 1) |
    // down-left
    downward_flips(this_side & R_EDGE, other_side & L_EDGE, stone & L_EDGE, 7) |
    // down
    downward_flips(this_side,          other_side,          stone,          8) |
    // down-right
    downward_flips(this_side & L_EDGE, other_side & R_EDGE, stone & R_EDGE, 9) |
    // left
    upward_flips  (this_side & R_EDGE, other_side & L_EDGE, stone & L_EDGE, 1) |
    // up-right
    upward_flips  (this_side & L_EDGE, other_side & R_EDGE, stone & R_EDGE, 7) |
    // up
    upward_flips  (this_side,          other_side,          stone,          8) |
    // up-left
    upward_flips  (this_side & R_EDGE, other_side & L_EDGE, stone & L_EDGE, 9);
}

void add_stone(Board board, Side side, uint8_t row, uint8_t col)
{
    uint64_t stone = new_stone(row, col);
    uint64_t flipped_stones =
    all_flips(board->bits[side], board->bits[!side], stone);

    board->bits[side] |= stone | flipped_stones;
    board->bits[!side] &= ~flipped_stones;
}

void add_stone_copy(
    Board board, Side side, uint8_t row, uint8_t col, Board destination
    )
{
    uint64_t stone = new_stone(row, col);
    uint64_t flipped_stones =
    all_flips(board->bits[side], board->bits[!side], stone);

    destination->bits[side] = board->bits[side] | stone | flipped_stones;
    destination->bits[!side] = board->bits[!side] & ~flipped_stones;
}


// <--------------------------------------------------------------------------->


// Finds the moves available to this side in the direction specified by
// shift_amount. The direction must be "downward"; i.e., right, down-left,
// down, or down-right.
inline uint64_t downward_moves(
    uint64_t this_side, uint64_t other_side, uint64_t empty_spaces,
    uint8_t shift_amount
    )
{
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

// Finds the moves available to this side in the direction specified by
// shift_amount. The direction must be "upward"; i.e., left, up-right, up, or
// up-left.
inline uint64_t upward_moves(
    uint64_t this_side, uint64_t other_side, uint64_t empty_spaces,
    uint8_t shift_amount
    )
{
    // Identical algorithm to downward_moves(), but with left bitshift instead
    // of right bitshift.

    this_side = (this_side >> shift_amount) & other_side;

    this_side |= (this_side >> shift_amount) & other_side;
    this_side |= (this_side >> shift_amount) & other_side;
    this_side |= (this_side >> shift_amount) & other_side;
    this_side |= (this_side >> shift_amount) & other_side;
    this_side |= (this_side >> shift_amount) & other_side;

    return (this_side >> shift_amount) & empty_spaces;
}

// Finds all moves available to this side, given the locations of all the stones
// that belong to either side.
inline uint64_t all_moves(uint64_t this_side, uint64_t other_side)
{
    uint64_t empty_spaces = ~(this_side | other_side);

    return
    // right
    downward_moves(this_side & R_EDGE, other_side & R_EDGE, empty_spaces, 1) |
    // down-left
    downward_moves(this_side & L_EDGE, other_side & L_EDGE, empty_spaces, 7) |
    // down
    downward_moves(this_side,          other_side,          empty_spaces, 8) |
    // down-right
    downward_moves(this_side & R_EDGE, other_side & R_EDGE, empty_spaces, 9) |
    // left
    upward_moves  (this_side & L_EDGE, other_side & L_EDGE, empty_spaces, 1) |
    // up-right
    upward_moves  (this_side & R_EDGE, other_side & R_EDGE, empty_spaces, 7) |
    // up
    upward_moves  (this_side,          other_side,          empty_spaces, 8) |
    // up-left
    upward_moves  (this_side & L_EDGE, other_side & L_EDGE, empty_spaces, 9);
}

uint8_t num_moves(Board board, Side side)
{
    return num_ones(all_moves(board->bits[side], board->bits[!side]));
}

// An array for finding the index of the least significant bit in a 64-bit
// integer using the DeBruijn sequence 0x03f79d71b4cb0a89.
const uint8_t DeBruijnIndices64Bit[] = {
     0,  1, 48,  2, 57, 49, 28,  3,
    61, 58, 50, 42, 38, 29, 17,  4,
    62, 55, 59, 36, 53, 51, 43, 22,
    45, 39, 33, 30, 24, 18, 12,  5,
    63, 47, 56, 27, 60, 41, 37, 16,
    54, 35, 52, 21, 44, 32, 23, 11,
    46, 26, 40, 15, 34, 20, 31, 10,
    25, 14, 19,  9, 13,  8,  7,  6
};

// Creates an array whose first element is the number of 1's in bits and whose
// remaining elements are the indices of all the 1's (with the least-significant
// 1 having the smallest index).
inline void get_indices(uint64_t bits, uint8_t *array)
{
    // Get the number of 1's in bits.
    uint8_t num_indices = num_ones(bits);

    // Set the first element of the array to the number of 1's.
    array[0] = num_indices;

    if (num_indices > 0)
    {
        do
        {
            // Store the indices of the 1's, with the index of the least
            // significant 1 at the end of the array and the index of the most
            // significant 1 at the front of the array. Obtain each subsequent
            // index from DeBruijnIndices64Bit[] using the algorithm described
            // at http://chessprogramming.wikispaces.com/BitScan. After each
            // subsequent index is obtained, set the 1 at that index to a 0 by
            // calling bits &= bits - 1.
            array[--num_indices + 1] =
            DeBruijnIndices64Bit[((bits & -bits) * 0x03f79d71b4cb0a89) >> 58];
        } while ((bits &= bits - 1) > 0);
    }
}

void get_moves(Board board, Side side, uint8_t *array)
{
    get_indices(all_moves(board->bits[side], board->bits[!side]), array);
}


// <--------------------------------------------------------------------------->


void set_board(char *data, Board board)
{
    for (int i = 0; i < 64; i++)
    {
        if (data[i] == 'w')
            board->bits[WHITE] |= new_stone(i / 8, i % 8);

        else if (data[i] == 'b')
            board->bits[BLACK] |= new_stone(i / 8, i % 8);
    }
}

void print_board(uint64_t whites, uint64_t blacks)
{

    for (int i = 0; i < 8; i++)
    {
        cerr << "----------------" << endl;
        for (int j = 0; j < 7; j++)
        {
            if (whites & 1)
                cerr << "w|";

            else if (blacks & 1)
                cerr << "b|";

            else
                cerr << " |";

            whites >>= 1;
            blacks >>= 1;
        }

        if (whites & 1)
            cerr << "w" << endl;

        else if (blacks & 1)
            cerr << "b" << endl;

        else
            cerr << " " << endl;

        whites >>= 1;
        blacks >>= 1;
    }

    cerr << "----------------" << endl;
}
