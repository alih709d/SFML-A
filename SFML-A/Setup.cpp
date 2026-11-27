// =============================================================================
//  CANDY CRUSH (fruit edition) - team template, SFML 3
//  CS1002 Programming Fundamentals, Fall 2026
//
//  HOW TO WORK ON THIS FILE
//   1. Git > Pull BEFORE you start. Commit + Push when a function works.
//   2. Write code ONLY inside your own feature section(s). Leave the rest alone.
//   3. Do NOT au/to-format the whole file (it changes every line and causes conflicts).
//   4. Never commit code that does not compile.
//   5. Everything marked "provided" is only drawing / reading keys (allowed AI help).
//      All game logic is written by the team.
//
//  FRUIT FOR EACH BOARD NUMBER (logic only ever sees the numbers 1..6):
//   1 apple   2 orange   3 lemon   4 lime slice   5 blueberry   6 grapes   (0 = empty)
//
//  HOW ONE TURN FLOWS (so everyone's functions fit together)
//   click (Feature 3) -> cellFromClick -> select / swap with trySwap
//   trySwap -> findMatches (Feature 4)
//        no match : swap back, no move used
//        match    : useMove (Feature 6) -> resolveChain (Feature 5)
//   resolveChain: repeat { findMatches, removeMarked, dropCandies, refillBoard }
//                 until findMatches finds nothing; points via addScore (Feature 6)
//   then checkWinLose (Feature 6) -> endGame (Feature 7) if the game is over
// =============================================================================
#include<iostream>
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <string>
#include<fstream>
#include <cstdlib>


using namespace std;


// ============================== SHARED CONSTANTS ==============================
const int WIN_W = 600, WIN_H = 680;
const int ROWS = 8, COLS = 8;
const float CELL = 70.f;
const float BOARD_X = 20.f, BOARD_Y = 100.f;

const int MENU = 0, PLAYING = 1, GAME_OVER = 2;                      // game states
const int NO_KEY = 0, KEY_UP = 1, KEY_DOWN = 2, KEY_ENTER = 3, KEY_ESCAPE = 4;
int board[ROWS][COLS];
int movesleft;
int targetscore;
int score = 0;

int selrow = -1;
int selcol = -1;

int swapRow1 = -1;
int swapCol1 = -1;
int swapRow2 = -1;
int swapCol2 = -1;





// ============================== SHARED GAME DATA ==============================
int board[ROWS][COLS];            // 0 = empty, 1..6 = fruit types
int score = 0, movesleftLeft = 0, targetscore = 0;
int selrow = -1, selCol = -1;     // -1 = nothing selected
int gameState = MENU;
int menuChoice = 0;               // 0 New Game, 1 Continue, 2 Exit
bool playerWon = false;
bool noSavedGame = false;         // show "No saved game" on the menu when true
string errorMessage = "";         // shown on the menu when not empty (e.g. level.txt missing)

// filled by readEvents() every frame
bool clicked = false;
int clickX = 0, clickY = 0;
int pressedKey = NO_KEY;

sf::Font font;
sf::Color colors[7] = {
    sf::Color(25, 25, 45),        // 0: empty
    sf::Color(225, 45, 55),       // 1: apple (red)
    sf::Color(255, 150, 0),       // 2: orange
    sf::Color(255, 220, 50),      // 3: lemon (yellow)
    sf::Color(80, 190, 90),       // 4: lime (green)
    sf::Color(70, 100, 235),      // 5: blueberry (blue)
    sf::Color(165, 85, 230) };    // 6: grapes (purple)


// ======= PROTOTYPES (so any section can call any function, in any order) =======
// Feature 1: handleInput / updateGame / drawGame are written by the Feature 1 owner
bool loadLevel();                                           // Feature 2
bool cellFromClick(int px, int py, int& row, int& col);     // Feature 3
bool trySwap(int r1, int c1, int r2, int c2);               // Feature 3
void handleBoardClick(int px, int py);                      // Feature 3
int  findMatches(bool marked[ROWS][COLS]);                  // Feature 4
void removeMarked(bool marked[ROWS][COLS]);                 // Feature 5
void dropCandies();                                         // Feature 5
void refillBoard();                                         // Feature 5
void resolveChain();                                        // Feature 5
void addScore(int clearedCandies);                          // Feature 6
void useMove();                                             // Feature 6
void checkWinLose();                                        // Feature 6
void startNewGame();                                        // Feature 7
void handleMenuInput();                                     // Feature 7
void handleGameOverInput();                                 // Feature 7
void endGame(bool won);                                     // Feature 7
void saveGame();                                            // Feature 8
bool loadGame();                                            // Feature 8
void clearSave();                                           // Feature 8


<<<<<< < HEAD
    ====== =
    // TEMPORARY: only so you can see the drawing work before loadLevel exists.
    // Delete this function (and its call in main) when Feature 2 is done.
    void fillTestBoard()
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            board[r][c] = (r * 3 + c * 2) % 6 + 1;
    score = 0; movesleftLeft = 20; targetscore = 800;
}
>>>>>> > 0a18372cc0149140c457320a9d48d6073f1c89b6


// =============================================================================
//  PROVIDED: DRAWING HELPERS (shapes, fruits, screens) - do not edit without
//  telling the team. Only drawing, no game logic.
// =============================================================================
sf::Color darker(sf::Color c)
{
    return sf::Color((std::uint8_t)(c.r * 0.7f), (std::uint8_t)(c.g * 0.7f), (std::uint8_t)(c.b * 0.7f));
}

void fillCircle(sf::RenderWindow& window, float cx, float cy, float radius, sf::Color color)
{
    sf::CircleShape shape(radius);
    shape.setOrigin({ radius, radius });
    shape.setPosition({ cx, cy });
    shape.setFillColor(color);
    window.draw(shape);
}

void fillEllipse(sf::RenderWindow& window, float cx, float cy, float rx, float ry, sf::Color color)
{
    sf::CircleShape shape(50.f, 40);                 // a circle stretched into an ellipse
    shape.setOrigin({ 50.f, 50.f });
    shape.setScale({ rx / 50.f, ry / 50.f });
    shape.setPosition({ cx, cy });
    shape.setFillColor(color);
    window.draw(shape);
}

void fillBar(sf::RenderWindow& window, float cx, float cy, float w, float h, float angleDeg, sf::Color color)
{
    sf::RectangleShape bar({ w, h });
    bar.setOrigin({ w / 2, h / 2 });
    bar.setPosition({ cx, cy });
    bar.setRotation(sf::degrees(angleDeg));
    bar.setFillColor(color);
    window.draw(bar);
}

void fillLeaf(sf::RenderWindow& window, float x, float y, sf::Color color)
{
    sf::ConvexShape leaf;
    leaf.setPointCount(4);
    leaf.setPoint(0, { x, y });
    leaf.setPoint(1, { x + 9, y - 7 });
    leaf.setPoint(2, { x + 20, y - 2 });
    leaf.setPoint(3, { x + 9, y + 5 });
    leaf.setFillColor(color);
    window.draw(leaf);
}

// one fruit, centred at (cx, cy)
void drawFruit(sf::RenderWindow& window, int type, float cx, float cy)
{
    const sf::Color base = colors[type];
    const sf::Color shine(255, 255, 255, 120);
    const sf::Color brown(110, 70, 30);
    const sf::Color leafGreen(60, 170, 60);

    switch (type)
    {
    case 1:   // apple
    {
        fillCircle(window, cx - 9, cy + 3, 18, base);
        fillCircle(window, cx + 9, cy + 3, 18, base);
        fillCircle(window, cx, cy + 6, 19, base);
        fillBar(window, cx, cy - 22, 3, 11, 0, brown);       // stem
        fillLeaf(window, cx + 2, cy - 22, leafGreen);
        fillEllipse(window, cx - 13, cy - 4, 5, 8, shine);
        break;
    }
    case 2:   // orange
    {
        fillCircle(window, cx, cy + 2, 25, darker(base));
        fillCircle(window, cx - 1, cy + 1, 23, base);
        fillCircle(window, cx + 8, cy + 8, 1.5f, darker(base));   // skin dots
        fillCircle(window, cx - 6, cy + 12, 1.5f, darker(base));
        fillCircle(window, cx + 12, cy - 4, 1.5f, darker(base));
        fillCircle(window, cx, cy - 22, 3, brown);
        fillLeaf(window, cx + 1, cy - 22, leafGreen);
        fillEllipse(window, cx - 11, cy - 7, 6, 4, shine);
        break;
    }
    case 3:   // lemon
    {
        fillEllipse(window, cx, cy, 28, 21, base);
        fillCircle(window, cx - 27, cy, 5, base);             // pointed tips
        fillCircle(window, cx + 27, cy, 5, base);
        fillLeaf(window, cx + 2, cy - 19, leafGreen);
        fillEllipse(window, cx - 9, cy - 8, 8, 4, shine);
        break;
    }
    case 4:   // lime slice
    {
        const sf::Color pale(220, 245, 170);
        fillCircle(window, cx, cy, 27, base);                 // skin
        fillCircle(window, cx, cy, 23, pale);                 // rind
        fillCircle(window, cx, cy, 20, sf::Color(140, 210, 90));   // flesh
        for (int i = 0; i < 3; i++)                           // 6 segment lines
            fillBar(window, cx, cy, 2.5f, 40, i * 60.f, pale);
        fillCircle(window, cx, cy, 4, pale);
        break;
    }
    case 5:   // blueberry
    {
        fillCircle(window, cx, cy + 1, 25, base);
        fillCircle(window, cx, cy - 6, 7, darker(base));      // crown on top
        fillCircle(window, cx, cy - 6, 3, sf::Color(30, 40, 120));
        fillEllipse(window, cx - 11, cy - 11, 6, 4, shine);
        break;
    }
    case 6:   // grapes
    {
        const float gx[6] = { -14, 0, 14, -7, 7, 0 };         // positions of the 6 grapes
        const float gy[6] = { -12, -12, -12, 2, 2, 16 };
        for (int i = 0; i < 6; i++)
        {
            fillCircle(window, cx + gx[i], cy + gy[i], 10, base);
            fillCircle(window, cx + gx[i] - 3, cy + gy[i] - 3, 3, shine);
        }
        fillBar(window, cx, cy - 24, 3, 10, 0, brown);        // stem
        fillLeaf(window, cx + 2, cy - 24, leafGreen);
        break;
    }
    default:
        break;
    }
}

void drawSelectionRing(sf::RenderWindow& window, float cx, float cy)
{
    sf::CircleShape ring(29.f);
    ring.setOrigin({ 29.f, 29.f });
    ring.setPosition({ cx, cy });
    ring.setFillColor(sf::Color::Transparent);
    ring.setOutlineThickness(4.f);
    ring.setOutlineColor(sf::Color::White);
    window.draw(ring);
}

void drawText(sf::RenderWindow& window, const string& str, float x, float y,
    unsigned int size, sf::Color color)
{
    sf::Text text(font, str, size);
    text.setFillColor(color);
    text.setPosition({ x, y });
    window.draw(text);
}

void drawBoard(sf::RenderWindow& window)
{
    // frame behind the board
    sf::RectangleShape frame({ COLS * CELL + 8, ROWS * CELL + 8 });
    frame.setPosition({ BOARD_X - 4, BOARD_Y - 4 });
    frame.setFillColor(sf::Color(20, 20, 50));
    frame.setOutlineThickness(3.f);
    frame.setOutlineColor(sf::Color(110, 120, 220));
    window.draw(frame);

    // one dark square per cell (the gap between them shows the grid)
    sf::RectangleShape cellBox({ CELL - 2, CELL - 2 });
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            cellBox.setFillColor((r + c) % 2 == 0 ? sf::Color(32, 32, 64) : sf::Color(40, 40, 78));
            cellBox.setPosition({ BOARD_X + c * CELL + 1, BOARD_Y + r * CELL + 1 });
            window.draw(cellBox);

            float cx = BOARD_X + c * CELL + CELL / 2;
            float cy = BOARD_Y + r * CELL + CELL / 2;
            if (board[r][c] != 0)
                drawFruit(window, board[r][c], cx, cy);
            if (r == selrow && c == selcol)
                drawSelectionRing(window, cx, cy);
        }
    }
}

void drawHUD(sf::RenderWindow& window)
{
    sf::RectangleShape panel({ 568.f, 64.f });
    panel.setPosition({ 16.f, 16.f });
    panel.setFillColor(sf::Color(20, 20, 50));
    panel.setOutlineThickness(3.f);
    panel.setOutlineColor(sf::Color(110, 120, 220));
    window.draw(panel);

    drawText(window, "SCORE: " + to_string(score), 30, 36, 22, sf::Color::White);
    drawText(window, "movesleft: " + to_string(movesleftLeft), 240, 36, 22, sf::Color(255, 220, 50));
    drawText(window, "TARGET: " + to_string(targetscore), 420, 36, 22, sf::Color(120, 230, 120));
}

void drawMenu(sf::RenderWindow& window)
{
    drawText(window, "CANDY CRUSH", 150, 70, 48, sf::Color::White);
    for (int i = 1; i <= 6; i++)                              // a row of fruits as decoration
        drawFruit(window, i, 85.f + (i - 1) * 86.f, 190.f);

    string options[3] = { "New Game", "Continue", "Exit" };
    for (int i = 0; i < 3; i++)
    {
        sf::Color col = (i == menuChoice) ? sf::Color(255, 220, 50) : sf::Color(180, 180, 180);
        drawText(window, (i == menuChoice ? "> " : "  ") + options[i], 220, 290 + i * 60, 32, col);
    }
    if (noSavedGame)
        drawText(window, "No saved game", 220, 490, 24, sf::Color(255, 90, 90));
    if (!errorMessage.empty())
        drawText(window, errorMessage, 60, 540, 22, sf::Color(255, 90, 90));
}

void drawGameOver(sf::RenderWindow& window)
{
    drawText(window, playerWon ? "YOU WIN" : "GAME OVER", 180, 200, 56,
        playerWon ? sf::Color(120, 230, 120) : sf::Color(255, 90, 90));
    drawText(window, "Final score: " + to_string(score), 200, 320, 30, sf::Color::White);
    drawText(window, "Press Enter", 230, 400, 26, sf::Color(180, 180, 180));
}

// Fills clicked / clickX / clickY / pressedKey. Call it once per frame from handleInput.
void readEvents(sf::RenderWindow& window)
{
    clicked = false;
    pressedKey = NO_KEY;

    while (const std::optional event = window.pollEvent())
    {


        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }



        if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouse->button == sf::Mouse::Button::Left)
            {
                clickX = mouse->position.x;
                clickY = mouse->position.y;
                clicked = true;
            }
        }
        else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Up)          pressedKey = KEY_UP;
            else if (key->code == sf::Keyboard::Key::Down)   pressedKey = KEY_DOWN;
            else if (key->code == sf::Keyboard::Key::Enter)  pressedKey = KEY_ENTER;
            else if (key->code == sf::Keyboard::Key::Escape) pressedKey = KEY_ESCAPE;
        }
    }
}


// Feature 1

void handleInput(sf::RenderWindow& window)
{
    readEvents(window);
    switch (gameState)
    {
    case MENU:
        handleMenuInput();
        break;

    case PLAYING:
        if (clicked)
        {
            handleBoardClick(clickX, clickY);

        }
        break;

    case GAME_OVER:
        handleGameOverInput();

        break;

    }
}
void updateGame()
{
    switch (gameState)
    {
    case MENU:
        break;

    case PLAYING:
        break;

    case GAME_OVER:
        break;
    }

}

void drawGame(sf::RenderWindow& window)
{

    window.clear(sf::Color(20, 20, 50));
    switch (gameState)
    {
    case MENU:


        drawMenu(window);

        break;
    case PLAYING:
        drawHUD(window);
        drawBoard(window);

        break;

    case GAME_OVER:
        drawGameOver(window);
        break;
    }

    window.display();


}




<<<<<<< HEAD
    //feature 2
    ====== =

    // =============================================================================
    //  FEATURE 2 - Board from level.txt (15 marks)                 Owner: Bodla
    //  load level.txt into board (5) | drawn with nested loops (done, in drawBoard) |
    //  every candy number has its own colour (done) | missing file shows an error (2)
    // =============================================================================
    // Reads rows/cols, then movesleft and target score, then the board into board[r][c].
    // Returns false (and sets errorMessage) if level.txt cannot be opened.
    // The starting board must not have three in a row anywhere.
    >>>>>> > 0a18372cc0149140c457320a9d48d6073f1c89b6
    bool loadLevel()
{

    ifstream fin("level.txt");
    if (!fin.is_open())
    {
        cout << "Error: Cannot open level.txt!" << endl;
        return false;
    }

    int rows, cols, count = 0;


    if (!(fin >> rows >> cols) || (rows != 8 || cols != 8))
    {
        cout << "Invalid dimensions!" << endl;
        return false;
    }

    if (!(fin >> movesleft >> targetscore) || (targetscore < 0 || movesleft < 0))
    {
        cout << "invalid target score or movesleft";
        return false;

    }

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            if (!(fin >> board[r][c]))
            {
                cout << "not conatins 64 values";
                return false;
            }
        }
    }
    for (int r = 0;r < 8;r++)
    {
        for (int c = 0;c < 8;c++)
        {
            if (board[r][c] >= 1 && board[r][c] <= 6)
            {
                count++;
            }
            else if (board[r][c] < 1 || board[r][c]>6)
            {
                cout << "invalid candies";
                return false;
            }
        }
    }
    if (count == 64)
    {
        cout << "all candies reaad";
    }



    fin.close();
    return true;
}




//feature 3
void selecting_swapping(int x, int y, int board[8][8])
{
    int col;
    int row;
    int temp;
    <<<<<< < HEAD
        // to keep in dimension
        if (x < BOARD_X || x >= BOARD_X + COLS * CELL ||
            y < BOARD_Y || y >= BOARD_Y + ROWS * CELL)
        {
            return;
        }
    // to give exact row and col location of user click
    ====== =

        if (x < BOARD_X || x >= BOARD_X + COLS * CELL ||
            y < BOARD_Y || y >= BOARD_Y + ROWS * CELL)
        {
            return;
        }
    >>>>>> > 0a18372cc0149140c457320a9d48d6073f1c89b6
        col = (x - BOARD_X) / CELL;
    row = (y - BOARD_Y) / CELL;
    if (selrow == -1 || selcol == -1)
    {
        cout << "no candy selected yet";
        selcol = col;
        selrow = row;
    }
    else // check right left above below
    {
        if ((row == selrow - 1 && col == selcol) || (row == selrow + 1 && col == selcol) || (col == selcol + 1 && row == selrow) || (col == selcol - 1 && row == selrow))
        {
            swapRow1 = selrow;
            swapCol1 = selcol;

            swapRow2 = row;
            swapCol2 = col;
            temp = board[selrow][selcol];
            board[selrow][selcol] = board[row][col];
            board[row][col] = temp;
            cout << "neighbour";
            selcol = -1;
            selrow = -1;


        }
        else
        {
            cout << "not neigghbour";

            selrow = row;
            selcol = col;

        }


    }



}





int findMatches(bool marked[ROWS][COLS])
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            marked[r][c] = false;
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c + 2 < COLS; c++)
        {
            int v = board[r][c];
            if (v != 0 && board[r][c + 1] == v && board[r][c + 2] == v)
                marked[r][c] = marked[r][c + 1] = marked[r][c + 2] = true;
        }
    for (int c = 0; c < COLS; c++)
        for (int r = 0; r + 2 < ROWS; r++)
        {
            int v = board[r][c];
            if (v != 0 && board[r + 1][c] == v && board[r + 2][c] == v)
                marked[r][c] = marked[r + 1][c] = marked[r + 2][c] = true;
        }
    int count = 0;
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if (marked[r][c])
                count++;

    return count;
}




void removecandies(int board[8][8], int& score, bool marked[][8])
{
    int r, c;
    for (r = 0;r < 8;r++)
    {
        for (c = 0;c < 8;c++)
        {
            if (marked[r][c] == true)
            {
                board[r][c] = 0;
                score += 10;
                marked[r][c] = false;


            }

        }
    }

}


<<<<<< < HEAD

    ====== =
    // In every column, candies above gaps fall down.
    void dropCandies()
{
    for (int c = 0; c < COLS; c++)
    {
        int writeRow = ROWS - 1;
        for (int r = ROWS - 1; r >= 0; r--)
        {
            if (board[r][c] != 0)
            {
                board[writeRow][c] = board[r][c];
                if (writeRow != r)
                    board[r][c] = 0;
                writeRow--;
            }
        }
    }
}
>>>>>> > 0a18372cc0149140c457320a9d48d6073f1c89b6

// Every cell still 0 at the top of a column gets a random candy 1..6.

void fallingcandies(int board[8][8])
{
    int r, c;
    for (c = 7;c >= 0;c--)
    {
        int writerow = 7;
        for (r = 7;r >= 0;r--)
        {
            if (board[r][c] != 0)
            {
                board[writerow][c] = board[r][c];
                if (r != writerow)
                {
                    board[r][c] = 0;
                }
                writerow--;
            }


        }

    }

}

// Repeats find / remove / drop / refill until findMatches finds nothing.
// The whole chain uses only the one move already taken.
void refilling(int board[8][8])
{
    int r, c;
    for (r = 0;r < 8;r++)
    {
        for (c = 0;c < 8;c++)
        {
            if (board[r][c] == 0)
            {
                board[r][c] = rand() % 6 + 1;
            }
        }
    }

}

void processMatches()
{
    bool marked[ROWS][COLS];
    int matches = findMatches(marked);

    while (matches > 0)
    {
        removecandies(board, score, marked);

        fallingcandies(board);

        refilling(board);

        matches = findMatches(marked);
    }
}





// =============================================================================
//  FEATURE 6 - Score, moves and winning (10 marks)             Owner: ________
//  HUD shows score, moves, target (4, drawHUD is done) | each matching swap uses
//  exactly one move (3) | target reached = YOU WIN, no moves left = GAME OVER (3)
// =============================================================================
// 10 points for every candy cleared, chains included.
void addScore(int clearedCandies)
{
    // TODO
}

// Called once for every swap that makes a match.
void useMove()
{
    // TODO
}

// Called after a move AND all its chains are finished.
// Target reached -> endGame(true). Else no moves left -> endGame(false).
void checkWinLose()
{
    // TODO
}




// =============================================================================
//  FEATURE 7 - Menu and game states (8 marks)                  Owner: Ali
//  gameState with named constants + switch-case (2) | menu works, New Game
//  starts fresh (4) | game over / you win screen, Enter goes to the menu (2)
// =============================================================================
// New Game: score 0, board / moves / target loaded again from level.txt (loadLevel),
// nothing selected, then gameState = PLAYING. Show errorMessage if loading fails.
void startNewGame()
{
    score = 0;
    <<<<<< < HEAD
        loadLevel();

    selrow = -1;
    ====== =
        //loadLevel();
        fillTestBoard();
    selRow = -1;
    >>>>>> > 0a18372cc0149140c457320a9d48d6073f1c89b6
        selCol = -1;
    gameState = PLAYING;
}

// MENU state: Up/Down change menuChoice, Enter runs New Game / Continue / Exit.
// Continue with no save: set noSavedGame = true and stay on the menu.
void handleMenuInput()
{

    if (pressedKey == KEY_UP && menuChoice > 0)
    {
        menuChoice--;
    }
    else if (pressedKey == KEY_DOWN && menuChoice < 2)
    {
        menuChoice++;
    }
    else if (pressedKey == KEY_ENTER)
    {

        if (menuChoice == 0)
        {
            startNewGame();
        }
        else if (menuChoice == 1)
        {

            if (loadGame())
            {
                gameState = PLAYING;
            }
            else
            {
                noSavedGame = true;
            }



        }
        else if (menuChoice == 2)
        {
            exit(0);
        }

    }


}

// GAME_OVER state: Enter goes back to the menu.
void handleGameOverInput()
{
    if (pressedKey == KEY_ENTER)
    {
        gameState = MENU;
    }
}

// Sets playerWon, empties the save (clearSave) and switches to GAME_OVER.
void endGame(bool won)
{
    playerWon = won;
    clearSave();
    gameState = GAME_OVER;
}




// =============================================================================
//  FEATURE 8 - Save and continue (7 marks)                     Owner: Ali
//  Esc saves to savegame.txt and returns to the menu (3) | Continue restores
//  score, moves, target and board (3) | "No saved game" shown, save emptied
//  when a game ends (1)
// =============================================================================
// Format: line 1 = score movesLeft targetscore, then the 8 board rows (8 numbers each).
void saveGame()
{
    // TODO
}

// Returns false if there is no saved game. Nothing is selected after loading.
bool loadGame()
{
    // TODO
    return false;
}

// Empties savegame.txt (open an ofstream and close it straight away).
void clearSave()
{
    // TODO
}




// =============================================================================
//  FEATURES 9 and 10 - everyone
//  9: public repo, every member 5+ commits on different days with clear messages,
//     README has names, roll numbers, controls, how to run, one screenshot.
//  10: small functions, main() short, named constants instead of magic numbers,
//     meaningful names, comments on the tricky parts.
// =============================================================================




int main()
{



    srand((unsigned)time(0));

    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
        return 1;                       // font missing (Windows path); change if needed


    gameState = MENU;                // TEMPORARY, so you can see the board; start in MENU later

    sf::RenderWindow window(sf::VideoMode({ 600, 680 }), "Candy Crush");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        handleInput(window);
        updateGame();
        drawGame(window);

    }

    return 0;
}