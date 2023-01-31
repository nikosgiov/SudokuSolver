CC=gcc -ansi -pedantic -Wall

sudoku: sudoku.o grid.o
	$(CC) -o sudoku $^	

grid.o: grid.c grid.h
	$(CC) -c -o $@ $<

sudoku.o: sudoku.c grid.h sudoku.h
	$(CC) -c -o $@ $<

clean:
	rm -rf *.o
	rm sudoku