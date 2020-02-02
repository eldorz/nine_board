# nine_board

9-board tic-tac-toe player
 *
 *  This agent uses minimax search algorithm with alpha-beta pruning, iterative
 *  deepening, transposition table and move ordering to choose the next move to
 *  make when presented with a game of 9-board tic-tac-toe.
 *
 *  The fundamental data structure is a sub-board, which corresponds to a
 *  traditional tic-tac-toe board. This is represented in bitwise fashion, with
 *  each of the 9 board positions (blank, X or O) represented by 2 bits. This
 *  allows the entire board to be represented in 18 bits. As a long int is
 *  guaranteed to be at least 32 bits, this is sufficient to represent any
 *  state of a sub-board.
 *
 *  The state of all 9 sub-boards is represented as a vector of 9 sub-boards.
 *  The memory required for this on a typical machine is 9 x 32 bits. If the
 *  subboards were represented by one short int per position, this would be
 *  9 x 9 x 16 bits - four-and-a-half times more memory.
 *
 *  Given that minimax is a fundamentally depth-first search algorithm, it
 *  might seem wrong to worry about memory usage. However many optimisations
 *  such as transposition tables or deep move ordering require storing states
 *  and can quickly consume large amounts of memory. Therefore I felt it prudent
 *  to begin with a compact representation.
 *
 *  Speed is important in any adversarial algorithm. One function which will be
 *  called very frequently is the evaluation function which decides the utility
 *  of any state. I have tried to optimise this using bitwise operations with
 *  predetermined comparators.
 *
 *  Move ordering is critical in alpha-beta pruning. The most important move to
 *  order is the first one, and the most value is obtained by ordering the upper
 *  levels of the tree. A transposition table is used to implement move ordering
 *  and also to improve efficiency of minimax. Previously seen states are
 *  recorded along with their calculated value, the value type (exact or a
 *  bound) and the height of the node in the search tree at the time of
 *  evaluation. The table uses std::unordered_set, which is essentially a hash
 *  table. The built-in hash function was specialised for the task.
 *
 *  Another important decision in any minimax algorithm is the heuristic to use.
 *  I used a combination of doublets (rows, columns or diagonals where the
 *  player has 2 positions and the opponent has none) and singlets (where the
 *  player has only 1 position, the opponent none). After some experimentation,
 *  the doublets are weighted 10 and the singlets 1.
 *
 *  To perform a valid move within the alloted 2 sec on average, iterative
 *  deepening was used, terminating if the sum of all previous levels took
 *  longer than 1 sec. This was chosen experimentally, with moves averaging
 *  around 2 sec.
 *
 *  Iterative deepening allowed move ordering using the stored values of states
 *  from previous iterations. States were not stored between moves.
 *
 *  I believe this to be a competitive algorithm, typically achieving a search
 *  depth of 8 or 9 at the beginning of a game, and improving as the game
 *  progresses. If further competitiveness were required I might consider
 *  using machine-learning methods to tune the heuristic weights, and possibly
 *  adding further heuristic features.
 */

