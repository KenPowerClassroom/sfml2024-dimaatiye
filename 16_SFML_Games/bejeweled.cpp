#include <SFML/Graphics.hpp>
#include <time.h>
using namespace sf;

int ts = 54;  // Tile size (each tile is 54x54 pixels)
Vector2i offset(48,24);  // Offset to position the grid on the window


struct piece
{ int x,y,col,row,kind,match,alpha;
//x and y Pixel position of the tile
//col and row The grid coordinates (column and row) of the tile
//kind Type of gem (integer representing different gem types)
//match Tracks if a tile is part of a match (1 or more if matched)
//alpha Opacity of the tile, used for fade-out animations
  piece(){match=0; alpha=255;}
} grid[10][10];

void swap(piece p1,piece p2)// Function to swap two tiles in the grid
{
  std::swap(p1.col,p2.col);// Swap their column and row positions
  std::swap(p1.row,p2.row);

  grid[p1.row][p1.col]=p1;// Update the grid with the new positions of each piece
  grid[p2.row][p2.col]=p2;
}


int bejeweled()//main
{
    srand(time(0));//random generator seed.

    RenderWindow app(VideoMode(740,480), "Match-3 Game!");//game window with size 740x480 pixels 
    app.setFramerateLimit(60);

    Texture t1,t2;
    t1.loadFromFile("images/bejeweled/background.png");
    t2.loadFromFile("images/bejeweled/gems.png");

    Sprite background(t1), gems(t2);

    for (int i=1;i<=8;i++)//Initializing the Grid
     for (int j=1;j<=8;j++)
      {
          grid[i][j].kind=rand()%3;//// Randomly assign a gem type
          grid[i][j].col=j;
          grid[i][j].row=i;
          grid[i][j].x = j*ts;
          grid[i][j].y = i*ts;
      }

    int x0,y0,x,y; int click=0; Vector2i pos;
    bool isSwap=false, isMoving=false;

    while (app.isOpen())//event handling
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed)
                app.close();
                   
            if (e.type == Event::MouseButtonPressed)
                if (e.key.code == Mouse::Left)
                {
                   if (!isSwap && !isMoving) click++;
                   pos = Mouse::getPosition(app)-offset;
                }
         }
    
   if (click==1)//Handling Tile Selection and Swapping
    {// First click to select a tile
      x0=pos.x/ts+1;
      y0=pos.y/ts+1;
    }
   if (click==2)// Second click to select a second tile
    {
      x=pos.x/ts+1;
      y=pos.y/ts+1;
      if (abs(x-x0)+abs(y-y0)==1)
        {swap(grid[y0][x0],grid[y][x]); isSwap=1; click=0;} // Swap the two selected tiles
      else click=1;
    }

   //Match finding both vertically and horizontally.
   for(int i=1;i<=8;i++)
   for(int j=1;j<=8;j++)
   {
    if (grid[i][j].kind==grid[i+1][j].kind)
    if (grid[i][j].kind==grid[i-1][j].kind)
     for(int n=-1;n<=1;n++) grid[i+n][j].match++;

    if (grid[i][j].kind==grid[i][j+1].kind)
    if (grid[i][j].kind==grid[i][j-1].kind)
     for(int n=-1;n<=1;n++) grid[i][j+n].match++;
   }

   //Moving animation
   isMoving=false;// Reset moving flag
   for (int i=1;i<=8;i++)
    for (int j=1;j<=8;j++)
     {
       piece &p = grid[i][j];
       int dx,dy;
       for(int n=0;n<4;n++)   // 4 - speed
       {dx = p.x-p.col*ts;
        dy = p.y-p.row*ts;
        if (dx) p.x-=dx/abs(dx);// Move tile horizontally
        if (dy) p.y-=dy/abs(dy);}// Move tile vertically
       if (dx||dy) isMoving=1;
     }

   //Deleting amimation if match
   if (!isMoving)
    for (int i=1;i<=8;i++)
    for (int j=1;j<=8;j++)
    if (grid[i][j].match)
        if (grid[i][j].alpha>10)
        {grid[i][j].alpha-=10; isMoving=true;}//Fades out tiles that are part of a match by reducing their alpha value.

   //Get score
   int score=0;
   for (int i=1;i<=8;i++)
    for (int j=1;j<=8;j++)
      score+=grid[i][j].match;// extra score for each match

   //Second swap if no match
   if (isSwap && !isMoving)
      {if (!score) swap(grid[y0][x0],grid[y][x]); isSwap=0;}

   //Drops unmatched tiles down to fill empty spaces 
   if (!isMoving)
    {
      for(int i=8;i>0;i--)
       for(int j=1;j<=8;j++)
         if (grid[i][j].match)
         for(int n=i;n>0;n--)
            if (!grid[n][j].match) {swap(grid[n][j],grid[i][j]); break;};

      for(int j=1;j<=8;j++)
       for(int i=8,n=0;i>0;i--)
         if (grid[i][j].match)
           {
            grid[i][j].kind = rand()%7;
            grid[i][j].y = -ts*n++;
            grid[i][j].match=0;
            grid[i][j].alpha = 255;
           }
     }


    //////draw///////
    app.draw(background);
    //Draws the background and each gem tile,
    for (int i=1;i<=8;i++)
     for (int j=1;j<=8;j++)
      {
        piece p = grid[i][j];
        gems.setTextureRect( IntRect(p.kind*49,0,49,49));
        gems.setColor(Color(255,255,255,p.alpha));
        gems.setPosition(p.x,p.y);
        gems.move(offset.x-ts,offset.y-ts);
        app.draw(gems);
      }

     app.display();
    }
    return 0;
}
