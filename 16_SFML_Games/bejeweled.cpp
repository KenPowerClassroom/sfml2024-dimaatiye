#include <SFML/Graphics.hpp>
#include <time.h>
using namespace sf;

const int TITEL_SIZE = 54;  // Tile size (each tile is 54x54 pixels)
Vector2i gridOffset(48,24);  // Offset to position the grid on the window


struct GemTile
{ int pixelX,pixelY,gridCol,gridRow,gemType,matchFlag,opacity;

int pixelX, pixelY; //x and y Pixel position of the tile
int gridCol, gridRow;//col and row The grid coordinates (column and row) of the tile
int gemType;//kind Type of gem (integer representing different gem types)
int matchFlag;//match Tracks if a tile is part of a match (1 or more if matched)
int opacity;//alpha Opacity of the tile, used for fade-out animations

  GemTile(){matchFlag=0; opacity=255;}
} gameGrid[10][10];

void swapTiles(GemTile firstTile,GemTile secondTile)// Function to swap two tiles in the grid
{
  std::swap(firstTile.gridCol,secondTile.gridCol);// Swap their column and row positions
  std::swap(firstTile.gridRow,secondTile.gridRow);

  gameGrid[firstTile.gridRow][firstTile.gridCol]=firstTile;// Update the grid with the new positions of each piece
  gameGrid[secondTile.gridRow][secondTile.gridCol]=secondTile;
}

void initializeGrid()
{
    for (int row = 1; row <= 8; row++)
        for (int col = 1; col <= 8; col++)
        {
            gameGrid[row][col].gemType = rand() % 3;
            gameGrid[row][col].gridCol = col;
            gameGrid[row][col].gridRow = row;
            gameGrid[row][col].pixelX = col * TILE_SIZE;
            gameGrid[row][col].pixelY = row * TILE_SIZE;
        }
}


bool handleInput(RenderWindow& gameWindow, int& clickCount, Vector2i& mousePos, bool& isSwapping, int& selectedCol1, int& selectedRow1, int& selectedCol2, int& selectedRow2)
{
    Event event;
    while (gameWindow.pollEvent(event))
    {
        if (event.type == Event::Closed)
            gameWindow.close();

        if (event.type == Event::MouseButtonPressed && event.key.code == Mouse::Left)
        {
            if (!isSwapping) clickCount++;
            mousePos = Mouse::getPosition(gameWindow) - gridOffset;

            if (clickCount == 1)
            {
                selectedCol1 = mousePos.x / TILE_SIZE + 1;
                selectedRow1 = mousePos.y / TILE_SIZE + 1;
            }
            else if (clickCount == 2)
            {
                selectedCol2 = mousePos.x / TILE_SIZE + 1;
                selectedRow2 = mousePos.y / TILE_SIZE + 1;

                if (abs(selectedCol2 - selectedCol1) + abs(selectedRow2 - selectedRow1) == 1)
                {
                    swapTiles(gameGrid[selectedRow1][selectedCol1], gameGrid[selectedRow2][selectedCol2]);
                    isSwapping = true;
                    clickCount = 0;
                }
                else clickCount = 1;
            }
            return true;
        }
    }
    return false;
}

void findMatches()
{
    for (int row = 1; row <= 8; row++)
        for (int col = 1; col <= 8; col++)
        {
            if (gameGrid[row][col].gemType == gameGrid[row + 1][col].gemType &&
                gameGrid[row][col].gemType == gameGrid[row - 1][col].gemType)
            {
                for (int offset = -1; offset <= 1; offset++)
                    gameGrid[row + offset][col].matchFlag++;
            }

            if (gameGrid[row][col].gemType == gameGrid[row][col + 1].gemType &&
                gameGrid[row][col].gemType == gameGrid[row][col - 1].gemType)
            {
                for (int offset = -1; offset <= 1; offset++)
                    gameGrid[row][col + offset].matchFlag++;
            }
        }
}


bool animateTileMovement()
{
    bool isMoving = false;
    for (int row = 1; row <= 8; row++)
        for (int col = 1; col <= 8; col++)
        {
            GemTile& tile = gameGrid[row][col];
            int deltaX = tile.pixelX - tile.gridCol * TILE_SIZE;
            int deltaY = tile.pixelY - tile.gridRow * TILE_SIZE;
            if (deltaX) tile.pixelX -= deltaX / abs(deltaX);
            if (deltaY) tile.pixelY -= deltaY / abs(deltaY);
            if (deltaX || deltaY) isMoving = true;
        }
    return isMoving;
}

bool animateMatchDeletion()
{
    bool isMoving = false;
    for (int row = 1; row <= 8; row++)
        for (int col = 1; col <= 8; col++)
            if (gameGrid[row][col].matchFlag && gameGrid[row][col].opacity > 10)
            {
                gameGrid[row][col].opacity -= 10;
                isMoving = true;
            }
    return isMoving;
}
oid updateGridAfterMatch()
{
    for (int col = 1; col <= 8; col++)
    {
        int emptyTilesAbove = 0;
        for (int row = 8; row > 0; row--)
        {
            if (gameGrid[row][col].matchFlag)
            {
                emptyTilesAbove++;
                gameGrid[row][col].gemType = rand() % 7;
                gameGrid[row][col].pixelY = -TILE_SIZE * emptyTilesAbove;
                gameGrid[row][col].matchFlag = 0;
                gameGrid[row][col].opacity = 255;
            }
            else if (emptyTilesAbove > 0)
            {
                swapTiles(gameGrid[row + emptyTilesAbove][col], gameGrid[row][col]);
            }
        }
    }
}

void renderGame(RenderWindow& gameWindow, Sprite& backgroundSprite, Sprite& gemSprite)
{
    gameWindow.draw(backgroundSprite);
    for (int row = 1; row <= 8; row++)
        for (int col = 1; col <= 8; col++)
        {
            GemTile tile = gameGrid[row][col];
            gemSprite.setTextureRect(IntRect(tile.gemType * 49, 0, 49, 49));
            gemSprite.setColor(Color(255, 255, 255, tile.opacity));
            gemSprite.setPosition(tile.pixelX, tile.pixelY);
            gemSprite.move(gridOffset.x - TILE_SIZE, gridOffset.y - TILE_SIZE);
            gameWindow.draw(gemSprite);
        }
}

int mainGameLoop()//main
{
    srand(time(0));//random generator seed.

    RenderWindow gameWindow(VideoMode(740,480), "Match-3 Game!");//game window with size 740x480 pixels 
    gameWindow.setFramerateLimit(60);

    Texture backgroundTexture,gemTexture;
    backgroundTexture.loadFromFile("images/bejeweled/background.png");
    gemTexture.loadFromFile("images/bejeweled/gems.png");

    Sprite background(backgroundTexture), gems(gemTexture);

    initializeGrid();

    
    int selectedCol1,selectedRow1,selectedCol2,selectedRow2;
    int click=0;
    Vector2i pos;
    bool isSwap=false, isMoving=false;

    while (gameWindow.isOpen())
    {
        if (handleInput(gameWindow, clickCount, mousePos, isSwapping, selectedCol1, selectedRow1, selectedCol2, selectedRow2))
        {
            findMatches();
        }

        isMoving = animateTileMovement();
        if (!isMoving)
        {
            bool isDeleting = animateMatchDeletion();
            if (!isDeleting && isSwapping)
            {
                int score=0;
                for (int i=1;i<=8;i++)
                 for (int col=1;col<=8;col++)
                score+=gameGrid[i][col].matchFlag;// extra score for each match

   //Second swap if no match
                   if (!score)
                    swapTiles(gameGrid[selectedRow1][selectedCol1],gameGrid[selectedRow2][selectedCol2]); isSwap=0;
                isSwapping = false;
            }

            if (!isDeleting)
                updateGridAfterMatch();
        }

        renderGame(gameWindow, backgroundSprite, gemSprite);
        gameWindow.display();

    }
    return 0;
}
