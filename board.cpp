#include "board.hpp"

// An array for finding the index of the least significant bit in a 64-bit
// integer using the DeBruijn sequence 0x03f79d71b4cb0a89. Obtained from
// http://chessprogramming.wikispaces.com/BitScan.
const uint8_t DEBRUIJN_INDICES_64BIT[] = {
     0,  1, 48,  2, 57, 49, 28,  3,
    61, 58, 50, 42, 38, 29, 17,  4,
    62, 55, 59, 36, 53, 51, 43, 22,
    45, 39, 33, 30, 24, 18, 12,  5,
    63, 47, 56, 27, 60, 41, 37, 16,
    54, 35, 52, 21, 44, 32, 23, 11,
    46, 26, 40, 15, 34, 20, 31, 10,
    25, 14, 19,  9, 13,  8,  7,  6
};

uint8_t stone_position(uint64_t stone)
{
    // Find the index of the least significant bit in the stone using the
    // DeBruijn sequence 0x03f79d71b4cb0a89.
    return DEBRUIJN_INDICES_64BIT[(stone * 0x03f79d71b4cb0a89) >> 58];
}

// 192 random 64-bit integers, generated using stdlib's rand() function.
const uint64_t RAND_HASHES[] = {
    0x3af4a91d18e89d7f, 0x8a0be6d7323930b9, 0xb9006a31d10e4e4b,
    0x91be4b119e68c9f5, 0x45b604fbe1f7e18c, 0xea8a59d46eb648c4,
    0x1fafc825dd7465b6, 0x95b2ef2375af4836, 0x1c828f6f3abcd924,
    0x1972cf70875b7eaa, 0xa0ebcef955a127e4, 0xe9ea7df586dea67a,
    0x0ca05142ac398df7, 0x6b9e3a8b3df071b6, 0x5849a2f01e300409,
    0xdb007c377619199f, 0x6d08c5a023fcdba4, 0x1646a60f830fcaa1,
    0x2f14a374309508c6, 0x15e1cf840583825b, 0xa003f06c61180c30,
    0x0fee1e4b45497034, 0x2086ca699cfe4663, 0x329c68c04bf74602,
    0xf8e6126a4c38eea4, 0x52e89a32766bddf7, 0xdb8ac5e140a86b22,
    0x01f6f3a8188d65c0, 0x780f6482858fcb2a, 0x51afb77961fd08f1,
    0xbcbd9ccf7cd64ec9, 0x1d44a811f41d1dbb, 0xd05f77af4da21d7e,
    0xc74bfa1e1467c0fb, 0x22d0fb34bc80d06d, 0x0d7eebd321763855,
    0xea5d56d1aa4652bf, 0xea1c50bc96aa3158, 0xf973fd6871b87bc2,
    0x33015179d43cd278, 0xdbb5983cd0949f31, 0xb65574115e4279d2,
    0xc0e6a0e8ac06d318, 0xa44a22eae0ebff71, 0x23e971af84232171,
    0x67595abc1e77b465, 0x94e977be079fae56, 0xc1f9b2f544b6dca6,
    0xc036c189d9c776be, 0xcba808b5a821d32e, 0x85d05d0a19f68065,
    0xf337c44a2b1dafed, 0x6c87ebe63e2af1b3, 0x90808f832cf51a76,
    0x6480b23b21a0b14b, 0x7beab52fa9820066, 0x5b01dd16173408b2,
    0x2e5a88efe2661f8e, 0x227a68bc04905533, 0x842899a370dbbfac,
    0x9c462fa078050f6f, 0x199ed109b3ed06ae, 0xb8294928cbf388b7,
    0x5f6c63423f8447a9, 0x970601379d492db3, 0x0ef3e7892d7963a1,
    0x2093e7e14fb8996f, 0xe549982694ae2274, 0xb2e5718ec61d1979,
    0x331dd2dbc8b13876, 0x6e4b12fb0e283b9d, 0xe6e012a83eaf41ef,
    0xdc9660e3b306a51a, 0x89c11064e9328aab, 0x759514f975c88cf9,
    0x1e430936cc1a90d1, 0x95466093bb28216c, 0xfa835a8a39806056,
    0xdd5f8263a34b87d1, 0x058ef6d761b46e70, 0x30ca1839447771b8,
    0x02946990eb1c69e2, 0xa26f5247a3f62705, 0xafa0a8b1ace3e4f8,
    0x56931bab9914b8da, 0x60fa5aac16f0e3f3, 0x8794d663ae346126,
    0x2332b006dca8568e, 0x645f125d5f7e4e14, 0xed6e242f16d3d406,
    0xddc699c845c66e8a, 0x8deac80a1db189dd, 0xd81d718483553b07,
    0x74840674ad485a9f, 0xdeebe86c63030dbd, 0x7474e5995bd172a0,
    0xbfc1a1276b5d2f4f, 0xeb3566e7f859f449, 0xdf09080e705622f6,
    0xeb4fbc1beff79f3f, 0x99b1461bb5b65bc7, 0x6bf21bfb7e3162b5,
    0x8581ebc925680820, 0x572e7928a817ff83, 0x334b7f5a6f867726,
    0xa0e24a475a44d811, 0xd091f41627ef8a8d, 0x876e83fb4bfa4ac0,
    0x3e0e7fd053a93719, 0xb299465fc94c6861, 0x66d53cff8d26b5aa,
    0x47842a9422194fe0, 0xf63d589c1b7541a8, 0xe1b3439122d43354,
    0x0f2cf00049dee2c2, 0xa82dea7d22f7096f, 0xc53fcb37ea155b5b,
    0x2aff6bada9c4d5ca, 0x7f0e272f4c9094e9, 0x43974e113a225812,
    0x1d4b97b2edf1e6a7, 0x076fb73f8efc2671, 0xaf9249dca2bf48b1,
    0xc650b68c4b4e94d1, 0xe787b6fc18cf8cf4, 0x5813c8e8b4410028,
    0x7514a20e69fda22f, 0x6c091bbf7da21462, 0x5229ca3208349c0c,
    0xae9a89f17ead76c4, 0x0014bc7b0467a0eb, 0xcd4f68fcbbd84fe4,
    0x1f7a2f22860bdb76, 0x4feb9ae99a1cec82, 0x6cf851cbe1444f2c,
    0x807e54652445b877, 0x1e021f4de4f0bd2c, 0x7dc33df036c40846,
    0x16d8c2490450020a, 0x9c86a92e16c599a2, 0x16e9ab30268085a9,
    0x153cadd614c5263d, 0x536f635944ae8863, 0x7363c5d919b634d0,
    0x25f19df9a5a60b7c, 0x1f8075849a2a4957, 0x4c4832237857daac,
    0x3bb0bb1d94b6e80c, 0xf26d703148e43256, 0x8e53d5630b4e2a49,
    0x33a443cf124f3ca5, 0xc645dfac6d160392, 0xb44477166ef1e419,
    0x64d9967408a87052, 0xd1db819272dfa3f6, 0x9f7e8339f788b041,
    0x392f0f7125bb67c9, 0x29ca88b9978990cb, 0x5a7ba51d982baa53,
    0x40a5765dc4abbec3, 0x91e31e48a34623d4, 0xa519560503c9921f,
    0x47dc9fb90f5b1ff4, 0xd10d49a50a67d2a2, 0x8a72e227e01d0658,
    0x38c7b076a59c26bb, 0x98d8e9b13d23a0fe, 0x7ed04209c0d6ce7a,
    0xbc804704c534088a, 0x7afb4de1b8b9406e, 0xe131c8da4acd6030,
    0x23911f90e07fdf3d, 0x5619d2bd21318922, 0x3dd5198f0fe7f048,
    0x4e0a052953a2b8ed, 0xa41e000309e95cbd, 0xc03f79d8e86d756c,
    0x0f3fcd4d0dabf189, 0xeb06d7e5e6b3eefe, 0x3915b429cda91941,
    0xcacfdeeb1400160f, 0xb78188717e7c7464, 0xe7d5246d7f17af8c,
    0xf8131ecc9335bd5e, 0x9637396a88b3d186, 0x1eb440890c013ffd,
    0xd92f0c3d440071ed, 0x20d9c60f92d20495, 0x928af6c3954bfe3e,
    0x55a27279636d665d, 0x0e55e7c33849644b, 0xae6314889004daa1
};

// Retrieves a hash for a given stone that belongs to the specified side.
inline uint64_t get_hash(uint64_t stone, Side side)
{
    return RAND_HASHES[stone_position(stone) + side * 64];
}


void set_bits(Board board, uint64_t whites, uint64_t blacks)
{
    board->bits[WHITE] = whites;
    board->bits[BLACK] = blacks;

    board->hash = 0;
    uint64_t stone = 1ULL;
    while (stone)
    {
        if (whites & stone)
            board->hash ^= get_hash(stone, WHITE);
        else if (blacks & stone)
            board->hash ^= get_hash(stone, BLACK);
        else
            board->hash ^= get_hash(stone, EMPTY);

        stone <<= 1;
    }
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

uint8_t num_spaces(uint64_t this_side_stones, uint64_t other_side_stones)
{
    return num_ones(
        (
            ((this_side_stones & R_EDGE) << 1) |     // right
            ((this_side_stones & L_EDGE) << 7) |     // down-left
            ( this_side_stones           << 8) |     // down
            ((this_side_stones & R_EDGE) << 9) |     // down-right
            ((this_side_stones & L_EDGE) >> 1) |     // left
            ((this_side_stones & R_EDGE) >> 7) |     // up-right
            ( this_side_stones           >> 8) |     // up
            ((this_side_stones & L_EDGE) >> 9)       // up-left
        ) & ~(this_side_stones | other_side_stones)  // empty spaces
        );
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

void add_stone(Board board, Side side, uint64_t stone)
{
    uint64_t flipped_stones =
    all_flips(board->bits[side], board->bits[!side], stone);

    board->bits[side] |= stone | flipped_stones;
    board->bits[!side] &= ~flipped_stones;

    board->hash ^= get_hash(stone, side) ^ get_hash(stone, EMPTY);

    do
        board->hash ^=
        get_hash(flipped_stones, side) ^ get_hash(flipped_stones, !side);
    while ((flipped_stones &= flipped_stones - 1));
}

void add_stone_copy(Board board, Side side, uint64_t stone)
{
    uint64_t flipped_stones =
    all_flips(board->bits[side], board->bits[!side], stone);

    (board + 1)->bits[side] = board->bits[side] | stone | flipped_stones;
    (board + 1)->bits[!side] = board->bits[!side] & ~flipped_stones;

    (board + 1)->hash =
    board->hash ^ get_hash(stone, side) ^ get_hash(stone, EMPTY);

    do
        (board + 1)->hash ^=
        get_hash(flipped_stones, side) ^ get_hash(flipped_stones, !side);
    while ((flipped_stones &= flipped_stones - 1));
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

uint64_t move_bitboard(uint64_t this_side_stones, uint64_t other_side_stones)
{
    return all_moves(this_side_stones, other_side_stones);
}

uint8_t num_safe(
    uint64_t this_side_stones, uint64_t other_side_stones,
    uint64_t other_side_moves
    )
{
    uint64_t flipped_stones = 0;

    do
        flipped_stones |= all_flips(
            other_side_stones, this_side_stones,
            other_side_moves & -other_side_moves
            );  // get all stones flipped after last move in other_side_moves
    while ((other_side_moves &= other_side_moves - 1));

    return num_ones(this_side_stones & ~flipped_stones);
}


// <--------------------------------------------------------------------------->


TableEntry get_entry(Board board, TableEntry table, size_t table_size)
{
    // Since table_size is a power of 2, hash % table_size is equivalent to
    // hash & (table_size - 1).
    TableEntry entry = table + (board->hash & (table_size - 1));

    if (!entry->in_use)
    {
        entry->board = board;
        return entry;
    }

    if (
        board->bits[WHITE] == entry->board->bits[WHITE] &&
        board->bits[BLACK] == entry->board->bits[BLACK]
        )
        return entry;

    // Deal with hash collisions using a linked list: every time a new collision
    // is detected, add a new entry containing the board that caused the
    // collision to the tail of the list.
    do
    {
        if (entry->next == nullptr)
            return (entry->next = new struct table_entry_struct(board));

        entry = entry->next;
    } while (
        board->bits[WHITE] != entry->board->bits[WHITE] ||
        board->bits[BLACK] != entry->board->bits[BLACK]
        );

    return entry;
}


// <--------------------------------------------------------------------------->


void get_moves(Board board, Side side, Movelist movelist)
{
    uint64_t moves = all_moves(board->bits[side], board->bits[!side]);

    if (moves)
    {
        uint64_t *movepointer = movelist->moves;

        do
            *movepointer++ = moves & -moves; // get last 1 in the move bitboard
        while ((moves &= moves - 1)); // set last 1 in the move bitboard to a 0

        movelist->num_moves = movepointer - movelist->moves;
    }

    else
        movelist->num_moves = 0;
}

void sort_moves(Movelist movelist, TableEntry entry)
{
    for (size_t i = 0; i < movelist->num_moves; i++)
    {
        if (movelist->moves[i] == entry->best_move)
        {
            movelist->moves[i] = movelist->moves[0];
            movelist->moves[0] = entry->best_move;
            break;
        }
        else if (movelist->moves[i] == entry->second_best_move)
        {
            movelist->moves[i] = movelist->moves[1];
            movelist->moves[1] = entry->second_best_move;
            break;
        }
        else if (movelist->moves[i] == entry->third_best_move)
        {
            movelist->moves[i] = movelist->moves[2];
            movelist->moves[2] = entry->third_best_move;
            break;
        }
    }
}


// <--------------------------------------------------------------------------->


void set_board(char *data, Board board)
{
    board->bits[WHITE] = 0;
    board->bits[BLACK] = 0;

    for (int i = 0; i < 64; i++)
    {
        if (data[i] == 'w')
            board->bits[WHITE] |= new_stone(i % 8, i / 8);

        else if (data[i] == 'b')
            board->bits[BLACK] |= new_stone(i % 8, i / 8);
    }
}

void print_board(Board board)
{
    uint64_t whites = board->bits[WHITE],
    blacks = board->bits[BLACK];

    for (int i = 0; i < 8; i++)
    {
        cerr << "---------------" << endl;
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

    cerr << "---------------" << endl;
}
