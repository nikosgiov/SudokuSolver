# SudokuSolverGenerator

# Instructions
To compile : make<br />
To clean compilation files : make clean<br />
To execute : <br />
Print the given puzzle and determine if it has a unique solution,multiple or none,giving a corresponding solution : <strong>./sudoku < yoursudoku/puzzle</strong><br />
Print the given puzzle and determine if the puzzle is a valid one : <strong>./sudoku -c < yoursudoku/puzzle</strong><br />
Generate a new puzzle with X empty cells(zero) and print it.Use -u flag if you want unique solution : <strong>./sudoku -g X -u</strong><br />

# Tests
In puzzles folder you can find some premade puzzles that cover most of the possible outcomes of a puzzle.

# Implementation
grid.h contains the structure used to store the grid and additional data used during the solving of the grid. It defines a specific type Grid_T and a set of extremely simple functions used to access (read or write) the data of a Grid_T object.
The algorithm for the sudoku solver is :
1.)For each empty grid position we form the possible choices that include all numbers that do not repeat in the same row, column, or sub-grid.
2.)We choose an empty grid position and one of the possible options to fill it. If the puzzle has a unique solution, then there should be at least one puzzle position that has exactly one possible solution/option at a time. If the puzzle has more than one solution, then there may not be a position with a unique option.
3.)We remove our selection from the selected item.
4.)There are two cases:
a.)If the puzzle has at least one item with a unique choice at each step, then our choice is "safe" and we will not have to "try" other choices, and therefore we remove our choice from the possible choices of all other positions in the puzzle.
b.)If not, the previous option is not unique and therefore may not lead to a solution. So, we should only apply it temporarily, so that if it does not lead to a solution, we can try the next possible option. This technique is called backtracking and guarantees that we will find the solution, if there is one. To implement backtracking we use a copy (helper) grid, which we initialize based on g and which we "solve" recursively using sudoku_solve(). If this auxiliary grid has a solution then we return it as the result of the current call to solve_grid() otherwise we "throw it away" and return the original g (having already removed the option we tried so as not to have infinite backtracking).
5.)Repeat until the puzzle is completed correctly.