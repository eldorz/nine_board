/*
 * GameEngine.h
 *
 *  Created on: 09/05/2014
 *      Author: Laughlin Dawes 3106483
 */

#ifndef GAMEENGINE_H_
#define GAMEENGINE_H_

#include <algorithm>
#include <ctime>
#include <iostream>
#include <limits>
#include <random>
#include <unordered_set>
#include <vector>

#include "State.h"


struct move_val_t {
	int move;
	int val;

	bool operator<(const move_val_t &rhs) const {
		return val > rhs.val; // note will reverse sort on val
	}
};

class GameEngine {
public:
	GameEngine() {
		generator.seed(clock());
	}
	void setPlayer(int);
	void setMove(int m) { move = m; }
	void setCurrSub(int s) { currState.setCurrSub(s); }

	int getOpponent() { return opponent; }
	int getPlayer() { return player; }
	int getMove() { return move; }

	void reset();
	void update(int board, int pos, int val) {
		currState.setCurrSub(board);
		currState.setCurrPlayer(val);
		currState.makeMove(pos);
	}
	std::vector<int> available(int board) { return currState.available(board); }

	int randomMove();
	int iterDeepSearch(int startDepth);
	int alphaBetaSearch(State &state, int depth);
	int alphaBetaSearch(int depth);
	int maxValue(State &s, int alpha, int beta, int depth);
	int minValue(State &s, int alpha, int beta, int depth);
	int cutoffTest(State &s, int depth);

	void printState();

private:
	State currState;
	int player = 0;
	int opponent = 0;
	int move = 0;
	int cutOffDepth = 0;
	std::default_random_engine generator;
	std::unordered_set<State> ttable; // this is a hash table

	/* experimentally, each level of tree takes 3x longer than sum of
	 * all previous levels. With a bit of tweaking, 1000ms seems a good time cut
	 * to give an average time per move of around 2s.
	 */
	enum { DEEP_TIME_CUT = 100, HARD_DEPTH_LIMIT = 20,
		MOVE_ORDER_DEPTH_LIMIT = 7 };

	void moveOrder(const State &, int depth, std::vector<move_val_t> &);
};


#endif /* GAMEENGINE_H_ */
