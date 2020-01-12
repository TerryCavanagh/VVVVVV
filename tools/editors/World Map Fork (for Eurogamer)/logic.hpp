
void fillarea(int x, int y, int w, int h, int t){
  for(int i=x; i<x+w; i++){
    for(int j=y; j<y+h; j++){
      areamap[i][j]=t;
    }
  }
}

void initworld(){
  //Setup the default world, load in any residual info
  for(int i=0; i<20; i++){
    for(int j=0; j<20; j++){
      areamap[i][j]=0;
    }
  }

  //lab
  fillarea(1,0,6,2,2);
  fillarea(4,0,1,5,2);
  areamap[7][0]=2;
  areamap[1][17]=2;
  fillarea(2,4,2,1,2);
  fillarea(0,18,5,2,2);
  fillarea(2,16,6,2,2);
  fillarea(7,15,1,5,2);

  //tower
  fillarea(9,0,1,20,3);
  areamap[8][9]=3;
  fillarea(10,4,2,1,3);

  //warpzone
  areamap[13][0]=4;
  fillarea(14,0,6,3,4);
  fillarea(16,3,4,1,4);

  //space station
  areamap[16][6]=5;
  fillarea(10,5,6,3,5);
  fillarea(12,3,4,3,5);
  fillarea(13,7,6,5,5);
  fillarea(18,7,1,8,5);
  fillarea(11,11,6,3,5);
  fillarea(11,14,2,1,5);

  //If restarting, comment out from here

  string s = "areamapin.txt";

  ifstream file;
  file.open(s.c_str());
  if(!file){
  }else{
    for(int j=0; j<20; j++){
      for(int i=0; i<20; i++){
        file >> areamap[i][j];
      }
    }
  }
}

void init(){
  test=false; teststring="TEST = True";
  state=0; gt=0; tileset=1;
  savednote=0; numentities=0; paraadjustdelay=0;
  currentarea="unset";
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
  set_color_depth(16);
  set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
  register_png_file_type();
  currentpage=0;
  fullscreen=false;

  worldmap=create_bitmap(800,600);

  buffer=load_png("../../engine/current/data/graphics/tiles.png", dummypal);

  for(int y=0;y<30;y++){
    for(int x=0;x<40;x++){
      tiles[x+(y*40)] = create_bitmap(8, 8);
      rectfill(tiles[x+(y*40)], 0, 0, 8, 8, makecol(16,16,16));
      blit(buffer, tiles[x+(y*40)], x*8, y*8, 0, 0, 8,8);
    }
  }
  destroy_bitmap(buffer);

  buffer=load_png("../../engine/current/data/graphics/tiles2.png", dummypal);
  for(int y=0;y<30;y++){
    for(int x=0;x<40;x++){
      tiles2[x+(y*40)] = create_bitmap(8, 8);
      rectfill(tiles2[x+(y*40)], 0, 0, 8, 8, makecol(16,16,16));
      blit(buffer, tiles2[x+(y*40)], x*8, y*8, 0, 0, 8,8);
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
  mapx=100; mapy=100;
  initworld();
  if(!loadmap(mapx, mapy)){
    for(int j=0; j<30; j++){
      for(int i=0; i<40; i++){
        mapcontents[i][j]=0;
      }
    }
  }
  xp=0; yp=0; ctile=0;
  tileselectdelay=0;
  reversekey=0;
}
