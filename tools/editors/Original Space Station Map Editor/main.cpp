#include <string>
#include <sstream>
#include <fstream>
#include <allegro.h>
#include <loadpng.h>
using namespace std;

volatile int speed_counter=0;
void increment_speed_counter(){speed_counter++;}
END_OF_FUNCTION(increment_speed_counter);

string currentarea;
PALLETE dummypal;
BITMAP *buffer, *biggerbuffer, *page[3];
BITMAP *tiles[4000];

bool test;
string teststring;


int mapcontents[40][29];
int mapcopycontents[40][29];

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

class entities{
  public:
  int x, y, t;
  //parameters
  int p1, p2, p3, p4, p5, p6;
};
int numentities;

entities entity[200];
void addentity(int xp, int yp, int tp, int p1=0, int p2=0, int p3=0, int p4=0, int p5=320, int p6=240){
  entity[numentities].x=xp;
  entity[numentities].y=yp;
  entity[numentities].t=tp;
  entity[numentities].p1=p1;
  entity[numentities].p2=p2;
  entity[numentities].p3=p3;
  entity[numentities].p4=p4;
  entity[numentities].p5=p5;
  entity[numentities].p6=p6;

  numentities++;
}
void naddentity(int xp, int yp, int tp, int p1=0, int p2=0, int p3=0, int p4=0, int p5=320, int p6=240){
  entity[numentities].x=xp;
  entity[numentities].y=yp;
  entity[numentities].t=tp;
  entity[numentities].p1=p1;
  entity[numentities].p2=p2;
  entity[numentities].p3=p3;
  entity[numentities].p4=p4;
  entity[numentities].p5=p5;
  entity[numentities].p6=p6;
}

void copyentity(int a, int b){
  entity[a].x=entity[b].x;
  entity[a].y=entity[b].y;
  entity[a].t=entity[b].t;
  entity[a].p1=entity[b].p1;
  entity[a].p2=entity[b].p2;
  entity[a].p3=entity[b].p3;
  entity[a].p4=entity[b].p4;
  entity[a].p5=entity[b].p5;
  entity[a].p6=entity[b].p6;
}

void removeentity(int t){
  if(t==numentities-1){
    numentities--;
  }else{
    for(int m=t;m<numentities;m++) copyentity(m,m+1);
    numentities--;
  }
}

int entat(int xp, int yp){
  for(int i=0; i<numentities; i++){
    if(entity[i].x==xp && entity[i].y==yp) return i;
  }
  return -1;
}


bool entclear(int xp, int yp){
  for(int i=0; i<numentities; i++){
    if(entity[i].x==xp && entity[i].y==yp) return false;
  }
  return true;
}

void savemapsimple(){
  string s = "imports/"+currentarea+"/x"+its(mapx)+"y"+its(mapy)+".txt";
  ofstream file;
  file.open(s.c_str(), ios::trunc);

  for(int j=0; j<29; j++){
    for(int i=0; i<40; i++){
      file << mapcontents[i][j] << " ";
    }
    file << "\n";
  }

  file << numentities << "\n";
  for(int i=0; i<numentities; i++){
    file << entity[i].x << " " << entity[i].y << " " << entity[i].t;
    switch(entity[i].t){
      case 1: case 2: //Enemy
        file << " " << entity[i].p1 << " " << entity[i].p2
             << " " << entity[i].p3 << " " << entity[i].p4
             << " " << entity[i].p5 << " " << entity[i].p6;
      break;
      case 10: case 11: case 12: //Save point
        file << " " << entity[i].p1; //p1: 0 hanging from roof
      break;
      default:
      break;
    }
    file << "\n";
  }

  file.close();
}

void savemap(){
  int ccount=0, scount=0;
  savemapsimple();
  string s = "maps/"+currentarea+"/x"+its(mapx)+"y"+its(mapy)+".txt";
  ofstream file;
  file.open(s.c_str(), ios::trunc);

  file << "case rn("<< its(mapx) << ","<< its(mapy) << "):\n";
  file << "tmap = new Array();\n";

  for(int j=0; j<29; j++){
    file << "tmap.push(\"";
    for(int i=0; i<39; i++){
      file << mapcontents[i][j] << ",";
    }
    file << mapcontents[39][j] << "\");\n";
  }
  //file << "fillcontent(tmap);\n";
  if(numentities>0){
    file << "\n";
    for(int i=0; i<numentities; i++){
      if(entity[i].t==11){
        if(entity[i].x==0){
          file << "obj.createentity(game, " << -8 << ", " << (entity[i].y*8)+4
               << ", " << entity[i].t;
        }else{
          file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)+4
               << ", " << entity[i].t;
        }
      }else if(entity[i].t==12){
        if(entity[i].y==0){
          file << "obj.createentity(game, " << (entity[i].x*8)+3 << ", " << -8
               << ", " << entity[i].t;
        }else{
          file << "obj.createentity(game, " << (entity[i].x*8)+3 << ", " << (entity[i].y*8)
               << ", " << entity[i].t;
        }
      }else if(entity[i].t==2 && entity[i].p1==4){
        entity[i].t=3;
      }else{
        file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)
             << ", " << entity[i].t;
      }
      switch(entity[i].t){
        case 1: //Enemy
          file << ", " << entity[i].p1 << ", " << entity[i].p2;
          if(entity[i].p3==0 && entity[i].p4==0 && entity[i].p5==320 && entity[i].p6==240){
            file << ");  // Enemy";
          }else{
            file << ", " << entity[i].p3 << ", " << entity[i].p4
                 << ", " << entity[i].p5 << ", " << entity[i].p6
                 << ");  // Enemy, bounded";
          }
        break;
        case 2: //Moving platform
          file << ", " << entity[i].p1 << ", " << entity[i].p2;
          if(entity[i].p3==0 && entity[i].p4==0 && entity[i].p5==320 && entity[i].p6==240){
            file << ");  // Platform";
          }else{
            file << ", " << entity[i].p3 << ", " << entity[i].p4
                 << ", " << entity[i].p5 << ", " << entity[i].p6
                 << ");  // Platform, bounded";
          }
        break;
        case 3:
          file << "obj.createentity(game, " << (entity[i].x*8)+4 << ", " << (entity[i].y*8)
               << ", 3);  //Disappearing Platform";
          entity[i].t=2; //So that it doesn't disapear from the editor
        break;
        case 8: //coin
          file << ", " << entity[i].p1 << ", coin+"<<ccount<<");  // (coins)";
          ccount++;
        break;
        case 9: //shiny
          file << ", " << entity[i].p1 << ", "<<ccount<<"+coin);  // (shiny trinket)";
          ccount++;
        break;
        case 10: //Save point
          if(currentarea=="otherlevel") mapy+=100;
          if(currentarea=="lab") mapy+=200;
          if(currentarea=="spacestation") mapy+=300;


          file << ", " << entity[i].p1 << ", " << int(scount+((mapx+(mapy*100))*10)) << ");  // (savepoint)";
          scount++;

          if(currentarea=="otherlevel") mapy-=100;
          if(currentarea=="lab") mapy-=200;
          if(currentarea=="spacestation") mapy-=300;
        break;
        case 11: //Horizontal
          if(entity[i].x==0){
            file << ", " << entity[i].p1+8 <<");  // (horizontal gravity line)";
          }else{
            file << ", " << entity[i].p1 <<");  // (horizontal gravity line)";
          }
        break;
        case 12: //Vertical
          if(entity[i].y==0){
            file << ", " << entity[i].p1+8 <<");  // (vertical gravity line)";
          }else{
            file << ", " << entity[i].p1 <<");  // (vertical gravity line)";
          }
        break;
        default:
          file << ", behave, para);";
        break;
      }
      file << "\n";
    }
  }

  //this bit only if in the lab:
  temp=-1;
  for(int j=0; j<29; j++){
    for(int i=0; i<39; i++){
      if(temp==-1){
        if(mapcontents[i][j]==280) temp=0;
        if(mapcontents[i][j]==283) temp=1;
        if(mapcontents[i][j]==286) temp=2;
        if(mapcontents[i][j]==289) temp=3;
        if(mapcontents[i][j]==292) temp=4;
        if(mapcontents[i][j]==295) temp=5;
      }
    }
  }
  //if(temp>=0){
  //  file << "rcol="<<temp<<";\n";
 // }
  //

  file << "break;\n";

  file.close();
}

bool loadmap(int x, int y){
  int xp, yp, t, n;
  int p1, p2, p3, p4, p5, p6;
  string s = "imports/"+currentarea+"/x"+its(x)+"y"+its(y)+".txt";

  ifstream file;
  file.open(s.c_str());
  if(!file){
    for(int j=1; j<28; j++){
      for(int i=1; i<39; i++){
        mapcontents[i][j]=0;
      }
    }
    numentities=0;
    return false;
  }

  for(int j=0; j<29; j++){
    for(int i=0; i<40; i++){
      file >> mapcontents[i][j];
    }
  }

  file >> p;
  numentities=0;
  for(int i=0; i<p; i++){
    file >> temp; file >> temp2; file >> temp3;
    switch(temp3){
      case 1: case 2: //Enemy, or platform
        file >> p1; file >> p2; file >> p3; file >> p4; file >> p5; file >> p6;
        addentity(temp,temp2,temp3,p1,p2,p3,p4,p5,p6);
      break;
      case 10: case 11: case 12: //Save point
        file >> p1;
        addentity(temp,temp2,temp3,p1);
      break;
      default:
        addentity(temp,temp2,temp3);
      break;
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


void copymap(){
  for(int j=0; j<29; j++){
    for(int i=0; i<40; i++){
       mapcopycontents[i][j]=mapcontents[i][j];
    }
  }
}

void mapput(int x, int y, int t){
  if(x>=0 && x<40 && y>=0 && y<29){
    mapcontents[x][y]=t;
  }
}

int at(int x, int y){
  if(x>=0 && x<40 && y>=0 && y<29){
    return mapcopycontents[x][y];
  }
  if(x==-1) return at(x+1,y);
  if(x==40) return at(x-1,y);
  if(y==-1) return at(x,y+1);
  if(y==29) return at(x,y-1);
  return 0;
}

int match(int x, int y, int t){
  //Returns the first position match for a border
  // 5 1 6
  // 2 X 4
  // 7 3 8

  if(at(x-1,y)!=t && at(x,y-1)!=t) return 10;
  if(at(x+1,y)!=t && at(x,y-1)!=t) return 11;
  if(at(x-1,y)!=t && at(x,y+1)!=t) return 12;
  if(at(x+1,y)!=t && at(x,y+1)!=t) return 13;

  if(at(x,y-1)!=t) return 1;
  if(at(x-1,y)!=t) return 2;
  if(at(x,y+1)!=t) return 3;
  if(at(x+1,y)!=t) return 4;
  if(at(x-1,y-1)!=t) return 5;
  if(at(x+1,y-1)!=t) return 6;
  if(at(x-1,y+1)!=t) return 7;
  if(at(x+1,y+1)!=t) return 8;
  return 0;
}

bool inbox(int x1, int y1, int x2, int y2, int xt, int yt){
  if(xt>=x1 && xt<x2 && yt>=y1 && yt<y2) return true;
  return false;
}

int getbackground(){
  //Return the background tile used on the map.
  int x, y, t;

  for(int j=0; j<29; j++){
    for(int i=0; i<40; i++){
      t=at(i,j);
      x=t%40;
      y=(t-x)/40;
      if(inbox(0,17,40,22,x,y)){
        y=x%3;
        x=(x-y)/3;
        return x;
      }
    }
  }

  return -1;
}

int antiedge(int x, int y, int t){
  //Opposite problem to edge: if t is in the range of any tile group, simplify
  //it back to its basics.
  //Simplist way is to turn it into a coordinate and check inbox ranges.
  x=t%40;
  y=(t-x)/40;
  for(int i=0; i<8; i++){
    if(inbox(0+(i*3),2,3+(i*3),7,x,y)) return 80+(i*3);
    if(inbox(0+(i*3),7,3+(i*3),12,x,y)) return 280+(i*3);
    if(inbox(0+(i*3),12,3+(i*3),17,x,y)) return 480+(i*3);
    if(inbox(0+(i*3),17,3+(i*3),22,x,y)) return 680+(i*3);
  }
  return t;
}

int edge(int x, int y, int t){
  temp=1;
  if(t>=480 && t<520){
    temp=t-480;
  }else if(t>=280 && t<320){
    temp=t-280;
  }else if(t>=80 && t<120){
    temp=t-80;
  }else{
    return t;
  }

  if(temp%3==0){
    //we have a candidate for edginess!
    switch(match(x,y,t)){
      case 10: return t+80; break;
      case 11: return t+82; break;
      case 12: return t+160; break;
      case 13: return t+162; break;
      case 1: return t+81; break;
      case 2: return t+120; break;
      case 3: return t+161; break;
      case 4: return t+122; break;
      case 5: return t+42; break;
      case 6: return t+41; break;
      case 7: return t+2; break;
      case 8: return t+1; break;
      case 0: default: return t; break;
    }
  }

  return t;
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

int main(){
  test=false; teststring="TEST = True";
  int state=0, gt=0;
  int tileset=0;
  savednote=0; numentities=0; paraadjustdelay=0;
  currentarea="spacestation";
  //currentarea="lab";
  //currentarea="otherlevel";
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

  //Setup graphics8
  set_color_depth(32);
  set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
  register_png_file_type();
  int currentpage=0;
  fullscreen=false;

  if(tileset==0){
    buffer=load_png("../../engine/current/data/graphics/tiles.png", dummypal);
  }else if(tileset==1){
    buffer=load_png("../../engine/current/data/graphics/tiles2.png", dummypal);
  }
  for(int y=0;y<30;y++){
    for(int x=0;x<40;x++){
      tiles[x+(y*40)] = create_bitmap(8, 8);
      rectfill(tiles[x+(y*40)], 0, 0, 8, 8, makecol(16,16,16));
      blit(buffer, tiles[x+(y*40)], x*8, y*8, 0, 0, 8,8);
    }
  }
  destroy_bitmap(buffer);

  buffer=create_bitmap(320,240);
  biggerbuffer=create_bitmap(640,480);
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
  mapx=50; mapy=50;
  if(!loadmap(mapx, mapy)){
    for(int j=0; j<29; j++){
      for(int i=0; i<40; i++){
        mapcontents[i][j]=0;
      }
    }
  }
  xp=0; yp=0; ctile=0;
  tileselectdelay=0;
  reversekey=0;

 //Starting main loop:
  while(!key[KEY_ESC]){
    xp=mouse_x/16;
    yp=mouse_y/16;
    //Render
    if(tileset==0){
      clear_to_color(buffer, makecol(0,0,0));
    }else{
      clear_to_color(buffer, makecol(32,32,32));
    }
    for(int j=0; j<29; j++){
      for(int i=0; i<40; i++){
        if(mapcontents[i][j]>0) drawtile(i*8, j*8, mapcontents[i][j]);
      }
    }
    //Draw entities
    temp=entat(xp,yp);
    for(int i=0; i<numentities; i++){
      switch(entity[i].t){
        case 1: //Enemy
          teststring="E";
          if(entity[i].p1==0) teststring+="v";
          if(entity[i].p1==1) teststring+="^";
          if(entity[i].p1==2) teststring+="<";
          if(entity[i].p1==3) teststring+=">";

          textout_ex(buffer, font, teststring.c_str(), (entity[i].x*8),(entity[i].y*8)+4,
                       makecol(255,255,255), -1);
          if(i==temp){
            teststring=its(entity[i].p2);
            textout_ex(buffer, font, teststring.c_str(), (entity[i].x*8)+4,(entity[i].y*8)+17,
                         makecol(255,255,255), -1);

            rect(buffer,entity[i].x*8,entity[i].y*8,(entity[i].x*8)+15,(entity[i].y*8)+15,
                 makecol(255,0,0));
            rect(buffer,entity[i].p3,entity[i].p4,entity[i].p5-1,entity[i].p6-1,
                 makecol(255,128,0));
          }else{
            rect(buffer,entity[i].x*8,entity[i].y*8,(entity[i].x*8)+15,(entity[i].y*8)+15,
                 makecol(255,164,164));
          }
        break;
        case 2: //Platform
          teststring="P ";
          if(entity[i].p1==0) teststring+="v";
          if(entity[i].p1==1) teststring+="^";
          if(entity[i].p1==2) teststring+="<";
          if(entity[i].p1==3) teststring+=">";
          if(entity[i].p1==4) teststring+="=";

          textout_ex(buffer, font, teststring.c_str(), (entity[i].x*8)+4,(entity[i].y*8),
                       makecol(255,255,255), -1);
          if(i==temp){
            teststring=its(entity[i].p2);
            textout_ex(buffer, font, teststring.c_str(), (entity[i].x*8)+4,(entity[i].y*8)+9,
                         makecol(255,255,255), -1);

            rect(buffer,entity[i].x*8,entity[i].y*8,(entity[i].x*8)+31,(entity[i].y*8)+7,
               makecol(0,0,255));
            rect(buffer,entity[i].p3,entity[i].p4,entity[i].p5-1,entity[i].p6-1,
                 makecol(255,128,0));
          }else{
            rect(buffer,entity[i].x*8,entity[i].y*8,(entity[i].x*8)+31,(entity[i].y*8)+7,
               makecol(164,164,255));
          }
        break;
        case 8: //Small Pickup
          rect(buffer,entity[i].x*8,entity[i].y*8,(entity[i].x*8)+7,(entity[i].y*8)+7,
               makecol(164,164,255));
        break;
        case 9: //Big Pickup
          rect(buffer,entity[i].x*8,entity[i].y*8,(entity[i].x*8)+15,(entity[i].y*8)+15,
               makecol(164,164,255));
        break;
        case 10: //Savepoint
          if(entity[i].p1==0){
            //on the roof!
            teststring="r";
            textout_ex(buffer, font, teststring.c_str(), (entity[i].x*8)+4,entity[i].y*8,
                       makecol(255,255,255), -1);
            teststring="v";
            textout_ex(buffer, font, teststring.c_str(), (entity[i].x*8)+4,(entity[i].y*8)+8,
                       makecol(255,255,255), -1);
          }else{
            //on the floor!
            teststring="^";
            textout_ex(buffer, font, teststring.c_str(), (entity[i].x*8)+4,entity[i].y*8,
                       makecol(255,255,255), -1);
            teststring="f";
            textout_ex(buffer, font, teststring.c_str(), (entity[i].x*8)+4,(entity[i].y*8)+8,
                       makecol(255,255,255), -1);
          }
          rect(buffer,entity[i].x*8,entity[i].y*8,(entity[i].x*8)+15,(entity[i].y*8)+15,
               makecol(164,255,164));
        break;
        case 11:
          if(i==temp){
            rect(buffer,entity[i].x*8,entity[i].y*8,(entity[i].x*8)+entity[i].p1-1,(entity[i].y*8)+7,
                 makecol(0,255,0));
            line(buffer,entity[i].x*8,(entity[i].y*8)+3,(entity[i].x*8)+entity[i].p1-1,(entity[i].y*8)+3,
                 makecol(0,255,0));
            line(buffer,entity[i].x*8,(entity[i].y*8)+4,(entity[i].x*8)+entity[i].p1-1,(entity[i].y*8)+4,
                 makecol(0,255,0));
          }else{
            rect(buffer,entity[i].x*8,entity[i].y*8,(entity[i].x*8)+entity[i].p1-1,(entity[i].y*8)+7,
                 makecol(164,255,164));
            line(buffer,entity[i].x*8,(entity[i].y*8)+3,(entity[i].x*8)+entity[i].p1-1,(entity[i].y*8)+3,
                 makecol(164,255,164));
            line(buffer,entity[i].x*8,(entity[i].y*8)+4,(entity[i].x*8)+entity[i].p1-1,(entity[i].y*8)+4,
                 makecol(164,255,164));
          }
        break;
        case 12: //vertical
          if(i==temp){
            rect(buffer,entity[i].x*8,entity[i].y*8,(entity[i].x*8)+7,(entity[i].y*8)-1+entity[i].p1,
                 makecol(0,255,0));
            line(buffer,(entity[i].x*8)+3,(entity[i].y*8),(entity[i].x*8)+3,(entity[i].y*8)-1+entity[i].p1,
                 makecol(0,255,0));
            line(buffer,(entity[i].x*8)+4,(entity[i].y*8),(entity[i].x*8)+4,(entity[i].y*8)-1+entity[i].p1,
                 makecol(0,255,0));

          }else{
            rect(buffer,entity[i].x*8,entity[i].y*8,(entity[i].x*8)+7,(entity[i].y*8)-1+entity[i].p1,
                 makecol(164,255,164));
            line(buffer,(entity[i].x*8)+3,(entity[i].y*8),(entity[i].x*8)+3,(entity[i].y*8)-1+entity[i].p1,
                 makecol(164,255,164));
            line(buffer,(entity[i].x*8)+4,(entity[i].y*8),(entity[i].x*8)+4,(entity[i].y*8)-1+entity[i].p1,
                 makecol(164,255,164));
          }
        break;
      }
    }
    //GUI
    rect(buffer, 4,230,13,239,makecol(128,128,128));
    drawtile(5, 231, ctile);
    print(buffer, 24, 231, "Tile " + its(ctile), 255,255,255);
    tempstring="("+its(xp)+","+its(yp)+") ["+its(mapx)+","+its(mapy)+"]";
    print(buffer, 200, 231, tempstring, 196,196,255);

    if(savednote>0){
      if(savednote>70){
        temp=200-((90-savednote)*10);
        print(buffer, 24, 115, "[Map Saved]", 255-temp,255-temp,255-temp, true);
      }else if(savednote<20){
        temp=(20-savednote)*10;
        print(buffer, 24, 115, "[Map Saved]", 255-temp,255-temp,255-temp, true);
      }else{
        print(buffer, 24, 115, "[Map Saved]", 255,255,255, true);
      }

      savednote--;
    }


    rect(buffer, (xp*8), (yp*8), (xp*8)+7, (yp*8)+7, makecol(255,128,128));

    if(test) print(buffer, 5, 5, teststring, 255,255,255);

    //Instructions
    //print(buffer, 5, 5, "Simple Tool framework", 255,255,255);

    stretch_blit(buffer, biggerbuffer, 0, 0, 320, 240, 0, 0, 640, 480);
    rprint(biggerbuffer, 5, 5, "Current Area: [" + currentarea+"]", 128,128, 255);
    rprint(biggerbuffer, 5, 20, "R[Hori] T[Vert] Y[Plat] U[Coins] I[Shiny] O[Enemy] P[Save]", 196,196,196);
    rprint(biggerbuffer, 5, 30, "Q[Edge] W[AntiEdge] A+[Move] S+[Invert] F[Fill]", 196,196,196);

    rprint(biggerbuffer, 5, 20, "R       T       Y       U        I        O        P      ", 128,128, 255);
    rprint(biggerbuffer, 5, 30, "Q       W           A+       S+         F      ", 128,128, 255);

    //print(biggerbuffer, 5, 5, its(numentities), 255,255,255);
    //for(int i=0; i<numentities; i++){
    //  tempstring="entity["+its(i)+"]=("+its(entity[i].x)+","+its(entity[i].y)+","+its(entity[i].t)+")";
    //  print(biggerbuffer, 5, 15+(i*10), tempstring, 255,255,255);
   // }

    blit(biggerbuffer, page[currentpage], 0, 0, 0, 0, 640, 480);
    //Instructions, more info
    show_video_bitmap(page[currentpage]);
    currentpage = (currentpage+1)%3;
    //Input
    if(mouse_b & 1){
      if(yp<29){
        //Left click
        if(key[KEY_Z]){
          mapput(xp-1, yp-1, ctile);
          mapput(xp,   yp-1, ctile);
          mapput(xp+1, yp-1, ctile);
          mapput(xp-1, yp,   ctile);
          mapput(xp,   yp,   ctile);
          mapput(xp+1, yp,   ctile);
          mapput(xp-1, yp+1, ctile);
          mapput(xp,   yp+1, ctile);
          mapput(xp+1, yp+1, ctile);
        }else{
          mapput(xp, yp, ctile);
        }
      }
    }
    if(mouse_b & 2){
      //Right click
      if(yp<29){
        ctile=mapcontents[xp][yp];
      }
      for(int i=0; i<numentities; i++){
        if(entity[i].x==xp && entity[i].y==yp){
          removeentity(i);
        }
      }
    }

    if(tileselectdelay<=0){
      if(key[KEY_COMMA]){
        ctile--;
        tileselectdelay=10;
      }
      if(key[KEY_STOP]){
        ctile++;
        tileselectdelay=10;
      }
      if(key[KEY_CLOSEBRACE]){
        ctile+=40;
        tileselectdelay=10;
      }
      if(key[KEY_OPENBRACE]){
        ctile-=40;
        tileselectdelay=10;
      }
      if(key[KEY_A]){
        if(key[KEY_UP])   { mapy--; loadmap(mapx,mapy); tileselectdelay=10; }
        if(key[KEY_DOWN]) { mapy++; loadmap(mapx,mapy); tileselectdelay=10; }
        if(key[KEY_LEFT]) { mapx--; loadmap(mapx,mapy); tileselectdelay=10; }
        if(key[KEY_RIGHT]){ mapx++; loadmap(mapx,mapy); tileselectdelay=10; }
      }
    }else{
      tileselectdelay--;
    }
    if(ctile<0) ctile+=1200;
    if(ctile>=1200) ctile-=1200;

    //Shortcuts
    if(tileselectdelay<=0){
      tileselectdelay=20;
      if(key[KEY_B]){ctile=680;} //Backgrounds
      else if(key[KEY_V]){ctile=80;} //Foregrounds
      else if(key[KEY_1]){p=80;if(ctile==200+p){ctile=400+p;}else if(ctile==p){ctile=200+p;}else{ctile=p;}}
      else if(key[KEY_2]){p=83;if(ctile==200+p){ctile=400+p;}else if(ctile==p){ctile=200+p;}else{ctile=p;}}
      else if(key[KEY_3]){p=86;if(ctile==200+p){ctile=400+p;}else if(ctile==p){ctile=200+p;}else{ctile=p;}}
      else if(key[KEY_4]){p=89;if(ctile==200+p){ctile=400+p;}else if(ctile==p){ctile=200+p;}else{ctile=p;}}
      else if(key[KEY_5]){p=92;if(ctile==200+p){ctile=400+p;}else if(ctile==p){ctile=200+p;}else{ctile=p;}}
      else if(key[KEY_6]){p=95;if(ctile==200+p){ctile=400+p;}else if(ctile==p){ctile=200+p;}else{ctile=p;}}
      else if(key[KEY_7]){p=98;if(ctile==200+p){ctile=400+p;}else if(ctile==p){ctile=200+p;}else{ctile=p;}}
      else if(key[KEY_8]){p=101;if(ctile==200+p){ctile=400+p;}else if(ctile==p){ctile=200+p;}else{ctile=p;}}
      else if(key[KEY_9]){ctile=680;}
      else{
        tileselectdelay=0;
      }
    }else{
      tileselectdelay--;
    }

    if(key[KEY_H]){
      //Flip background
      copymap();
      temp=getbackground();
      if(temp>-1){
        temp2=temp+1;
        if(temp2>=7) temp2=0;
        temp=680+(temp*3);
        temp2=680+(temp2*3);
        for(int j=0; j<29; j++){
          for(int i=0; i<40; i++){
            if(mapcontents[i][j]==temp) mapcontents[i][j]=temp2;
          }
        }
        while(key[KEY_H]);
      }
    }

    if(key[KEY_F8]){
      savemap();
      while(key[KEY_F8]);
      savednote=90;
    }

    if(key[KEY_F7]){
      loadmap(mapx,mapy);
      while(key[KEY_F7]);
    }


    if(key[KEY_F]){
      copymap();
      temp=at(xp,yp);
      for(int j=0; j<29; j++){
        for(int i=0; i<40; i++){
          if(mapcontents[i][j]==temp) mapcontents[i][j]=ctile;
        }
      }
    }

    if(key[KEY_G]){
      for(int j=0; j<29; j++){
        for(int i=0; i<40; i++){
          mapcontents[i][j]=ctile;
        }
      }
    }



    if(key[KEY_Q]){
      //Autoedge! Experimental :O
      copymap();
      for(int j=0; j<29; j++){
        for(int i=0; i<40; i++){
          temp=mapcopycontents[i][j];
          mapcontents[i][j]=edge(i, j, temp);
        }
      }
      while(key[KEY_Q]);
    }

    if(key[KEY_W]){
      //Antiedge!
      copymap();
      for(int j=0; j<29; j++){
        for(int i=0; i<40; i++){
          temp=mapcopycontents[i][j];
          mapcontents[i][j]=antiedge(i, j, temp);
        }
      }
      while(key[KEY_W]);
    }

    if(reversekey<=0){
      if(key[KEY_S] && (key[KEY_LEFT]||key[KEY_RIGHT])){
        copymap();
        for(int j=0; j<29; j++){
          for(int i=0; i<40; i++){
            mapcontents[i][j]=mapcopycontents[39-i][j];
          }
        }
        reversekey=10;
      }
      if(key[KEY_S] && (key[KEY_UP]||key[KEY_DOWN])){
        copymap();
        for(int j=0; j<29; j++){
          for(int i=0; i<40; i++){
            mapcontents[i][j]=mapcopycontents[i][28-j];
          }
        }
        reversekey=10;
      }
    }else{
      reversekey--;
    }

    if(key[KEY_F4]){
      if(fullscreen){
        change_windowed();
        fullscreen=false;
      }else{
        change_fullscreen();
        fullscreen=true;
      }
      while(key[KEY_F4]);
    }

    if(entclear(xp,yp)){
      if(key[KEY_R]){ addentity(xp,yp,11,8); while(key[KEY_R]); }
      if(key[KEY_T]){ addentity(xp,yp,12,8); while(key[KEY_T]); }

      if(key[KEY_Y]){ addentity(xp,yp,2); while(key[KEY_Y]); }
      if(key[KEY_U]) addentity(xp,yp,8);
      if(key[KEY_I]) addentity(xp,yp,9);
      if(key[KEY_O]){ addentity(xp,yp,1); while(key[KEY_O]); }
      if(key[KEY_P]){ addentity(xp,yp,10); while(key[KEY_P]); }
    }else{
      temp=entat(xp,yp);
      if(temp>-1){
        if(key[KEY_O]){
          if(entity[temp].t==1){
            entity[temp].p1++;
            if(entity[temp].p1>=4) entity[temp].p1=0;
            while(key[KEY_O]);
          }
        }

        if(key[KEY_Y]){
          if(entity[temp].t==2){
            entity[temp].p1++;
            if(entity[temp].p1>=5) entity[temp].p1=0;
            while(key[KEY_Y]);
          }
        }

        if(key[KEY_P]){
          if(entity[temp].t==10){
            if(entity[temp].p1==0){
              entity[temp].p1=1;
            }else{
              entity[temp].p1=0;
            }
            while(key[KEY_P]);
          }
        }

        if(entity[temp].t==11 || entity[temp].t==12){
          if(paraadjustdelay<=0){
            if(key[KEY_J]){
              if(key[KEY_LEFT]) { entity[temp].p1-=8; paraadjustdelay=4; }
              if(key[KEY_RIGHT]){ entity[temp].p1+=8; paraadjustdelay=4; }
              if(key[KEY_UP]) { entity[temp].p1-=8; paraadjustdelay=4; }
              if(key[KEY_DOWN]){ entity[temp].p1+=8; paraadjustdelay=4; }
              if(entity[temp].p1<=0) entity[temp].p1=0;
            }
          }else{
            paraadjustdelay--;
          }
        }

        if(entity[temp].t==1 || entity[temp].t==2){
          if(paraadjustdelay<=0){
            if(key[KEY_J]){
              if(key[KEY_LEFT]) { entity[temp].p2--; paraadjustdelay=4; }
              if(key[KEY_RIGHT]){ entity[temp].p2++; paraadjustdelay=4; }
              if(entity[temp].p2<=0) entity[temp].p2=0;
            }
            if(key[KEY_K]){
              if(key[KEY_LEFT]) { entity[temp].p3-=8; paraadjustdelay=4; }
              if(key[KEY_RIGHT]){ entity[temp].p3+=8; paraadjustdelay=4; }
              if(key[KEY_UP]) { entity[temp].p4-=8; paraadjustdelay=4; }
              if(key[KEY_DOWN]){ entity[temp].p4+=8; paraadjustdelay=4; }
            }
            if(key[KEY_L]){
              if(key[KEY_LEFT]) { entity[temp].p5-=8; paraadjustdelay=4; }
              if(key[KEY_RIGHT]){ entity[temp].p5+=8; paraadjustdelay=4; }
              if(key[KEY_UP]) { entity[temp].p6-=8; paraadjustdelay=4; }
              if(key[KEY_DOWN]){ entity[temp].p6+=8; paraadjustdelay=4; }
            }
          }else{
            paraadjustdelay--;
          }
        }
      }
    }

    if(key[KEY_BACKSPACE]){
      numentities=0;
    }

    /*if(key[KEY_F1]) currentarea="otherlevel";
    if(key[KEY_F2]) currentarea="spacestation";
    if(key[KEY_F3]) currentarea="lab";*/

    //Logic
    gt++;
    while(speed_counter<1) rest(1); speed_counter = 0;
  }

	return 0;
}
END_OF_MAIN()
