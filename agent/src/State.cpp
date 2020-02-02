/*
 * State.cpp
 *
 *  Created on: 07/05/2014
 *      Author: Laughlin Dawes 3106483
 */

#include "State.h"
#include "SubBoard.h"

/* set a square on sub-board board at position pos to X or O. Safe only if the
 * square is blank. Use the enums SubBoard::X_MARK, SubBoard::O_MARK and
 * SubBoard::BLANK to pass values */
void State::update(int board, int pos, int val) {
	state[board - 1].update(pos,val);
	evaluated = false;
}

// returns the value of a square on sub-board board, position pos
int State::query(int board, int pos) {
	return state[board - 1].query(pos);
}

// clear all sub-boards
void State::clear() {
	for (int i = 0; i < 9; ++i) {
		state[i].clear();
	}
	util = 0;
	currSub = 0;
	currPlayer = 0;
	evaluated = false;
	value = 0;
	value_type = NO_VALUE;
	relDepth = 0;
}

// return a vector of blank positions in sub-board board.
std::vector<int> State::available(int board) const {
	return state[board - 1].available();
}

// return the utility of this state, given player
int State::utility(int player, int depth) const {
	if (evaluated == false) {
		evaluate();
		evaluated = true;
	}

	int retval = 0;
	if (player == SubBoard::X_MARK) {
		if (util == SubBoard::WIN) retval = util - depth;
		else if (util == SubBoard::LOSS) retval = util + depth;
		else retval = util;
	}
	else {
		if (util == SubBoard::WIN) retval = -(util - depth);
		else if (util == SubBoard::LOSS) retval = -(util + depth);
		else retval = -util;
	}

	return retval;
}

// calculate and store the utility of this state
void State::evaluate() const {
	util = 0;
	// sum the utilities of the sub-boards, or assign WIN or LOSS
	for (int i = 0; i != 9; ++i) {
		int subBoardEval = state[i].evaluate();
		if (subBoardEval == SubBoard::WIN || subBoardEval == SubBoard::LOSS) {
			util = subBoardEval;
			return;
		}
		util += subBoardEval;
	}
}

// update the state given a move
void State::makeMove(int pos) {
	// update the sub-board with the required move
	update(currSub, pos, currPlayer);
	// set the new sub-board of play
	currSub = pos;
	// swap players
	currPlayer = (currPlayer == SubBoard::X_MARK) ? SubBoard::O_MARK
					: SubBoard::X_MARK;
}

// define equality and inequality operators for State class
bool operator==(const State &a, const State &b) {
	/* a state is equal if all sub-boards are equal, the active sub-boards
	 * are equal and the active players are equal */
	for (int i = 0; i != 9; ++i) {
		if (a.state[i].getBoard() != b.state[i].getBoard()) return false;
	}
	if (a.currSub != b.currSub) return false;
	if (a.currPlayer != b.currPlayer) return false;

	return true;
}

bool operator!=(const State &a, const State &b) {
	return !(a == b);
}

// print the whole board
std::ostream &operator<<(std::ostream &os, const State &s) {
	for (int row = 0; row < 9; ++row) {
		for (int col = 0; col < 9; ++ col) {
			SubBoard sub = s.state[(row/3)*3 + col/3];
			int out = sub.query((row%3)*3 + (col%3 + 1));
			if (out == SubBoard::BLANK) {
				os << ".";
			}
			else if (out == SubBoard::X_MARK) {
				os << "X";
			}
			else if (out == SubBoard::O_MARK) {
				os << "O";
			}
			else if (out == 3) {
				os << "!"; // an unsafe update operation will do this
			}
			if (col%3 == 2) {
				os << " ";
				if (col%8 == 0) {
					os << "\n";
				}
			}
		}
		if (row%3 == 2) {
			os << "\n";
		}
	}
	os << "\nvalue: " << s.utility(SubBoard::X_MARK, 0);

	return os;
}
