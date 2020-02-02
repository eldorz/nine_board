/*
 * State.h
 *
 *  Created on: 07/05/2014
 *      Author: Laughlin Dawes 3106483
 */

#ifndef STATE_H_
#define STATE_H_

#include <vector>
#include "SubBoard.h"

class State {
	friend std::ostream &operator<<(std::ostream&, const State&);
	friend std::hash<State>;
	friend bool operator==(const State &, const State &);
	friend bool operator!=(const State &, const State &);

public:
	State() : state(9) { }
	void update(int board, int pos, int val);
	int query(int board, int pos);
	void clear();
	std::vector<int> available (int board) const;
	int utility(int player, int depth) const;

	int getCurrSub() const { return currSub; }
	int getCurrPlayer() const { return currPlayer; }
	int getValue() const { return value; }
	int getValueType() const { return value_type; }
	int getRelDepth() const { return relDepth; }

	void setCurrSub(int s) { currSub = s; }
	void setCurrPlayer(int p) { currPlayer = p; }
	void setValue(int v) const { value = v; }
	void setValueType(int vt) const { value_type = vt; }
	void setRelDepth(int d) const { relDepth = d; }

	void makeMove(int);

	enum {NO_VALUE, EXACT, UPPER_BOUND, LOWER_BOUND}; // value types for t-table

private:
	std::vector<SubBoard> state;
	int currSub = 0; // current sub-board of play
	mutable int util = 0; // record the utility of this state
	int currPlayer = 0; // current player for this state
	mutable bool evaluated = false;
	void evaluate() const;

	// transposition table variables
	mutable int value = 0;
	mutable int value_type = NO_VALUE; // see public enum above for types
	mutable int relDepth = 0; // height of node above depth cutoff
};

// declare non-member functions
std::ostream &operator<<(std::ostream&, const State&);
bool operator==(const State &, const State &);
bool operator!=(const State &, const State &);

// hash_combine borrowed from Boost library; using just EOR gave collisions
inline void hash_combine(size_t &seed, int v) {
	seed ^= std::hash<int>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

/* Specialise the hash function; a state is uniquely identified by the board,
 * the current sub-board of play and the current player. Other variables, such
 * as those used by the transposition table, are not identifiers of a unique
 * state and are expressly not included.
 */
namespace std {
template <> struct hash<State> {
	std::size_t operator()(State const& s) const {
		std::size_t h = 0;
		for (int i = 0; i != 9; ++i) {
			hash_combine(h, s.state[i].getBoard());
		}
		hash_combine(h, s.currSub);
		hash_combine(h, s.currPlayer);
		return h;
	}
};
}

#endif /* STATE_H_ */
