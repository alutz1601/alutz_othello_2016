#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include "common.h"
#include "board.h"
using namespace std;

class Player {

private:
	Side color;
	Side other; // other player's color
public:
	Board *gameboard;
    Player(Side side);
    ~Player();
    
    Move *doMove(Move *opponentsMove, int msLeft);
    Move *doTest();
    int minimax(Move *move, int depth, Board *board, bool self);
    int weights(Move *move, int base);

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;
};

#endif
