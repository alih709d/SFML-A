#include <iostream>
#include <fstream>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <optional>

using namespace std;

// Constants
const int ROWS = 8;
const int COLS = 8;
const int CELL_SIZE = 70;
const int BOARD_X = 20;
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
                BOARD_X + c * CELL_SIZE + 13,
                BOARD_Y + r * CELL_SIZE + 13
            ));

            window.draw(candy);

            // Draw cell border
            sf::RectangleShape cell(sf::Vector2f(
                static_cast<float>(CELL_SIZE),
               static_cast<float>(CELL_SIZE)
            ));

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
            BOARD_X + selcol * CELL_SIZE + 10,
            BOARD_Y + selrow * CELL_SIZE + 10
        ));

        window.draw(ring);
    }
}
bool loadLevel()
{

    ifstream fin("level.txt");
    if (!fin.is_open())
    {
        cout << "Error: Cannot open level.txt!" << endl;
        return false;
    }

    int rows, cols,count=0;

    
    if (!(fin >> rows >> cols) || (rows!=8 || cols!=8))
    {
        cout << "Invalid dimensions!" << endl;
        return false;
    }

    if(!(fin>>moves>>targetscore) || (targetscore<0 || moves<0))
    {
        cout<<"invalid target score or moves";
        return false;
        
    }
    
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            if(!(fin>>board[r][c]))
            {
                cout<<"not conatins 64 values";
                return false;
            }
        }
    }
    for(int r=0;r<8;r++)
    {
        for(int c=0;c<8;c++)
        {
            if(board[r][c]>=1 && board[r][c]<=6)
            {
                count++;
            }
            else if(board[r][c]<1 || board[r][c]>6)
            {
                cout<<"invalid candies";
                return false;
            }
        }
    }
    if(count==64)
    {
        cout<<"all candies reaad";
    }
    
    

    fin.close();
    return true;
}

void selecting_swapping(int x,int y,int board[8][8])
{
    int col;
    int row;
    int temp;
    
    if (x < BOARD_X || x >= BOARD_X + COLS * CELL_SIZE ||
    y < BOARD_Y || y >= BOARD_Y + ROWS * CELL_SIZE)
    {
        return;
    }
    col = (x - BOARD_X) / CELL_SIZE;
    row = (y - BOARD_Y) / CELL_SIZE;
    if(selrow==-1 || selcol==-1)
    {
        cout<<"no candy selected yet";
        selcol=col;
        selrow=row;
    }
    else 
    {
        if((row ==selrow-1 && col==selcol)|| (row==selrow+1 && col==selcol)||(col==selcol+1&& row==selrow)||(col==selcol-1&& row==selrow))
        {
            swapRow1 = selrow;
            swapCol1 = selcol;

            swapRow2 = row;
            swapCol2 = col;
            temp=board[selrow][selcol];
            board[selrow][selcol]=board[row][col];
            board[row][col]=temp;
            cout<<"neighbour";
            selcol=-1;
            selrow=-1;
    

        }
        else
        {
            cout<<"not neigghbour";
             
            selrow=row;
            selcol=col;

        }
           
    
    }
    


}



void removecandies(int board[8][8],int&score, int marked[][8])
{
    int r,c;
    for(r=0;r<8;r++)
    {
        for(c=0;c<8;c++)
        {
            if(marked[r][c]==true)
            {
                board[r][c]=0;
                score  +=10;
                marked[r][c]=false;


            }
            
        }
    }

}
void fallingcandies(int board[8][8])
{
    int r,c;
    for(c=7;c>=0;c--)
    {
        int writerow=7;
        for(r=7;r>=0;r--)
        {
            if(board[r][c]!=0)
            {
                board[writerow][c]=board[r][c];
                if(r != writerow)
                {
                board[r][c]=0;
                }
                writerow--;
            }
            

        }
        
    }

}
void refilling(int board[8][8])
{
    int r,c;
    for(r=0;r<8;r++)
    {
        for(c=0;c<8;c++)
        {
            if(board[r][c]==0)
            {
                board[r][c]=rand()%6+1;
            }
        }
    }

}

void processMatches()
{
    int matches = findMatches(board, marked);

    while(matches > 0)
    {
        removecandies(board, score, marked);

        fallingcandies(board);

        refilling(board);

        matches = findMatches(board, marked);
    }
}
