#include "player.hpp"
#include <iostream>

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish
 * within 30 seconds.
 */
Player::Player(Side side) {
    this->side = side;

    board = board_stack;
    movelist = movelist_stack;

    set_bits(board, 0x0000001008000000, 0x0000000810000000);

    table_size = MAX_TABLE_SIZE;
    bool unsuccessful_alloc = true;

    while (unsuccessful_alloc)
    {
        try {
            table = new struct table_entry_struct[table_size];
            unsuccessful_alloc = false;
        }

        catch (const bad_alloc&) {
            table_size >>= 2;
        }
    }
}

/*
 * Destructor for the player.
 */
Player::~Player() {
    for (size_t i = 0; i < table_size; i++)
        if (table[i].next)
            delete table[i].next;

    delete[] table;
}

/*
 * Compute the next move given the opponent's last move. Your AI is
 * expected to keep track of the board on its own. If this is the first move,
 * or if the opponent passed on the last move, then opponentsMove will be
 * nullptr.
 *
 * msLeft represents the time your AI has left for the total game, in
 * milliseconds. doMove() must take no longer than msLeft, or your AI will
 * be disqualified! An msLeft value of -1 indicates no time limit.
 *
 * The move returned must be legal; if there are no valid moves for your side,
 * return nullptr.
 */
Move *Player::doMove(Move *opponentsMove, int msLeft) {
    if (opponentsMove)
        add_stone(board, !side, new_stone(opponentsMove->x, opponentsMove->y));

    get_moves(board, side, movelist);

    if (movelist->num_moves == 0)
        return nullptr;

    TableEntry entry = get_entry(board, table, table_size);
    if (entry->in_use)
        sort_moves(movelist, entry);

    uint64_t best_move = 0,
    move;

    int32_t alpha = INT32_MIN,
    move_score;

    for (size_t i = 0; i < movelist->num_moves; i++)
    {
        move = get_move(movelist, i);
        add_stone_copy(board, side, move);

        // Run negascout for the first move with a full search interval.
        if (i == 0)
            move_score = -negascout(
                board + 1, movelist + 1, !side,
                INT32_MIN, INT32_MAX, MAXDEPTH
                );

        // Run negascout for subsequent moves with an empty search interval. If
        // the move score with an empty search interval is above the max-
        // player's highest score and below the min-player's lowest score, rerun
        // negascout with a full search interval.
        else
        {
            move_score = -negascout(
                board + 1, movelist + 1, !side,
                -alpha - 1, -alpha, MAXDEPTH
                );

            if (move_score > alpha)
                move_score = -negascout(
                    board + 1, movelist + 1, !side,
                    INT32_MIN, -move_score, MAXDEPTH
                    );
        }

        if (move_score > alpha)
        {
            alpha = move_score;
            best_move = move;
        }
    }

    add_stone(board, side, best_move);
    uint8_t best_move_position = stone_position(best_move);
    return new Move(best_move_position % 8, best_move_position / 8);
}

int32_t Player::negascout(
    Board cur_board, Movelist cur_movelist, Side cur_side,
    int32_t alpha, int32_t beta, uint8_t depth
    )
{
    TableEntry entry = get_entry(cur_board, table, table_size);
    if (entry->in_use && entry->depth == depth)
        return entry->score;

    if (depth == 0)
        return (entry->score = (cur_side == side) ?
        heuristic(cur_board) : -heuristic(cur_board));

    get_moves(cur_board, cur_side, cur_movelist);

    if (cur_movelist->num_moves == 0)
        return (entry->score = (cur_side == side) ?
        heuristic(cur_board) : -heuristic(cur_board));

    if (entry->in_use)
        sort_moves(movelist, entry);

    uint64_t best_move = 0, second_best_move = 0, third_best_move = 0,
    move;

    int32_t move_score;

    for (size_t i = 0; i < cur_movelist->num_moves; i++)
    {
        move = get_move(cur_movelist, i);
        add_stone_copy(cur_board, cur_side, move);

        // Run negascout for the first move with a full search interval.
        if (i == 0)
            move_score = -negascout(
                cur_board + 1, cur_movelist + 1, !cur_side,
                -beta, -alpha, depth - 1
                );

        // Run negascout for subsequent moves with an empty search interval. If
        // the move score with an empty search interval is above the max-
        // player's highest score and below the min-player's lowest score, rerun
        // negascout with a full search interval.
        else
        {
            move_score = -negascout(
                cur_board + 1, cur_movelist + 1, !cur_side,
                -alpha - 1, -alpha, depth - 1
                );

            if (move_score > alpha && move_score < beta)
                move_score = -negascout(
                    cur_board + 1, cur_movelist + 1, !cur_side,
                    -beta, -move_score, depth - 1
                    );
        }

        if (move_score > alpha)
        {
            alpha = move_score;
            third_best_move = second_best_move;
            second_best_move = best_move;
            best_move = move;

            // If the lower bound has reached the upper bound, the remaining
            // moves can be ignored.
            if (alpha >= beta)
                break;
        }
    }

    if (alpha < beta && beta - alpha > 1)
    {
        entry->in_use = true;
        entry->depth = depth;
        entry->score = alpha;
    }

    entry->best_move = best_move;
    entry->second_best_move = second_best_move;
    entry->third_best_move = third_best_move;

    return alpha;
}

const int32_t STONEIMB_MULT_CHANGE  =
(STONEIMB_MULT_END - STONEIMB_MULT_START) / 60;
const int32_t MOBILITY_MULT_CHANGE  = -MOBILITY_MULT_START  / 60;
const int32_t PMOBILITY_MULT_CHANGE = -PMOBILITY_MULT_START / 60;
const int32_t CORNERS_MULT_CHANGE   = -CORNERS_MULT_START   / 60;
const int32_t PCORNERS_MULT_CHANGE  = -PCORNERS_MULT_START  / 60;
const int32_t SAFETY_MULT_CHANGE    = -SAFETY_MULT_START    / 60;

// Calculates the score for a given board.
int32_t Player::heuristic(Board cur_board)
{
    uint64_t this_stones    = get_stones(cur_board, side),
    other_stones            = get_stones(cur_board, !side);

    uint8_t num_this_stones = num_ones(this_stones),
    num_other_stones        = num_ones(other_stones),
    turn                    = num_this_stones + num_other_stones - 4;

    // On the last turn, the only part of the heuristic that matters is the
    // stone imbalance, so all further calculations can be avoided.
    if (turn == 60)
        return STONEIMB_MULT_END *
        (num_this_stones - num_other_stones) /
        (num_this_stones + num_other_stones);

    uint64_t this_moves     = move_bitboard(this_stones, other_stones),
    other_moves             = move_bitboard(other_stones, this_stones);

    uint8_t num_this_moves  = num_ones(this_moves),
    num_other_moves         = num_ones(other_moves),
    num_this_spaces         = num_spaces(this_stones, other_stones),
    num_other_spaces        = num_spaces(other_stones, this_stones),
    num_this_corners        = num_corners(this_stones),
    num_other_corners       = num_corners(other_stones),
    num_this_corner_moves   = num_corners(this_moves),
    num_other_corner_moves  = num_corners(other_moves),
    num_this_safe           = (num_other_moves) ?
    num_safe(this_stones, other_stones, other_moves) : num_this_stones,
    num_other_safe          = (num_this_moves) ?
    num_safe(other_stones, this_stones, this_moves) : num_other_stones;

    // STONE IMBALANCE
    // There is no need to check whether num_this_stones + num_other_stones is
    // nonzero, since there will always be at least 4 stones on the board.
    int32_t score = (STONEIMB_MULT_START + STONEIMB_MULT_CHANGE * turn) *
    (num_this_stones - num_other_stones) /
    (num_this_stones + num_other_stones);

    // MOBILITY
    if (num_this_moves + num_other_moves)
        score += (MOBILITY_MULT_START + MOBILITY_MULT_CHANGE * turn) *
        (num_this_moves - num_other_moves) /
        (num_this_moves + num_other_moves);

    // POTENTIAL MOBILITY
    // There is no need to check whether num_other_spaces + num_this_spaces is
    // nonzero, since there will only be 0 spaces on the board on the 60th turn.
    score += (PMOBILITY_MULT_START + PMOBILITY_MULT_CHANGE * turn) *
    (num_other_spaces - num_this_spaces) /
    (num_other_spaces + num_this_spaces);

    // CORNERS
    if (num_this_corners + num_other_corners)
        score += (CORNERS_MULT_START + CORNERS_MULT_CHANGE * turn) *
        (num_this_corners - num_other_corners) /
        (num_this_corners + num_other_corners);

    // POTENTIAL CORNERS
    if (num_this_corner_moves + num_other_corner_moves)
        score += (PCORNERS_MULT_START + PCORNERS_MULT_CHANGE * turn) *
        (num_this_corner_moves - num_other_corner_moves) /
        (num_this_corner_moves + num_other_corner_moves);

    // SAFETY
    if (num_this_safe + num_other_safe)
        score += (SAFETY_MULT_START + SAFETY_MULT_CHANGE * turn) *
        (num_this_safe - num_other_safe) /
        (num_this_safe + num_other_safe);

    return score;
}
