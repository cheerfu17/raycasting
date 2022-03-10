#include <stdio.h>
#include <math.h>
#include <graphics.h>

typedef struct Map
{
   int x;
   int y;
   int size = 100;
} Map;


typedef struct Player
{
   
   int x = 400;
   int y = 300;
   int speed = 3;
   double angle = 0.0;
   
   double num_rays = 120;
   double fov = acos(-1.0) / 3.0;
   int max_depth = 800;
   double delta_angle = fov / num_rays;
   double half_fov = fov/2; 
   double dist = num_rays / (2*tan(half_fov));
   double proj_coeff = dist * num_rays;
   int scale = 1200 / num_rays;
    
} Player;

Player player;


void initPlayer()
{
   fillellipse(player.x, player.y, 5, 5);
   setcolor(YELLOW);
   line(player.x, player.y, player.x+960*cos(player.angle), player.y+960*sin(player.angle));
   
   
}


void initMap(int *count, Map *m)
{
  
   int c = 0;

   char map[8][13] = {"############",
                                       "#*****#****#",
                                       "#**********#",
                                       "#***##***#*#",
                                       "#*#******#*#",
                                       "#*####***#*#",
                                       "#*#******#*#",
                                       "############"};
   
                                    
   for (int i = 0; i < 8; i++)
   {
      for (int j = 0; j < 12; j++)
      {
         if (map[i][j] == '#')
         {
            m[c].x = j*100;
            m[c].y= i*100;
            m[c].size = 100;
            c++;
         }
         
      }
   }
   *count = c;
   
}

void drawMap(int *count, Map *m)
{
   for (int i = 0; i <= *count; i++)
   {
      setfillstyle(SOLID_FILL, GREEN);
      bar(m[i].x, m[i].y, m[i].x + 100, m[i].y + 100 );
   }
}


void moving()
{
   double sin_a = sin(player.angle);
   double cos_a = cos(player.angle);
   
   if (GetKeyState('A') & 0x8000)
   {
      player.x += player.speed * sin_a;
      player.y += -player.speed * cos_a;
   }
   else if(GetKeyState('D') & 0x8000)
   {
      player.x += -player.speed * sin_a;
      player.y += player.speed * cos_a;
   }
   
   if (GetKeyState('W') & 0x8000)
   {
      player.x += player.speed * cos_a;
      player.y += player.speed * sin_a;
   }
   else if (GetKeyState('S') & 0x8000)
   {
      player.x += -player.speed * cos_a;
      player.y += -player.speed * sin_a;
   }
   
   if (GetKeyState(VK_LEFT) & 0x8000)
   {
      player.angle -= 0.05;
   }
   else if (GetKeyState(VK_RIGHT) & 0x8000)
   {
      player.angle += 0.05;
   }
}

int inSquare(int x, int y, int *count, Map *m)
{
   for (int sq = 0; sq < *count; sq++)
   {
      
      if ((x / 100) * 100 == m[sq].x & (y / 100)*100 == m[sq].y)
      {
         return 1;
      }
   }

   return 0;
}

void squareNow (int *a, int *b)
{
   *a = (player.x / 100) * 100;
   *b = (player.y / 100) * 100;
}


int rayCasting(int *count, Map *m)
{
   double depth;
   int x;
   int y;
   
   int x_v;
   double dx;
   double dy;
   
   int ax = player.x;
   int ay = player.y;
   
   int xm;
   int ym;
   
   squareNow(&xm, &ym);
   
   double cur_angle = player.angle - player.half_fov;
   
   double depth_h;
   double depth_v;
   
   double sin_a;
   double cos_a;
   
   for (int ray = 0; ray < player.num_rays; ray++)
   {
      sin_a = sin(cur_angle);
      cos_a = cos(cur_angle);
      
      if (cos_a >= 0.0)
      {
         x = xm + 100;
         dx = 1;
      }
      else
      {
         x = xm;
         dx = -1;
      }
      
      for (int i = 0; i < 1200; i+=100)
      {
         depth_v = (x - ax) / cos_a;
         y = ay + depth_v*sin_a;
         
         if (inSquare(x+dx, y, count, m))
         {
            break;
         }
         x+=dx * 100;
         
      }
      
      if (sin_a >= 0.0)
      {
         y = ym + 100;
         dy = 1;
      }
      else
      {
         y = ym;
         dy = -1;
      }
      
      for (int i = 0; i < 800; i+=100)
      {
         depth_h = (y - ay) / sin_a;
         x = ax + depth_h*cos_a;
         
         if (inSquare(x, y + dy, count, m))
         {
            break;
         }
         y+=dy * 100;
         
         
      }
      
      
      if(depth_h > depth_v)
      {
         depth = depth_v;
      }
      else
      {
         depth = depth_h;
      }
      
      //int x1 = ax + depth* cos_a;
      //int y1 = ay + depth*sin_a;
      
      //line(ax, ay, x1, y1);
      
      depth*=cos(player.angle - cur_angle);
      double proj_h = 6*player.proj_coeff / depth;
      
      int c = 255 / (1 + depth*depth * 0.000002);

      
      setfillstyle(SOLID_FILL, COLOR(c, c, c));
      bar(ray * player.scale, 400 - (int)proj_h / 2, (ray * player.scale) + 10,(400 - (int)proj_h / 2)+ proj_h);
      cur_angle += player.delta_angle;
   }
   
   
   return 0;
   
}

void drawBG()
{
   setfillstyle(SOLID_FILL, COLOR(167, 185, 198));
  bar(0, 0, 1200, 400);
  setfillstyle(SOLID_FILL, CYAN);
  bar(0, 400, 1200, 800);
}

void drawCross()
{
   setfillstyle(SOLID_FILL, BLACK);
   bar(598, 360, 602, 380);
   bar(580, 378,620 , 382);
}

int main()
{
   Map m[97];
   int tile = 100;
   int count = 0;
   initwindow(1200, 800, "RayCasting", 500, 30);
   setbkcolor(BLACK);
   initPlayer();
   initMap(&count, m);
  int p=0; // сначала текущая видимая страница 0
  int t=0; // номер кадра (время)
  while(1)
  {
    p=1-p;
    setactivepage(p); // активная страница == 1-видимая
    setbkcolor(BLACK); // цвет фона
    clearviewport(); // закраска текущей страницы цветом фона
    ++t;
     //drawMap(&count, m);
      moving();
      //initPlayer();
      drawBG();
      rayCasting(&count , m);
     
     //drawCross();
    setvisualpage(p); // делаем активную страницу видимой
    delay(10); // задержка на 1/30 секунды
  }
   return 0;
}




