#include <iostream>
#include <fstream>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <optional>

using namespace std;

// Constants
const int ROWS = 8;
const int COLS = 8;
const int CELL_SIZE = 60;
const int BOARD_X = 100;
const int BOARD_Y = 100;

// Shared game variables
int board[ROWS][COLS];
int moves;
int targetscore;
int score = 0;

int selrow = -1;
int selcol = -1;

int swapRow1 = -1;
int swapCol1 = -1;
int swapRow2 = -1;
int swapCol2 = -1;

bool marked[ROWS][COLS];