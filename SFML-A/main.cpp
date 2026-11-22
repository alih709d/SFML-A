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
void drawBoard(sf::RenderWindow &window)
{
    for(int r = 0; r < 8; r++)
    {
        for(int c = 0; c < 8; c++)
        {
            sf::Color candyColor;

            if(board[r][c] == 1)
                candyColor = sf::Color::Red;

            else if(board[r][c] == 2)
                candyColor = sf::Color(255, 165, 0);

            else if(board[r][c] == 3)
                candyColor = sf::Color::Yellow;

            else if(board[r][c] == 4)
                candyColor = sf::Color::Green;

            else if(board[r][c] == 5)
                candyColor = sf::Color::Blue;

            else if(board[r][c] == 6)
                candyColor = sf::Color(128, 0, 128);

            // Create candy shape
            sf::CircleShape candy(22.f);

            candy.setFillColor(candyColor);

            // Calculate candy position
            candy.setPosition(sf::Vector2f(
                BOARD_X + c * CELL_SIZE + 8,
                BOARD_Y + r * CELL_SIZE + 8
            ));

            window.draw(candy);

            // Draw cell border
            sf::RectangleShape cell(sf::Vector2f(60.f, 60.f));

            cell.setPosition(sf::Vector2f(
                BOARD_X + c * CELL_SIZE,
                BOARD_Y + r * CELL_SIZE
            ));

            cell.setFillColor(sf::Color::Transparent);
            cell.setOutlineColor(sf::Color::White);
            cell.setOutlineThickness(1.f);

            window.draw(cell);
        }
    }

    // Draw white ring around selected candy
    if( selrow != -1 &&  selcol != -1)
    {
        sf::CircleShape ring(25.f);

        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineColor(sf::Color::White);
        ring.setOutlineThickness(3.f);

        ring.setPosition(sf::Vector2f(
            BOARD_X + selcol * CELL_SIZE + 5,
            BOARD_Y + selrow * CELL_SIZE + 5
        ));

        window.draw(ring);
    }
}
