#include "player.h"
#include <iostream>
#include <stdlib.h>
#include <vector>

#define SLEN 8
#define MINSCORE -10000
#define MAXDEPTH 5

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish 
 * within 30 seconds.
 */
Player::Player(Side side) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;

    this->gameboard = new Board(); // create new board
    this->color = side; // set the player's color for later use
    // then set the other player's color
    if (side == WHITE)
    {
        this->other = BLACK;
    }
    else
    {
        this->other = WHITE;
    }
}

/*
 * Destructor for the player.
 */
Player::~Player() {
}

/*
 * Compute the next move given the opponent's last move. Your AI is
 * expected to keep track of the board on its own. If this is the first move,
 * or if the opponent passed on the last move, then opponentsMove will be NULL.
 *
 * msLeft represents the time your AI has left for the total game, in
 * milliseconds. doMove() must take no longer than msLeft, or your AI will
 * be disqualified! An msLeft value of -1 indicates no time limit.
 *
 * The move returned must be legal; if there are no valid moves for your side,
 * return NULL.
 */
Move *Player::doMove(Move *opponentsMove, int msLeft) {
    // first we want to update the board given the opponent's move
    gameboard->doMove(opponentsMove, this->other);
    if (!gameboard->hasMoves(this->color))
    {
        // if no moves are available, we need to return NULL
        return NULL;
    }
    if (testingMinimax)
    {
        return this->doTest();
    }
    std::vector<Move *> first_moves;
    for (int i = 0; i < SLEN; i++)
    {
        for (int j = 0; j < SLEN; j++)
        {
            Move *temp = new Move(i, j);
            if (gameboard->checkMove(temp, color))
            {
                first_moves.push_back(temp);
            }
        }
    }
    // once we have a set number of first moves, we apply the minimax function recursively
    int min_score = MINSCORE;
    Move *real_move = first_moves[0];
    for (unsigned int k = 0; k < first_moves.size(); k++)
    {
        int best_val = minimax(first_moves[k], MAXDEPTH, gameboard, true);
        if (best_val + this->weights(first_moves[k], best_val / 2) > min_score)
        {
            // we choose the best move of our set of first moves
            real_move = first_moves[k];
            min_score = best_val + this->weights(first_moves[k], best_val / 2);
        }
    }
    gameboard->doMove(real_move, this->color);
    return real_move;
}

/**
* Returns the minimax value for a given node. Will iterate recursively until the 
* depth is equal to 0
*
* @param move: The parent move for minimax recursion
* @param depth: The remaining plies for the minimax algorithm
* @param board: The board state before the parent move
* @param self: TRUE if the player does the parent move, FALSE if the enemy moves
*/
int Player::minimax(Move *move, int depth, Board *board, bool self)
{
    Board *new_board = board->copy();
    // now make the parent move with the appropriate color
    if (self)
    {
        new_board->doMove(move, this->color);
        // if we are at the end of the tree, simply stop here
        if (!new_board->hasMoves(this->other))
        {
            int base = new_board->count(this->color) - new_board->count(this->other);
            return base + this->weights(move, base);
        }
    }
    else
    {
        new_board->doMove(move, this->other);
        if (!new_board->hasMoves(this->color))
        {
            int base = new_board->count(this->color) - new_board->count(this->other);
            return base + this->weights(move, base);
        }
    }
    // if we have reached our final depth, return the total move value
    if (depth == 0)
    {
        int base = new_board->count(this->color) - new_board->count(this->other);
        return base + this->weights(move, base);
    }
    // otherwise, find the possible moves for the player or enemy
    std::vector<Move *> moveset;
    for (int i = 0; i < SLEN; i++)
    {
        for (int j = 0; j < SLEN; j++)
        {
            Move *temp = new Move(i, j);
            if (self)
            {
                if (new_board->checkMove(temp, this->other))
                {
                    moveset.push_back(temp);
                }
            }
            else
            {
                if (new_board->checkMove(temp, this->color))
                {
                    moveset.push_back(temp);
                }
            }
        }
    }
    // now we need to set our recursion based on who will play
    if (self)
    {
        int min_value = -MINSCORE;
        for (unsigned int k; k < moveset.size(); k++)
        {
            // keep minimaxing until we reach the max depth or end of the game
            int value = this->minimax(moveset[k], depth - 1, new_board, false);
            if (value < min_value)
            {
                // we want to minimize the loss on our turn
                min_value = value;
            }
        }
        return min_value;
    }
    else
    {
        int max_value = MINSCORE;
        for (unsigned int k; k < moveset.size(); k++)
        {
            // minimax recursively
            int value = this->minimax(moveset[k], depth - 1, new_board, true);
            if (value > max_value)
            {
                // opponent wants to maximize the loss
                max_value = value;
            }
        }
        return max_value;
    }
}

/**
* Returns a weight bonus/penalty depending on placement
*
* @param move: the move to be decided
* @param base: the base move value for scaling
*/
int Player::weights(Move *move, int base)
{
    int x = move->getX();
    int y = move->getY();
    int pos_base = abs(base);
    // Consider x near side or corner:
    if ((x == 0) || (x == SLEN - 1))
    {
        if ((y == 0) || (y == SLEN - 1))
        {
            // give a huge bonus for corner moves
            return 3 * pos_base;
        }
        else if ((y == 1) || (y == SLEN - 2))
        {
            // give a huge penalty for spaces next to corners
            return -3 * pos_base;
        }
        else
        {
            // give a bonus for side moves
            return 2 * pos_base;
        }
    }
    // now consider y near side or corner
    if ((y == 0) || (y == SLEN - 1))
    {
        if ((x == 1) || (x == SLEN - 2))
        {
            // this will result in a corner
            return -3 * pos_base;
        }
        else if ((x > 1) && (x < SLEN - 2))
        {
            // this will get a border
            return 2 * pos_base;
        }
    }
    // now consider x close to the edge
    if ((x == 1) || (x == SLEN - 2))
    {
        if ((y == 1) || (y == SLEN - 2))
        {
            // this is diagonally close to an edge
            return -3 * pos_base;
        }
    }
    return 0;
}

/*
* Simple 2-ply depth minimax for testing purposes
*/
Move *Player::doTest()
{
    std::vector<Move *> first_moves; // vector of the first ply moves.
    for (int i = 0; i < SLEN; i++)
    {
        for (int j = 0; j < SLEN; j++)
        {
            Move *temp = new Move(i, j);
            if (gameboard->checkMove(temp, this->color))
            {
                // add all available moves to the first_moves vector
                first_moves.push_back(temp);
            }
        }
    }
    // now we have a vector of possible moves. We now try to minimax our subsequent turn
    int min_score = MINSCORE;
    Move *real_move;
    for (unsigned int k = 0; k < first_moves.size(); k++)
    {
        Board *tempboard = gameboard->copy(); // generate another board for testing
        tempboard->doMove(first_moves[k], this->color);
        int branch_min = -MINSCORE;
        for (int i = 0; i < SLEN; i++)
        {
            for (int j = 0; j < SLEN; j++)
            {
                Move *enemy = new Move(i, j); // test the enemy moves
                if (tempboard->checkMove(enemy, this->other))
                {
                    // if the enemy can make a move, try it out
                    Board *enemyboard = tempboard->copy();
                    enemyboard->doMove(enemy, this->other);
                    if (enemyboard->count(this->color) - enemyboard->count(this->other) < branch_min)
                    {
                        branch_min = enemyboard->count(this->color) - enemyboard->count(this->other);
                    }
                    delete enemyboard;
                }
            }
        }
        // finally, compare the branch minimum to the total mimnum
        if (branch_min > min_score)
        {
            // if we have a higher minimum score, we want to take this as our next move
            min_score = branch_min;
            real_move = first_moves[k];
        }
        delete tempboard;
    }
    gameboard->doMove(real_move, this->color);
    return real_move;
}
