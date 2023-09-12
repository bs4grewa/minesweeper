all: minesweeper

minesweeper: minesweeper.cpp
	g++ -lncurses minesweeper.cpp -o minesweeper

clean:
	rm minesweeper
