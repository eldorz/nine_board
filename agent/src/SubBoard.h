/*
 * SubBoard.h
 *
 * This class holds a sub-board in a single long int using bitwise
 * representation, two bits per sub-board square. There may be a speed penalty
 * here, but I want to save on space as I plan to remember board states between
 * iterations to help with minimax ordering.
 *
 *  Created on: 06/05/2014
 *      Author: Laughlin Dawes 3106483
 */
#include <iostream>
#include <vector>

#ifndef SUBBOARD_H_
#define SUBBOARD_H_

class SubBoard {
friend std::ostream &operator<<(std::ostream&, const SubBoard&);

public:
	void update(const int pos, const int val);
	int query(const int pos);
	bool poscheck(const int pos, const int val);
	void clear();
	std::vector<int> available() const;
	int evaluate() const;
	unsigned long getBoard() const { return the_board; }

	enum { BLANK = 0, X_MARK = 1, O_MARK = 2 };
	enum { WIN = 1000, LOSS = -1000 };

private:
	unsigned long int the_board = 0; //guaranteed to be at least 32 bit

	// predetermined masks for rows, columns and diagonals
	enum { MASK_ROW_1 = 0x3F, MASK_ROW_2 = 0xFC0, MASK_ROW_3 = 0x3F000,
		MASK_COL_1 = 0x30C3, MASK_COL_2 = 0xC30C, MASK_COL_3 = 0x30C30,
		MASK_DIAG_DOWN = 0x30303, MASK_DIAG_UP = 0x3330 };

	// predetermined values for triplets, NB depends on X_MARK = 1, O_MARK = 2
	enum { TRIPLET_ROW_X = 0x15, TRIPLET_ROW_O = 0x2A,
		TRIPLET_COL_X = 0x1041, TRIPLET_COL_O = 0x2082,
		TRIPLET_DIAG_DOWN_X = 0x10101, TRIPLET_DIAG_DOWN_O = 0x20202,
		TRIPLET_DIAG_UP_X = 0x1110, TRIPLET_DIAG_UP_O = 0x2220 };

	// values of doublets and singlets - a bit of experimenting here
	enum { DOUBLET_VAL = 10, SINGLET_VAL = 1 };

	int rowUt() const;
	int colUt() const;
	int diagUt() const;
	int rowUtCalc(long unsigned int) const;
	int colUtCalc(long unsigned int) const;
};

#endif /* SUBBOARD_H_ */
