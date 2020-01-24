string currentarea;
PALLETE dummypal;
BITMAP *buffer, *biggerbuffer, *page[3];
BITMAP *worldmap;
int state, gt;
int currentpage;
BITMAP *tiles[4000];
BITMAP *tiles2[4000];
int tileset;

bool test;
string teststring;

int areamap[20][20];

int mapcontents[40][30];
int mapcopycontents[40][30];
int maptileset;

int xp, yp, ctile;
int reversekey;
int mapx, mapy;
int savednote;
int temp, temp2, temp3, p;

int tileselectdelay;
int paraadjustdelay;
string tempstring;

bool fullscreen;

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

void drawtile(int xp, int yp, int t){
  if(tileset==0){
    blit(tiles[t], buffer, 0, 0, xp, yp, 16, 16);
  }else{
    blit(tiles2[t], buffer, 0, 0, xp, yp, 16, 16);
  }
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

void rprint(BITMAP* bmp, int x, int y, string t, int r, int g, int b, bool c=false){
  const char* txt=t.c_str();
  if(c){x=160-(t.length()*4);}
  x=640-x-t.length()*8;
  textout_ex(bmp, font, txt, x-1, y, makecol(0,0,0), -1);
  textout_ex(bmp, font, txt, x+1, y, makecol(0,0,0), -1);
  textout_ex(bmp, font, txt, x, y-1, makecol(0,0,0), -1);
  textout_ex(bmp, font, txt, x, y+1, makecol(0,0,0), -1);
  textout_ex(bmp, font, txt, x, y, makecol(r,g,b), -1);
}

void r2print(BITMAP* bmp, int x, int y, string t, int r, int g, int b, bool c=false){
  const char* txt=t.c_str();
  if(c){x=160-(t.length()*4);}
  x=320-x-t.length()*8;
  textout_ex(bmp, font, txt, x-1, y, makecol(0,0,0), -1);
  textout_ex(bmp, font, txt, x+1, y, makecol(0,0,0), -1);
  textout_ex(bmp, font, txt, x, y-1, makecol(0,0,0), -1);
  textout_ex(bmp, font, txt, x, y+1, makecol(0,0,0), -1);
  textout_ex(bmp, font, txt, x, y, makecol(r,g,b), -1);
}

void change_fullscreen(){
  destroy_bitmap(page[0]);
  destroy_bitmap(page[1]);
  destroy_bitmap(page[2]);
  set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 640, 480, 0, 0);
  clear_bitmap(screen);


  page[0]=create_video_bitmap(640, 480);
  page[1]=create_video_bitmap(640, 480);
  page[2]=create_video_bitmap(640, 480);
}

void change_windowed(){
  destroy_bitmap(page[0]);
  destroy_bitmap(page[1]);
  destroy_bitmap(page[2]);

  set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
  clear_bitmap(screen);


  page[0]=create_video_bitmap(640, 480);
  page[1]=create_video_bitmap(640, 480);
  page[2]=create_video_bitmap(640, 480);
}

void savescreen(BITMAP* bmp){
  if (bitmap_color_depth(bmp) == 8)
	set_palette(dummypal);
  tempstring="minimap.png";
  save_bitmap(tempstring.c_str(), bmp, dummypal);
}
