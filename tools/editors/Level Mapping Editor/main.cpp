#include <string>
#include <sstream>
#include <fstream>
#include <allegro.h>
#include <loadpng.h>
using namespace std;

volatile int speed_counter=0;
void increment_speed_counter(){speed_counter++;}
END_OF_FUNCTION(increment_speed_counter);

PALLETE dummypal;
BITMAP *buffer, *page[3];
BITMAP *tiles[3000];

int mapcontents[40][30];
int mapcontentsu[40][30];
int mapcontentsd[40][30];
int mapcontentsl[40][30];
int mapcontentsr[40][30];

int xp, yp, ctile;

int xpos, ypos;

int tileselectdelay;
bool getgrid(int x, int y, int u, int d, int l, int r){
  if(mapcontentsu[x][y]==u){
    if(mapcontentsd[x][y]==d){
      if(mapcontentsl[x][y]==l){
        if(mapcontentsr[x][y]==r){
          return true;
        }
      }
    }
  }
  return false;
}
//Some helpful functions!
string its(int t, int s=0){
  string te;
  ostringstream *its_temp;
  its_temp = new ostringstream;
  (*its_temp) << t;
  te = (*its_temp).str();
  delete its_temp;
  if(s==0){ return te;
  }else{
    int d;
    d=s-te.length();
    if(d<0){
      return te;
    }else{
      while(d>0){
        te = "0" + te;
        d--;
      }
      return te;
    }
  }
}

int sti(string s){
  istringstream stringbuf(s);
  int t;
  stringbuf >> t;
  return t;
}

void savemapsimple(){
  string s = "import.txt";
  ofstream file;
  file.open(s.c_str(), ios::trunc);
  int temp=0;

  for(int j=0; j<30; j++){
    for(int i=0; i<40; i++){
      temp=1;
      if(getgrid(i,j,1,0,0,0)) temp=2;
      if(getgrid(i,j,0,0,0,1)) temp=3;
      if(getgrid(i,j,0,1,0,0)) temp=4;
      if(getgrid(i,j,0,0,1,0)) temp=5;
      if(getgrid(i,j,1,0,0,1)) temp=6;
      if(getgrid(i,j,1,1,0,0)) temp=7;
      if(getgrid(i,j,1,0,1,0)) temp=8;
      if(getgrid(i,j,0,1,0,1)) temp=9;
      if(getgrid(i,j,0,0,1,1)) temp=10;
      if(getgrid(i,j,0,1,1,0)) temp=11;
      if(getgrid(i,j,0,1,1,1)) temp=12;
      if(getgrid(i,j,1,1,1,0)) temp=13;
      if(getgrid(i,j,1,0,1,1)) temp=14;
      if(getgrid(i,j,1,1,0,1)) temp=15;
      if(getgrid(i,j,1,1,1,1)) temp=16;
      if(mapcontents[i][j]==1){
        file << temp << " ";
      }else{
        file << 0 << " ";
      }
    }
    file << "\n";
  }

  file.close();
}

void savemap(){
  savemapsimple();
  int temp=0;
  string s = "output.txt";
  ofstream file;
  file.open(s.c_str(), ios::trunc);

  for(int j=0; j<30; j++){
    for(int i=0; i<40; i++){
      temp=1;
      if(getgrid(i,j,1,0,0,0)) temp=2;
      if(getgrid(i,j,0,0,0,1)) temp=3;
      if(getgrid(i,j,0,1,0,0)) temp=4;
      if(getgrid(i,j,0,0,1,0)) temp=5;
      if(getgrid(i,j,1,0,0,1)) temp=6;
      if(getgrid(i,j,1,1,0,0)) temp=7;
      if(getgrid(i,j,1,0,1,0)) temp=8;
      if(getgrid(i,j,0,1,0,1)) temp=9;
      if(getgrid(i,j,0,0,1,1)) temp=10;
      if(getgrid(i,j,0,1,1,0)) temp=11;
      if(getgrid(i,j,0,1,1,1)) temp=12;
      if(getgrid(i,j,1,1,1,0)) temp=13;
      if(getgrid(i,j,1,0,1,1)) temp=14;
      if(getgrid(i,j,1,1,0,1)) temp=15;
      if(getgrid(i,j,1,1,1,1)) temp=16;
      if(mapcontents[i][j]==1){
        file << temp;
        if(i<39) file << ",";
      }else{
        file << 0;
        if(i<39) file << ",";
      }
    }
    file << "\n";
  }

  file.close();
}

bool loadmap(){
  int xp, yp, t, n;
  string s = "import.txt";
  int temp=0;

  ifstream file;
  file.open(s.c_str());
  if(!file){
    return false;
  }

  for(int j=0; j<30; j++){
    for(int i=0; i<40; i++){
      file >> temp;
      mapcontents[i][j]=1;
      switch(temp){
        case 0: mapcontents[i][j]=0; break;
        case 2: mapcontentsu[i][j]=1; break;
        case 3: mapcontentsr[i][j]=1; break;
        case 4: mapcontentsd[i][j]=1; break;
        case 5: mapcontentsl[i][j]=1; break;
        case 6: mapcontentsu[i][j]=1; mapcontentsr[i][j]=1; break;
        case 7: mapcontentsu[i][j]=1; mapcontentsd[i][j]=1; break;
        case 8: mapcontentsu[i][j]=1; mapcontentsl[i][j]=1; break;
        case 9: mapcontentsd[i][j]=1; mapcontentsr[i][j]=1; break;
        case 10: mapcontentsl[i][j]=1; mapcontentsr[i][j]=1; break;
        case 11: mapcontentsd[i][j]=1; mapcontentsl[i][j]=1; break;
        case 12: mapcontentsd[i][j]=1; mapcontentsl[i][j]=1; mapcontentsr[i][j]=1; break;
        case 13: mapcontentsu[i][j]=1; mapcontentsd[i][j]=1; mapcontentsl[i][j]=1; break;
        case 14: mapcontentsu[i][j]=1; mapcontentsl[i][j]=1; mapcontentsr[i][j]=1; break;
        case 15: mapcontentsu[i][j]=1; mapcontentsd[i][j]=1; mapcontentsr[i][j]=1; break;
        case 16: mapcontentsu[i][j]=1; mapcontentsd[i][j]=1; mapcontentsl[i][j]=1; mapcontentsr[i][j]=1; break;
      }
    }
  }

  file.close();
  return true;
}

void drawtile(int xp, int yp, int t){
  blit(tiles[t], buffer, 0, 0, xp, yp, 16, 16);
}

void print(BITMAP* bmp, int x, int y, string t, int r, int g, int b, bool c=false){
  const char* txt=t.c_str();
  if(c){x=160-(t.length()*4);}
  textout_ex(bmp, font, txt, x-1, y, makecol(0,0,0), -1);
  textout_ex(bmp, font, txt, x+1, y, makecol(0,0,0), -1);
  textout_ex(bmp, font, txt, x, y-1, makecol(0,0,0), -1);
  textout_ex(bmp, font, txt, x, y+1, makecol(0,0,0), -1);
  textout_ex(bmp, font, txt, x, y, makecol(r,g,b), -1);
}



int main(){
  int skipf=0, skipd=0;
  int state=0, gt=0;
  //Setup Allegro
  allegro_init();
  //Setup input
  install_keyboard();
  install_mouse();
  show_os_cursor(MOUSE_CURSOR_ARROW);

  //Setup timer
  install_timer();
  LOCK_VARIABLE(speed_counter);
  LOCK_FUNCTION(increment_speed_counter);
  install_int_ex(increment_speed_counter, BPS_TO_TIMER(60));

  //Setup graphics
  set_color_depth(32);
  set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
  register_png_file_type();
  int currentpage=0;

  buffer=load_png("../../engine/current/data/graphics/tiles2.png", dummypal);
  for(int y=0;y<30;y++){
    for(int x=0;x<40;x++){
      tiles[x+(y*40)] = create_bitmap(8, 8);
      rectfill(tiles[x+(y*40)], 0, 0, 8, 8, makecol(16,16,16));
      blit(buffer, tiles[x+(y*40)], x*8, y*8, 0, 0, 8,8);
    }
  }
  destroy_bitmap(buffer);

  buffer=create_bitmap(320,240);
  page[0]=create_video_bitmap(640, 480);
  page[1]=create_video_bitmap(640, 480);
  page[2]=create_video_bitmap(640, 480);
  currentpage=0;

  //Setup sound
  set_volume_per_voice(0);
  if (install_sound(DIGI_AUTODETECT, MIDI_NONE, 0) < 0) {
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
    allegro_message("Error installing sound.\n");
  }

  //Init map
  //Load last?
  //Blank
  if(!loadmap()){
    for(int j=0; j<30; j++){
      for(int i=0; i<40; i++){
        mapcontents[i][j]=0;
        mapcontentsu[i][j]=0;
        mapcontentsd[i][j]=0;
        mapcontentsl[i][j]=0;
        mapcontentsr[i][j]=0;
      }
    }
  }
  xp=0; yp=0; ctile=0;
  tileselectdelay=0;

  xpos=20; ypos=15;
  int temp=0;

 //Starting main loop:
  while(!key[KEY_ESC]){
    xp=mouse_x/16;
    yp=mouse_y/16;
    //Render
    clear_to_color(buffer, makecol(32,32,32));
    for(int j=0; j<30; j++){
      for(int i=0; i<40; i++){
        if(mapcontents[i][j]>0){
          temp=1;
          if(getgrid(i,j,1,0,0,0)) temp=2;
          if(getgrid(i,j,0,0,0,1)) temp=3;
          if(getgrid(i,j,0,1,0,0)) temp=4;
          if(getgrid(i,j,0,0,1,0)) temp=5;
          if(getgrid(i,j,1,0,0,1)) temp=6;
          if(getgrid(i,j,1,1,0,0)) temp=7;
          if(getgrid(i,j,1,0,1,0)) temp=8;
          if(getgrid(i,j,0,1,0,1)) temp=9;
          if(getgrid(i,j,0,0,1,1)) temp=10;
          if(getgrid(i,j,0,1,1,0)) temp=11;
          if(getgrid(i,j,0,1,1,1)) temp=12;
          if(getgrid(i,j,1,1,1,0)) temp=13;
          if(getgrid(i,j,1,0,1,1)) temp=14;
          if(getgrid(i,j,1,1,0,1)) temp=15;
          if(getgrid(i,j,1,1,1,1)) temp=16;
          drawtile(i*8, j*8, temp+880+80+80);
        }
      }
    }
    //GUI
    /*  rect(buffer, 4,4,21,21, makecol(0,0,0));
      drawtile(5, 5, ctile);
      print(buffer, 24, 7, "Tile " + its(ctile), 255,255,255);
    }else{
      rect(buffer, 4,215,21,232, makecol(0,0,0));
      drawtile(5, 216, ctile);
      print(buffer, 24, 220, "Tile " + its(ctile), 255,255,255);
    }*/

    rect(buffer, (xp*8), (yp*8), (xp*8)+7, (yp*8)+7, makecol(96,96,96));

    if(skipf==0){
      rect(buffer, (xpos*8), (ypos*8), (xpos*8)+7, (ypos*8)+7, makecol(255,255,255));
    }else{
      rect(buffer, (xpos*8), (ypos*8), (xpos*8)+7, (ypos*8)+7, makecol(128,128,128));
    }
    skipd--;
    if(skipd<=0){
      skipd=25;
      skipf=skipf+1;
      if(skipf==2)skipf=0;
    }

    //Instructions
    //print(buffer, 5, 5, "Simple Tool framework", 255,255,255);

    stretch_blit(buffer, page[currentpage], 0, 0, 320, 240, 0, 0, 640, 480);
    show_video_bitmap(page[currentpage]);
    currentpage = (currentpage+1)%3;
    //Input
    if(mouse_b & 1){
      //Left click
      mapcontents[xp][yp]=1;
      xpos=xp;
      ypos=yp;
    }
    if(mouse_b & 2){
      //Right click
      mapcontents[xp][yp]=0;
      mapcontentsu[xp][yp]=0;
      mapcontentsd[xp][yp]=0;
      mapcontentsl[xp][yp]=0;
      mapcontentsr[xp][yp]=0;

      mapcontentsu[xp][yp+1]=0;
      mapcontentsd[xp][yp-1]=0;
      mapcontentsl[xp+1][yp]=0;
      mapcontentsr[xp-1][yp]=0;
    }

    if(tileselectdelay<=0){
      if(key[KEY_COMMA]){
        ctile--;
        tileselectdelay=4;
      }
      if(key[KEY_STOP]){
        ctile++;
        tileselectdelay=4;
      }

      if(key[KEY_UP]){
        tileselectdelay=8;
        mapcontents[xpos][ypos]=1;
        mapcontentsu[xpos][ypos]=1;
        ypos--;
        mapcontentsd[xpos][ypos]=1;
      }else if(key[KEY_DOWN]){
        tileselectdelay=8;
        mapcontents[xpos][ypos]=1;
        mapcontentsd[xpos][ypos]=1;
        ypos++;
        mapcontentsu[xpos][ypos]=1;
      }else if(key[KEY_LEFT]){
        tileselectdelay=8;
        mapcontents[xpos][ypos]=1;
        mapcontentsl[xpos][ypos]=1;
        xpos--;
        mapcontentsr[xpos][ypos]=1;
      }else if(key[KEY_RIGHT]){
        tileselectdelay=8;
        mapcontents[xpos][ypos]=1;
        mapcontentsr[xpos][ypos]=1;
        xpos++;
        mapcontentsl[xpos][ypos]=1;
      }

    }else{
      tileselectdelay--;
    }
    if(ctile<0) ctile+=10;
    if(ctile>=10) ctile-=10;



    if(key[KEY_F8]){
      savemap();
      while(key[KEY_F8]);
    }

    //Logic
    gt++;
    while(speed_counter<1) rest(1); speed_counter = 0;
  }

	return 0;
}
END_OF_MAIN()
