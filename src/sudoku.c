/*	Nikolaos Giovanopoulos - CSD4613
	ASSIGNMENT 4	
	sudoku.c
*/
#include "sudoku.h"

/* Read a sudoku grid from stdin and return an object Grid_T
   initialized to these values. The input has the format:

1 2 3 4 5 6 7 8 9 
4 5 6 7 8 9 1 2 3
7 8 9 1 2 3 4 5 6
2 3 4 5 6 7 8 9 1 
5 6 7 8 9 1 2 3 4
8 9 1 2 3 4 5 6 7 
3 4 5 6 7 8 9 1 2
6 7 8 9 1 2 3 4 5 
9 1 2 3 4 5 6 7 8 

   Each number is followed by a space. Each line is terminated with
   \n. Values of 0 indicate empty grid cells.
*/
Grid_T sudoku_read(void){
    Grid_T g;
    char str[2];
    int n, count = 0, i = 0, j = 0;
    str[1] = '\0';
    while ((str[0] = getchar()) != EOF){
        if (str[0] == '\n'){                /*if newline*/
            i++;                            /*go to the next row*/
            j = 0;                          /*go to the start of column*/
            continue;                       /*next loop*/
        }
        if ( isdigit((int)str[0]) ){        /*if number*/
            n = atoi(str);                  /*string to integer*/
            if (n < 0 || n > 9){            /*check it's range*/
                fprintf(stderr, "Wrong input!\n");
                exit(1);
            }
            grid_update_value(&g, i, j, n); /*save value*/
            j++;                            /*go to the next column*/
            count++;                        /*update count*/
        }
        else if (str[0] == ' '){
            continue;
        }
    }
    if (count != 81) {                      /*check total inputs*/
        fprintf(stderr, "Wrong input!\n");
        exit(1);
    }
    return g;
}

/*Returns true if grid g contains zero.Otherwise returns false.*/
static int zero_counter(Grid_T g){
    int i, j, zeros = 0;
    for (i = 0; i < 9; i++)
        for (j = 0; j < 9; j++)
            if (!grid_read_value(g, i, j))
                zeros++;
    return zeros;
}
/*Takes a grid g and checks for sub-grid errors/conflicts found in puzzle.If puzzle is correct
returns false.Otherwise reuturns true and if argument print is true prints them.*/
static int errors(Grid_T g,int print){
    int i, j, k, m, num, err = 0;
    for (i = 0; i < 9; i++){
        for (j = 0; j < 9; j++){
            num = grid_read_value(g, i, j); /*get the value of each cell.If it's empty(0) go to the next one*/
            if (!num)
                continue;

            for (k = 0; k < 9; k++){        /*Row check*/
                if (num == grid_read_value(g, i, k) && k != j){
                    if (print) fprintf(stderr,"Error: Row Conflict in elements: [%d , %d] , [%d , %d]\n",i, j, i, m);
                    err++;
                }
                if (num == grid_read_value(g, k, j) && k != i){    /*Column check*/
                    if (print) fprintf(stderr,"Error: Column Conflict in elements: [%d , %d] , [%d , %d]\n",i, j, k, j);
                    err++;
                }
            }

            for (k = (i/3)*3; k <(i/3)*3+3; k++){
                for (m = (j/3)*3; m < (j/3)*3+3; m++){
                    if ( (((k!=i) && (m==j))||((k==i) && (m!=j))) && num == grid_read_value(g, k, m)){  /*Sub-grid check*/
                        if (print) fprintf(stderr,"Error: Sub Conflict in elements: [%d,%d] - [%d,%d]\n",i, j, k, m);
                        err++;
                    }
                }
            }
        }
    }
    return err;
}

/* Print the sudoku puzzle defined by g to stream s in the same format
   as expected by sudoku_read(). */
void sudoku_print(FILE *s, Grid_T g){
    int i, j;
    for(i = 0; i < 9; i++){
        for(j = 0; j < 9; j++){
            fprintf(s, "%d ", grid_read_value(g,i,j));
        }
        fprintf(s, "\n");
    }
}
/* Print all row, col, sub-grid errors/conflicts found in puzzle g;
   some errors may be reported more than once. */
void sudoku_print_errors(Grid_T g){
    errors(g, 1);
}
/* Return true iff puzzle g is correct. */
int sudoku_is_correct(Grid_T g){
    return ( !errors(g, 0) && !zero_counter(g) );
}
/*--------------------------------------------------*/

/*Completes the possible choices of each cell according to the values of grid g*/
static void sudoku_init_choices(Grid_T *g){
    int i, j, k, l, m;
    grid_set_unique(g);                         /*set unique solution*/
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++){                /*for each element: */
            grid_set_count(g,i,j);              /*set choices count*/ 
            grid_clear_choice(g,i,j,0);         /*clear choice : 0*/
            for (m = 1; m < 10; m++)
                grid_set_choice(g, i, j, m);    /*Set all choices */
        }
    }/*Remove the choices of each element according to the existing values of grid*/
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++){
            m = grid_read_value(*g, i, j);
            if (m){      /*If cell isn't empty*/
                for (m = 1; m < 10; m++){grid_clear_choice(g, i, j, m);}  /*clear all of its choices*/
                grid_clear_count(g, i, j);                                /*update choice count(0)*/
                continue;
            }
            /*Otherwise, the cell is empty*/
            for (k = 0; k < 9; k++){
                grid_remove_choice(g, i, j, grid_read_value(*g, k, j) ); /*Remove from the i,j element, the choices that*/
                grid_remove_choice(g, i, j, grid_read_value(*g, i, k) ); /*represent numbers that already exist in the column*/
            }                                                            /*Do the same for the rows*/
            for (k = (i/3)*3; k < (i/3)*3+3; k++){
                for (l = (j/3)*3; l < (j/3)*3+3; l++){
                    grid_remove_choice(g, i, j, grid_read_value(*g, k, l) );/*Do the same for the sub-grids*/
                }
            }
            
        }
    }
}
/*Checks the existing choices and returns a unique one if exists
or reutrns a random one if there are multiple ones.When puzzle is
complete returns 0*/
static int sudoku_try_next(Grid_T g, int *row, int *col){
    int i, j, k, n;
    int nchoice = 10;
    for (i = 0; i < 9; i++){
	    for (j = 0; j < 9; j++){
	        n = grid_read_count(g, i, j);
	        if (n > 0 && n < nchoice)   /*Finding the least number of choices*/
	            nchoice = n;
	    }
    }
    while (nchoice < 10){
	    i = rand()%9;
	    j = rand()%9;               /*Searching cells randomly to find one with "nchoice" choices*/
	    n = grid_read_count(g, i, j);
	    if (n == nchoice) {
	        for (;;){
		        k = rand()%9 + 1;   /*Produce random numbers until there is one that is valid*/
		        if (grid_choice_is_valid(g, i, j, k)){
		            *row = i;
		            *col = j;
		            return k;
		        }
	        }
	    }
    }
    return 0;   /*otherwise return 0*/
}

/*Removes from grid g in cell i,j the choice n and returns its number of choices before the removal*/
static int sudoku_update_choice(Grid_T *g, int i, int j, int n){
    int oldchoice = grid_read_count(*g, i, j);
    grid_remove_choice(g, i, j, n);
    return oldchoice;
}

/*Removes from row r, column c and corresponding sub-grid the choice n*/
static void sudoku_eliminate_choice(Grid_T *g, int r, int c, int n){
    int i, j;
    for (i = 0; i < 9; i++){
        grid_remove_choice(g, r, i, n);         /*remove choice n from the row i*/
        grid_remove_choice(g, i, c, n);         /*remove choice n from the column i*/
    }
    for (i = (r/3)*3; i < (r/3)*3+3; i++){
        for (j = (c/3)*3; j < (c/3)*3+3; j++){
            grid_remove_choice(g, i, j, n);     /*remove choice n from the sub-grid i*/
        }
    }
}

/*Starts with an empty puzzle.Produces all possible choices using sudoku_init_choices()
and replaces each empty cell with valid values using sudoku_try_next()*/
Grid_T sudoku_generate_complete(void){
    Grid_T g;
    int i,j,n,tries;
    int ready_puzzle[81] = {  1, 2, 3, 4, 5, 6, 7, 8, 9,
            	              4, 5, 6, 7, 8, 9, 1, 2, 3,
      	     	          	  7, 8, 9, 1, 2, 3, 4, 5, 6,
                          	  2, 3, 4, 5, 6, 7, 8, 9, 1,
                          	  5, 6, 7, 8, 9, 1, 2, 3, 4,
                          	  8, 9, 1, 2, 3, 4, 5, 6, 7,
                          	  3, 4, 5, 6, 7, 8, 9, 1, 2,
                          	  6, 7, 8, 9, 1, 2, 3, 4, 5,
                          	  9, 1, 2, 3, 4, 5, 6, 7, 8 };

    for (tries = 10; tries!=0; tries--){                        /*10 tries*/
        for(i=0;i<9;i++)
            for(j=0;j<9;j++){grid_update_value(&g,i,j,0);}      /*each cell's value becomes zero*/

        sudoku_init_choices(&g);                                /*set choices*/

        while ( (n = sudoku_try_next(g, &i, &j)) ){             /*loop terminates when the puzzle is complete*/
                grid_update_value(&g, i, j, n);                 /*i,j cell's value becomes n*/
                sudoku_init_choices(&g);                    /*eliminate the choices of i,j cell and remove the*/
        }                                                   /*the choice n from the i-row,j-column and sub-grid*/
        if (sudoku_is_correct(g)) return g;
    }
    n = 0;  /*Since our 10 tries failed, we use a ready puzzle*/
    for (i = 0; i < 9; i++){
        for (j = 0; j < 9; j++){
            grid_update_value(&g, i, j, ready_puzzle[n++]);
        }
    }
    sudoku_init_choices(&g);                                    /*and reset choices*/
    return g;
}

/* Returns true if solution g, as returned by sudoku_solve, has a
   unique choice for each step (no backtracking required). (Note, g
   must have been computed with the use of sudoku_solve.) */
int sudoku_solution_is_unique(Grid_T g){
    return (grid_read_unique(g));
}

/* Generate and return a sudoku puzzle with "approximately" nelts
   elements having non-0 value. The smaller nelts the harder may be to
   generate/solve the puzzle. For instance, nelts=81 should return a
   completed and correct puzzle. */
Grid_T sudoku_generate(int nelts, int unique){
    Grid_T g, tmp;
    int i, j, total;
    g = sudoku_generate_complete();                 /*Produce a puzzle*/
    tmp = g;                                        /*make a copy of the puzzle*/
    for (total=81; total != nelts;){
        i = rand()%9;
        j = rand()%9;
        if (!grid_read_value(g, i, j) ) continue;   /*if value is already zero try another cell*/
        grid_update_value(&tmp,i,j,0);              /*tmp i,j cell's value becomes zero*/
        sudoku_init_choices(&tmp);                  /*set tmp's choices*/
        if (unique == 1){                           /*if we want a unique solution*/
            if (sudoku_solution_is_unique(sudoku_solve(tmp))){  /*if the tmp has unique solution then update the value*/
                sudoku_init_choices(&g);                        /*of the original puzzle.Otherwise, try a differnt cell to eliminate(0)*/
                grid_update_value(&g, i, j, 0);
		        total--;
	        }
            else
                tmp = g;                                       /*else discard the elimination and go back to the old puzzle and tetry*/
        }
        else{			/*if we don't want a unique solution*/
            sudoku_init_choices(&g);    /*set g's choices*/
            grid_update_value(&g,i,j,0);
	        total--;
        }
    }
    return g;
}

/* Sovle puzzle g and return the solved puzzle; if the puzzle has
   multiple solutions, return one of the possible solutions. */
Grid_T sudoku_solve(Grid_T g){
    int i, j, n, choice;
    Grid_T tmp, solved;
    while ((n = sudoku_try_next(g, &i, &j))){       /*loop terminates when puzzle is complete*/
        choice = sudoku_update_choice(&g, i, j, n); /*remove choice n from i,j cell*/
        if (choice != 1){                   /*if we still have choices*/
            grid_clear_unique(&g);          /*g is not unique*/
            tmp = g;                        /*make a copy of the puzzle*/
            grid_update_value(&tmp, i, j, n);
            solved = sudoku_solve(tmp);     /*recursive call*/
            if (sudoku_is_correct(solved)) return solved;   /*base case*/
        }
        else{                               /*if there is no choices*/
            grid_update_value(&g, i, j, n);
            sudoku_eliminate_choice(&g, i, j, n);
        }
    }
    return g;
}
/*--------------------------------------------------*/
int main(int argc, char **argv){
    Grid_T init, fin;
    int n, flag = 1;
    srand(getpid());
    switch (argc){
        case 1:                 /*sudoku*/
            init = sudoku_read();
            sudoku_print(stdout, init);
            sudoku_init_choices(&init);
            fin = sudoku_solve(init);
            if (sudoku_is_correct(fin)){
                if (sudoku_solution_is_unique(fin))
                    fprintf(stdout, "Puzzle has a unique solution:\n");
                else
                    fprintf(stdout, "Puzzle has a solution:\n");
            }
            else{
                fprintf(stdout, "Puzzle does not have a solution:\n");
            }
            sudoku_print(stdout, fin);
            sudoku_print_errors(fin);
            break;
        case 2:                 /*sudoku –c*/
            if (argv[1][0] == '-' && argv[1][1] == 'c' && argv[1][2] == '\0'){
                init = sudoku_read();
                sudoku_print(stdout, init);
                if (sudoku_is_correct(init)) fprintf(stdout, "Puzzle solution is correct.\n");
                else fprintf(stderr, "Puzzle solution is not correct.\n");
                sudoku_print_errors(init);
            }
            break;
        case 3:                 /*sudoku –g nelts*/
            if (argv[1][0] == '-' && argv[1][1] == 'g' && argv[1][2] == '\0'){
                for (n=0; argv[2][n] != '\0'; n++)
                    if (!isdigit((int)argv[2][n])) flag = 0;
                if (flag){
                    n = atoi(argv[2]);
                    init = sudoku_generate(n, 0);
                    fprintf(stdout, "New puzzle:\n");
                    sudoku_print(stdout, init);
                }
            }
            break;
        case 4:                 /*sudoku –g nelts –u*/
            if (argv[1][0] == '-' && argv[1][1] == 'g' && argv[1][2] == '\0'
            && argv[3][0] == '-' && argv[3][1] == 'u' && argv[3][2] == '\0'){
	            for (n=0; argv[2][n] != '\0'; n++)
                    if (!isdigit((int)argv[2][n])) {flag = 0;}
	            if (flag){
                    n = atoi(argv[2]);
                    init = sudoku_generate(n, 1);
                    fprintf(stdout, "New puzzle:\n");
                    sudoku_print(stdout, init);
                }
            }
            break;
        default:
            fprintf(stderr,"Wrong arguments!");
    }
    return 0;
}