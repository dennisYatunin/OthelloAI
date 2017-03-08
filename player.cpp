#include "player.hpp"
#include <iostream>

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish
 * within 30 seconds.
 */
Player::Player(Side side) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;

    set_bits(board_stack, 0x0000001008000000, 0x0000000810000000);
    this->side = side;
}

/*
 * Destructor for the player.
 */
Player::~Player() {
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
        add_stone(board_stack, !side, opponentsMove->y, opponentsMove->x);

    get_moves(board_stack, side, &(move_stack[0][0]));
    uint8_t *moves = &(move_stack[0][0]);

    if (moves[0] == 0)
        return nullptr;

    size_t best_move = 1;

    //print_board(board_stack->bits[WHITE], board_stack->bits[BLACK]);
    //cerr << "(" << (long)(board_stack->bits[WHITE]) << ", " << (long)(board_stack->bits[BLACK]) << ")" << endl;
    //for (size_t i = 1; i <= moves[0]; i++)
    //{
    //    cerr << "move " << i << " is " << moves[i] / 8 << moves[i] % 8 << endl;
    //}

    add_stone_copy(
        board_stack, side, moves[1] / 8, moves[1] % 8, board_stack + 1
        );
    int32_t best_move_score = negascout(1, MAXDEPTH, INT32_MIN, INT32_MAX, 1);
    for (size_t i = 2; i <= moves[0]; i++)
    {
        add_stone_copy(
            board_stack, side, moves[i] / 8, moves[i] % 8, board_stack + 1
            );
        int32_t move_score = negascout(1, MAXDEPTH, INT32_MIN, INT32_MAX, 1);

        if (move_score > best_move_score)
        {
            best_move = i;
            best_move_score = move_score;
        }
    }

    //for (size_t i = 1; i <= moves[0]; i++)
    //{
    //    cerr << "move " << i << " is now " << moves[i] / 8 << moves[i] % 8 << endl;
    //}
    //cerr << "best move is " << best_move << endl;

    add_stone(board_stack, side, moves[best_move] / 8, moves[best_move] % 8);
    Move *result = new Move(moves[best_move] % 8, moves[best_move] / 8);

    return result;
}

int32_t Player::negascout(
    size_t stack_pos, uint8_t depth, int32_t alpha, int32_t beta,
    int8_t multiplier
    )
{
    if (depth == 0)
        return multiplier * heuristic(stack_pos);

    Side whose_move = (multiplier == 1) ? side : !side;
    get_moves(board_stack + stack_pos, whose_move, &(move_stack[stack_pos][0]));
    uint8_t *moves = &(move_stack[stack_pos][0]);

    if (moves[0] == 0)
        return multiplier * heuristic(stack_pos);

    // Run negascout for the first move with a full-size search interval.
    add_stone_copy(
        board_stack + stack_pos, side, moves[1] / 8, moves[1] % 8,
        board_stack + stack_pos + 1
        );
    int32_t move_score = -negascout(
        stack_pos + 1, depth - 1, -beta, -alpha, -multiplier
        );

    if (move_score > alpha)
    {
        alpha = move_score;

        if (alpha >= beta)
            return alpha;
    }

    for (size_t i = 2; i <= moves[0]; i++)
    {
        // Run negascout for subsequent moves with an empty search interval.
        add_stone_copy(
            board_stack + stack_pos, side, moves[i] / 8, moves[i] % 8,
            board_stack + stack_pos + 1
            );
        move_score = -negascout(
            stack_pos + 1, depth - 1, -alpha - 1, -alpha, -multiplier
            );

        // If the move score with an empty search interval was out of bounds,
        // run negascout again with a full-size search interval.
        if (move_score > alpha && move_score < beta)
            move_score = -negascout(
                stack_pos + 1, depth - 1, -beta, -move_score, -multiplier
                );

        if (move_score > alpha)
        {
            alpha = move_score;

            // If the lower bound has reached the upper bound, the remaining
            // moves can be ignored.
            if (alpha >= beta)
                return alpha;
        }
    }

    return alpha;
}

int32_t Player::heuristic(size_t stack_pos)
{
    Board board = board_stack + stack_pos;

    uint8_t num_this_stones = num_stones(board, side),
    num_other_stones = num_stones(board, !side);

    if (is_full(board))
        return 1000 *
        (num_this_stones - num_other_stones) /
        (num_this_stones + num_other_stones);

    uint8_t num_this_moves = num_moves(board, side),
    num_other_moves = num_moves(board, !side),
    num_this_cat1 = num_cat1(board, side),
    num_other_cat1 = num_cat1(board, !side),
    num_this_cat2 = num_cat2(board, side),
    num_other_cat2 = num_cat2(board, !side),
    num_this_cat3 = num_cat3(board, side),
    num_other_cat3 = num_cat3(board, !side);

    int32_t coin_parity, mobility, cat1, cat2, cat3;

    coin_parity = 100 *
    (num_this_stones - num_other_stones) / (num_this_stones + num_other_stones);

    if (num_this_moves + num_other_moves == 0)
        mobility = 0;
    else
        mobility = 1000 *
        (num_this_moves - num_other_moves) / (num_this_moves + num_other_moves);

    if (num_this_cat1 + num_other_cat1 == 0)
        cat1 = 0;
    else
        cat1 = 1000 *
        (num_this_cat1 - num_other_cat1) / (num_this_cat1 + num_other_cat1);

    if (num_this_cat2 + num_other_cat2 == 0)
        cat2 = 0;
    else
        cat2 = -500 *
        (num_this_cat2 - num_other_cat2) / (num_this_cat2 + num_other_cat2);

    if (num_this_cat3 + num_other_cat3 == 0)
        cat3 = 0;
    else
        cat3 = 250 *
        (num_this_cat3 - num_other_cat3) / (num_this_cat3 + num_other_cat3);

    return coin_parity + mobility + cat1 + cat2 + cat3;
}
