/*
 * GameEngine.cpp
 *
 *  Created on: 09/05/2014
 *      Author: Laughlin Dawes 3106483
 *
 *  This class manages the persistent data required for a game and
 *  offers several algorithms for play including random move, alpha-beta
 *  search and iterative deepening search.
 */

#include "GameEngine.h"

// reset GameEngine state
void GameEngine::reset() {
	currState.clear();
	player = 0;
	opponent = 0;
	move = 0;
	cutOffDepth = 0;
	ttable.clear();
}

// set up the game engine with the given player, must be called before starting
void GameEngine::setPlayer(int this_player) {
	// set player X or O
	if (this_player == 0) {
		player = SubBoard::X_MARK;
		opponent = SubBoard::O_MARK;
	} else {
		player = SubBoard::O_MARK;
		opponent = SubBoard::X_MARK;
	}
}

// take a random move (not used)
int GameEngine::randomMove() {
	// get list of available moves in my sub-board
	std::vector<int> avail = available(currState.getCurrSub());

	// choose a random move
	std::uniform_int_distribution<int> distribution(0,avail.size()-1);
	move = avail[distribution(generator)];

	// update board and return move
	currState.update(currState.getCurrSub(), move, player);

	return move;
}

// perform iterative deepening minimax search with alpha-beta pruning, starting
// at depth depth, and ceasing based on elapsed time
int GameEngine::iterDeepSearch(int depth) {
	unsigned long start = clock();
	unsigned long current = clock();
	unsigned long clocks_per_ms = (unsigned long)CLOCKS_PER_SEC / 1000ul;

	// set up for move
	move = 0;
	ttable.clear();

	// perform alpha-beta searches with increasing depth
	while (((current - start) / clocks_per_ms < DEEP_TIME_CUT) &&
			depth <= HARD_DEPTH_LIMIT) {
		State temp = currState;
		move = alphaBetaSearch(temp, depth++);
		current = clock();
	}

	// update current state with the last calculated move and return it
	currState.makeMove(move);
	return move;
}

// perform minimax search with alpha-beta pruning, limited to depth 'depth'
int GameEngine::alphaBetaSearch(State &state, int depth) {
	using std::vector;
	using std::max;

	// set cut-off depth and current player
	cutOffDepth = depth;
	state.setCurrPlayer(player);

	// get ordered list of available moves
	vector<move_val_t> moves;
	moveOrder(state, 0, moves);

	// get value of each move
	int v = 0;
	int alpha = std::numeric_limits<int>::min();
	int beta = std::numeric_limits<int>::max();
	int vMax = std::numeric_limits<int>::min();
	vector<int> move_values;
	for (move_val_t mv : moves) {
		State nextState = state;
		nextState.makeMove(mv.move);

		// pass control to MIN
		v = minValue(nextState, alpha, beta, 1);

		// keep note of best move and move values
		vMax = max(vMax, v);
		move_values.push_back(v);

		// update alpha
		alpha = max(alpha, v);
	}

	// vMax is now the value of the best move, choose the move with this value
	auto iter = std::find(move_values.begin(), move_values.end(), vMax);
	move = moves[std::distance(move_values.begin(),iter)].move;

	// update board and return move
	state.makeMove(move);

	return move;
}

// perform minimax with alpha-beta pruning on current state to depth 'depth'
int GameEngine::alphaBetaSearch(int depth) {
	return alphaBetaSearch(currState, depth);
}

// the MAX side of minimax
int GameEngine::maxValue(State &state, int alpha, int beta, int depth) {
	using std::vector;
	using std::max;
	using std::min;

	// see if an entry for this state exists in transposition table
	auto iter = ttable.find(state);
	if (iter != ttable.end()) {
		// if current relative depth <= stored state relative depth we can use
		if (cutOffDepth - depth <= iter->getRelDepth()) {
			int valType = iter->getValueType();
			// if the state has an exact value, return it
			if (valType == State::EXACT) {
				return iter->getValue();
			}
			// if the state has an upper bound, use it to set beta
			if (valType == State::UPPER_BOUND) {
				beta = min(iter->getValue(), beta);
			}
			// if the state has a lower bound, use it to set alpha
			if (valType == State::LOWER_BOUND) {
				alpha = max(iter->getValue(), alpha);
			}
		}
	}

	// check if a terminal condition exists
	if (cutoffTest(state,depth)) {
		// we have an exact value for the transposition table
		if (iter != ttable.end()) {
			// state already exists in table, update it
			iter->setValue(iter->utility(player, depth));
			iter->setValueType(State::EXACT);
			iter->setRelDepth(cutOffDepth - depth);
			return iter->getValue();
		} else {
			// state is new, add to table
			state.setValue(state.utility(player, depth));
			state.setValueType(State::EXACT);
			state.setRelDepth(cutOffDepth - depth);
			ttable.insert(state);
			return state.getValue();
		}
	}

	// get an ordered list of available moves
	vector<move_val_t> moves;
	moveOrder(state, depth, moves);

	// find best value for available moves
	int v = std::numeric_limits<int>::min();
	for (move_val_t mv : moves) {
		State nextState = state;
		nextState.makeMove(mv.move);

		// pass control to MIN
		v = max(v, minValue(nextState, alpha, beta, depth + 1));

		// prune if v greater than beta
		if (v >= beta) break;

		// update alpha
		alpha = max(alpha, v);
	}

	// we now have an upper bound on v, update t-table
	if (iter != ttable.end()) {
		iter->setValue(v);
		iter->setValueType(State::UPPER_BOUND);
		iter->setRelDepth(cutOffDepth - depth);
	} else {
		state.setValue(v);
		state.setValueType(State::UPPER_BOUND);
		state.setRelDepth(cutOffDepth - depth);
		ttable.insert(state);
	}

	return v;
}

// the MIN side of minimax
int GameEngine::minValue(State &state, int alpha, int beta, int depth) {
	using std::vector;
	using std::min;
	using std::max;

	// see if an entry for this state exists in transposition table
	auto iter = ttable.find(state);
	if (iter != ttable.end()) {
		// if current relative depth <= stored state relative depth we can use
		if (cutOffDepth - depth <= iter->getRelDepth()) {
			int valType = iter->getValueType();
			// if the state has an exact value, return it
			if (valType == State::EXACT) {
				return iter->getValue();
			}
			// if the state has an upper bound, use it to set beta
			if (valType == State::UPPER_BOUND) {
				beta = min(iter->getValue(), beta);
			}
			// if the state has a lower bound, use it to set alpha
			if (valType == State::LOWER_BOUND) {
				alpha = max(iter->getValue(), alpha);
			}
		}
	}

	if (cutoffTest(state,depth)) {
		// we have an exact value for the transposition table
		if (iter != ttable.end()) {
			iter->setValue(iter->utility(player, depth));
			iter->setValueType(State::EXACT);
			iter->setRelDepth(cutOffDepth - depth);
			return iter->getValue();
		} else {
			state.setValue(state.utility(player, depth));
			state.setValueType(State::EXACT);
			state.setRelDepth(cutOffDepth - depth);
			ttable.insert(state);
			return state.getValue();
		}
	}

	int v = std::numeric_limits<int>::max();

	// determine the minimum attainable value given available moves

	// get an ordered list of available moves
	vector<move_val_t> moves;
	moveOrder(state, depth, moves);

	for (move_val_t mv : moves) {
		State nextState = state;
		nextState.makeMove(mv.move);

		v = min(v, maxValue(nextState, alpha, beta, depth + 1));

		if (v <= alpha) break;

		beta = min(beta, v);
	}

	// we now have a lower bound on v, update t-table
	if (iter != ttable.end()) {
		iter->setValue(v);
		iter->setValueType(State::LOWER_BOUND);
		iter->setRelDepth(cutOffDepth - depth);
	} else {
		state.setValue(v);
		state.setValueType(State::LOWER_BOUND);
		state.setRelDepth(cutOffDepth - depth);
		ttable.insert(state);
	}

	return v;
}

// termination test for minimax search
int GameEngine::cutoffTest(State &state, int depth) {
	if (depth == cutOffDepth) return true;

	// see if state is in ttable
	auto iter = ttable.find(state);
	if (iter != ttable.end()) {
		if (iter->utility(player,depth) >= SubBoard::WIN - HARD_DEPTH_LIMIT ||
				iter->utility(player, depth) <=
				SubBoard::LOSS + HARD_DEPTH_LIMIT)
			return true;
	}
	else {
		// state is not in ttable
		if (state.utility(player, depth) >= SubBoard::WIN - HARD_DEPTH_LIMIT ||
				state.utility(player, depth) <=
				SubBoard::LOSS + HARD_DEPTH_LIMIT) {
			// add state to table for future reference
			ttable.insert(state);
			return true;
		}
	}
	return false;
}

/* accepts a State, a current depth, and a reference to a vector of move_val_t.
 * returns a vector of move_val_t, reverse sorted on value.
 */
void GameEngine::moveOrder(const State &s, int depth,
		std::vector<move_val_t> &moveVals) {
	using std::vector;
	using std::min;
	using std::max;

	vector<int> avail = s.available(s.getCurrSub());
	moveVals.clear();

	for (int move : avail) {
		State nextState = s;

		// make the move
		nextState.makeMove(move);
		move_val_t thisMoveVal{move, 0};

		/* move ordering reduces in value with depth, just return available
		 * moves. */
		if (depth >= MOVE_ORDER_DEPTH_LIMIT) {
			moveVals.push_back(thisMoveVal);
			continue;
		}

		// check if state has been found before, if so use its value
		auto iter = ttable.find(nextState);
		if (iter != ttable.end()) {
			int valType = iter->getValueType();
			if (valType == State::EXACT) {
				// use the previously determined value
				thisMoveVal.val = iter->getValue();
			}
			else if (valType == State::UPPER_BOUND) {
				// use 0 if within bound, otherwise use bound
				thisMoveVal.val = min(0,
						iter->getValue());
			}
			else if (valType == State::LOWER_BOUND) {
				// use 0 if within bound, otherwise use bound
				thisMoveVal.val = max(0,
						iter->getValue());
			}
			else {
				// use 0
				thisMoveVal.val = 0;
			}
		}
		else {
			// state has not been found before, use 0
			thisMoveVal.val = 0;
		}
		// keep the moves and values in a vector
		moveVals.push_back(thisMoveVal);
	}

	// reverse sort available moves by value (see state_move_t definition)
	if (depth < MOVE_ORDER_DEPTH_LIMIT) {
		std::sort(moveVals.begin(), moveVals.end());
	}
}

// print the state, not used
void GameEngine::printState() {
	std::cout << currState << std::endl;
}
