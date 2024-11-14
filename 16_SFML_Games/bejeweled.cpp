#include <SFML/Graphics.hpp>
#include <time.h>
using namespace sf;

const int TITEL_SIZE = 54;  // Tile size (each tile is 54x54 pixels)
Vector2i gridOffset(48,24);  // Offset to position the grid on the window


struct GemTile
{ int pixelX,pixelY,gridCol,gridRow,gemType,matchFlag,opacity;





//x and y Pixel position of the tile
//col and row The grid coordinates (column and row) of the tile
//kind Type of gem (integer representing different gem types)
//match Tracks if a tile is part of a match (1 or more if matched)
//alpha Opacity of the tile, used for fade-out animations
  GemTile(){matchFlag=0; opacity=255;}
} gameGrid[10][10];

void swapTiles(GemTile firstTile,GemTile secondTile)// Function to swap two tiles in the grid
{
  std::swap(firstTile.gridCol,secondTile.gridCol);// Swap their column and row positions
  std::swap(firstTile.gridRow,secondTile.gridRow);

  gameGrid[firstTile.gridRow][firstTile.gridCol]=firstTile;// Update the grid with the new positions of each piece
  gameGrid[secondTile.gridRow][secondTile.gridCol]=secondTile;
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

    for (int row=1; row <=8; row++)//Initializing the Grid
     for (int col =1; col <=8; col++)
      {
          gameGrid[row][col].gemType=rand()%3;//// Randomly assign a gem type
          gameGrid[row][col].gridCol= col;
          gameGrid[row][col].gridRow= row;
          gameGrid[row][col].pixelX = col * TITEL_SIZE;
          gameGrid[row][col].pixelY = row * TITEL_SIZE;
      }

    int selectedCol1,selectedRow1,selectedCol2,selectedRow2; int click=0; Vector2i pos;
    bool isSwap=false, isMoving=false;

    while (gameWindow.isOpen())//event handling
    {
        Event event;
        while (gameWindow.pollEvent(event))
        {
            if (event.type == Event::Closed)
                gameWindow.close();
                   
            if (event.type == Event::MouseButtonPressed)
                if (event.key.code == Mouse::Left)
                {
                   if (!isSwap && !isMoving) click++;
                   pos = Mouse::getPosition(gameWindow)-gridOffset;
                }
         }
    
   if (click==1)//Handling Tile Selection and Swapping
    {// First click to select a tile
      selectedCol1=pos.x/ TITEL_SIZE +1;
      selectedRow1=pos.y/ TITEL_SIZE +1;
    }
   if (click==2)// Second click to select a second tile
    {
      selectedCol2=pos.x/ TITEL_SIZE +1;
      selectedRow2=pos.y/ TITEL_SIZE +1;
      if (abs(selectedCol2-selectedCol1)+abs(selectedRow2-selectedRow1)==1)
        {swapTiles(gameGrid[selectedRow1][selectedCol1],gameGrid[selectedRow2][selectedCol2]); isSwap=1; click=0;} // Swap the two selected tiles
      else click=1;
    }

   //Match finding both vertically and horizontally.
   for(int row =1; row <=8; row++)
   for(int col=1; col <=8; col++)
   {
    if (gameGrid[row][col].gemType==gameGrid[row +1][col].gemType)
    if (gameGrid[row][col].gemType==gameGrid[row -1][col].gemType)
     for(int offset=-1;offset<=1;offset++) gameGrid[row +offset][col].matchFlag++;

    if (gameGrid[row][col].gemType==gameGrid[row][col +1].gemType)
    if (gameGrid[row][col].gemType==gameGrid[row][col -1].gemType)
     for(int offset=-1;offset<=1;offset++) gameGrid[row][col +offset].matchFlag++;
   }

   //Moving animation
   isMoving=false;// Reset moving flag
   for (int row=1;row<=8;row++)
    for (int col=1;col<=8;col++)
     {
       GemTile &pixelX = gameGrid[row][col];
       int dx,dy;
       for(int n=0;n<4;n++)   // 4 - speed
       {dx = pixelX.pixelX-pixelX.gridCol* TITEL_SIZE;
        dy = pixelX.pixelY-pixelX.gridRow* TITEL_SIZE;
        if (dx) pixelX.pixelX-=dx/abs(dx);// Move tile horizontally
        if (dy) pixelX.pixelY-=dy/abs(dy);}// Move tile vertically
       if (dx||dy) isMoving=1;
     }

   //Deleting amimation if match
   if (!isMoving)
    for (int row=1;row<=8;row++)
    for (int j=1;j<=8;j++)
    if (gameGrid[row][j].matchFlag)
        if (gameGrid[row][j].opacity>10)
        {gameGrid[row][j].opacity-=10; isMoving=true;}//Fades out tiles that are part of a match by reducing their alpha value.

   //Get score
   int score=0;
   for (int i=1;i<=8;i++)
    for (int col=1;col<=8;col++)
      score+=gameGrid[i][col].matchFlag;// extra score for each match

   //Second swap if no match
   if (isSwap && !isMoving)
      {if (!score) swapTiles(gameGrid[selectedRow1][selectedCol1],gameGrid[selectedRow2][selectedCol2]); isSwap=0;}

   //Drops unmatched tiles down to fill empty spaces 
   if (!isMoving)
    {
      for(int row=8;row>0;row--)
       for(int col=1;col<=8;col++)
         if (gameGrid[row][col].matchFlag)
         for(int aboveRow=row;aboveRow>0;aboveRow--)
            if (!gameGrid[aboveRow][col].matchFlag) {swapTiles(gameGrid[aboveRow][col],gameGrid[row][col]); break;};

      for(int col=1;col<=8;col++)
       for(int row=8,newGemCount=0;row>0;row--)
         if (gameGrid[row][col].matchFlag)
           {
            gameGrid[row][col].gemType = rand()%7;
            gameGrid[row][col].pixelY = -TITEL_SIZE *newGemCount++;
            gameGrid[row][col].matchFlag=0;
            gameGrid[row][col].opacity = 255;
           }
     }


    //////draw///////
    gameWindow.draw(background);
    //Draws the background and each gem tile,
    for (int row=1;row<=8;row++)
     for (int col=1;col<=8;col++)
      {
        GemTile pixelX = gameGrid[row][col];
        gems.setTextureRect( IntRect(pixelX.gemType*49,0,49,49));
        gems.setColor(Color(255,255,255,pixelX.opacity));
        gems.setPosition(pixelX.pixelX,pixelX.pixelY);
        gems.move(gridOffset.x- TITEL_SIZE,gridOffset.y- TITEL_SIZE);
        gameWindow.draw(gems);
      }

     gameWindow.display();
    }
    return 0;
}
