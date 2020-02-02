/*
 * SubBoard.cpp
 *
 *  Created on: 06/05/2014
 *      Author: Laughlin Dawes 3106483
 */

#include "SubBoard.h"

/* insert an X or an O into position pos (NB safe only if pos is blank). We
 * only ever need to add X or O, never remove or change them, so we can use
 * bitwise OR here.
 */
void SubBoard::update(const int pos, const int val) {
	unsigned long int bitVal = val;
	bitVal = bitVal << 2 * (pos - 1);
	the_board = the_board | bitVal;
}

// clear the subboard
void SubBoard::clear() {
	the_board = 0;
}

// give the contents of position pos
int SubBoard::query(const int pos) {
	unsigned long int mask = 3; // 11
	mask = mask << 2 * (pos - 1);
	mask = the_board & mask;
	return mask >> 2 * (pos - 1);
}

// return true if the contents of position pos equals val
bool SubBoard::poscheck(const int pos, const int val) {
	return query(pos) == val;
}

// return all the available (blank) positions as a vector of position numbers
std::vector<int> SubBoard::available() const {
	long unsigned int copy = the_board;
	long unsigned int mask = 3;
	std::vector<int> result;
	for (int i = 1; i != 10; ++i) {
		if ((copy & mask) == 0) result.push_back(i);
		copy = copy >> 2;
	}
	return result;
}

// return the utility of this board state, assuming player is X
int SubBoard::evaluate() const {
	// determine if win or loss
	// check rows
	unsigned long int masked = the_board & MASK_ROW_1;
	if (masked == TRIPLET_ROW_X) return WIN;
	if (masked == TRIPLET_ROW_O) return LOSS;

	masked = (the_board & MASK_ROW_2) >> 6;
	if (masked == TRIPLET_ROW_X) return WIN;
	if (masked == TRIPLET_ROW_O) return LOSS;

	masked = (the_board & MASK_ROW_3) >> 12;
	if (masked == TRIPLET_ROW_X) { return WIN; }
	if (masked == TRIPLET_ROW_O) { return LOSS; }

	// check columns
	masked = the_board & MASK_COL_1;
	if (masked == TRIPLET_COL_X) { return WIN; }
	else if (masked == TRIPLET_COL_O) { return LOSS; }

	masked = (the_board & MASK_COL_2) >> 2;
	if (masked == TRIPLET_COL_X) { return WIN; }
	else if (masked == TRIPLET_COL_O) { return LOSS; }

	masked = (the_board & MASK_COL_3) >> 4;
	if (masked == TRIPLET_COL_X) { return WIN; }
	else if (masked == TRIPLET_COL_O) { return LOSS; }

	// check diagonals
	masked = the_board & MASK_DIAG_DOWN;
	if (masked == TRIPLET_DIAG_DOWN_X) { return WIN; }
	else if (masked == TRIPLET_DIAG_DOWN_O) { return LOSS; }

	masked = the_board & MASK_DIAG_UP;
	if (masked == TRIPLET_DIAG_UP_X) { return WIN; }
	else if (masked == TRIPLET_DIAG_UP_O) { return LOSS; }

	// generate utilities for rows, columns & diagonals
	int ut = 0;
	ut += rowUt();
	ut += colUt();
	ut += diagUt();

	return ut;
}

int SubBoard::rowUt() const {
	int ut = 0;

	long unsigned int masked = the_board & MASK_ROW_1;
	ut += rowUtCalc(masked);

	masked = (the_board & MASK_ROW_2) >> 6;
	ut += rowUtCalc(masked);

	masked = (the_board & MASK_ROW_3) >> 12;
	ut += rowUtCalc(masked);

	return ut;
}

int SubBoard::colUt() const {
	int ut = 0;

	long unsigned int masked = the_board & MASK_COL_1;
	ut += colUtCalc(masked);

	masked = (the_board & MASK_COL_2) >> 2;
	ut += colUtCalc(masked);

	masked = (the_board & MASK_COL_3) >> 4;
	ut += colUtCalc(masked);

	return ut;
}

int SubBoard::diagUt() const {
	int x = 0, o = 0, total = 0;
	long unsigned int masked = 0, player = 0;

	masked = the_board & MASK_DIAG_UP;
	masked = masked >> 4;
	for (int i = 0; i != 3; ++i) {
		player = masked & 3;
		if (player == X_MARK) ++x;
		if (player == O_MARK) ++o;
		masked = masked >> 4;
	}
	if (x == 1 && o == 0) total += SINGLET_VAL;
	else if (o == 1 && x == 0) total -= SINGLET_VAL;
	else if (x == 2 && o == 0) total += DOUBLET_VAL;
	else if (o == 2 && x == 0) total -= DOUBLET_VAL;

	x = 0;
	o = 0;
	masked = the_board & MASK_DIAG_DOWN;
	for (int i = 0; i != 3; ++i) {
		player = masked & 3;
		if (player == X_MARK) ++x;
		if (player == O_MARK) ++o;
		masked = masked >> 8;
	}
	if (x == 1 && o == 0) total += SINGLET_VAL;
	else if (o == 1 && x == 0) total -= SINGLET_VAL;
	else if (x == 2 && o == 0) total += DOUBLET_VAL;
	else if (o == 2 && x == 0) total -= DOUBLET_VAL;

	return total;
}

int SubBoard::rowUtCalc(long unsigned int masked) const {
	int x = 0, o = 0;
	long unsigned int player;
	// count the x, o and blank in a row
	for (int i = 0; i < 3; ++i) {
		player = masked & 3;
		if (player == X_MARK) ++x;
		if (player == O_MARK) ++o;
		masked = masked >> 2;
	}
	if (x == 1 && o == 0) return SINGLET_VAL;
	if (o == 1 && x == 0) return -SINGLET_VAL;
	if (x == 2 && o == 0) return DOUBLET_VAL;
	if (o == 2 && x == 0) return -DOUBLET_VAL;

	return 0;
}

int SubBoard::colUtCalc(long unsigned int masked) const {
	int x = 0;
	int o = 0;
	long unsigned int player;
	for (int i = 0; i < 3; ++i) {
		player = masked & 3;
		if (player == X_MARK) ++x;
		if (player == O_MARK) ++o;
		masked = masked >> 6;
	}
	if (x == 1 && o == 0) return SINGLET_VAL;
	if (o == 1 && x == 0) return -SINGLET_VAL;
	if (x == 2 && o == 0) return DOUBLET_VAL;
	if (o == 2 && x == 0) return -DOUBLET_VAL;

	return 0;
}

// print the sub-board
std::ostream &operator<<(std::ostream &os, const SubBoard &subboard) {
	unsigned long int mask = 3;
	for (int i = 1; i != 10; ++ i) {
		if ((subboard.the_board & mask) == SubBoard::BLANK) {
			os << ".";
		}
		else if ((subboard.the_board & mask) >> 2 * (i - 1) == SubBoard::X_MARK) {
			os << "X";
		}
		else if ((subboard.the_board & mask) >> 2 * (i - 1 )== SubBoard::O_MARK) {
			os << "O";
		}
		if ((i % 3) == 0 && i != 9) {
			os << "\n";
		}
		mask = mask << 2;
	}
	return os;
}
