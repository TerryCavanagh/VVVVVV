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

  for(int j=0; j<30; j++){
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

void saveareamapimport(){
  string s = "areamapin.txt";
  ofstream file;
  file.open(s.c_str(), ios::trunc);

  for(int j=0; j<20; j++){
    for(int i=0; i<20; i++){
      file << areamap[i][j] << " ";
    }
    file << "\n";
  }

  file.close();
}

void saveareamap(){
  saveareamapimport();
  string s = "areamap.txt";
  ofstream file;
  file.open(s.c_str(), ios::trunc);

  file << "tmap = new Array();\n";

  for(int j=0; j<20; j++){
    file << "tmap.push(\"";
    for(int i=0; i<19; i++){
      file << areamap[i][j] << ",";
    }
    file << areamap[19][j] << "\");\n";
  }

  file.close();
}

bool existmap(int x, int y){
  //returns true if an "other" map exists at x, y
  x+=100;
  y+=100;
  string s = "imports/other/x"+its(x)+"y"+its(y)+".txt";

  ifstream file;
  file.open(s.c_str());
  if(!file){
    return false;
  }
  file.close();
  return true;
}

bool loadmap(int x, int y){
  //set current area based on area
  maptileset=1;
  temp=areamap[x-100][y-100];
  switch(temp){
    case 0: currentarea="unset"; break;
    case 1: currentarea="other"; break;
    case 2: currentarea="lab"; break;
    case 3: currentarea="tower"; break;
    case 4: currentarea="warp"; break;
    case 5: currentarea="spacestation2"; break;
  }

  //set area offset
  x=x-100;
  y=y-100;

  switch(temp){
    case 0: currentarea="unset"; break;
    case 1:
      x+=100;
      y+=100;
    break;
    case 2:
      if(y<5){
        x+=50-2; y+=54;    //lab
      }else{
        x+=50-2; y+=50-16; //lab
      }
    break;
    case 3: currentarea="tower"; break;
    case 4:
      x+=50-14; y+=49;   //warp
    break;
    case 5:
      x+=50-12; y+=50-14;   //Space Station
    break;
  }

  int xp, yp, t, n;
  int p1, p2, p3, p4, p5, p6;
  string s = "imports/"+currentarea+"/x"+its(x)+"y"+its(y)+".txt";

  ifstream file;
  file.open(s.c_str());
  if(!file){
    for(int j=1; j<29; j++){
      for(int i=1; i<39; i++){
        mapcontents[i][j]=0;
      }
    }
    numentities=0;
    return false;
  }

  for(int j=0; j<30-(temp>2); j++){
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

void supersavemap(){
  //holy crap
  int tmpx, tmpy;
  int ccount=0, scount=0;
  string s = "overworld.txt";
  ofstream file;
  file.open(s.c_str(), ios::trunc);

  tmpx=mapx; tmpy=mapy;
  for(int i=0; i<20; i++){
    for(int j=0; j<20; j++){
      if(existmap(i,j)){
         mapx=i; mapy=j;
         loadmap(i+100,j+100);
         scount=0;
        file << "case rn("<< its(mapx) << ","<< its(mapy) << "):\n";
    file << "tmap = new Array();\n";

    for(int j=0; j<30; j++){
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
        }else if(entity[i].t==9){
          //Ok, we're looking for the warp token signiture, which is a teleporter on top of
          //a shiny trinket. So we need to check the other entities here:
          for(int j=0; j<numentities; j++){
            if(entity[j].t==8){
              if(entity[j].x+1==entity[i].x){
                if(entity[j].y+1==entity[i].y){
                  //It's a warp token!
                  entity[i].t=13;
                  entity[j].t=14;
                }
              }
            }
          }

          if(entity[i].t==9){
            file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)
                 << ", " << entity[i].t;
          }
        }else if(entity[i].t==8){
          file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)
                << ", 14); //Teleporter!";
        }else if(entity[i].t==2 && entity[i].p1==4){
          entity[i].t=3;
        }else if(entity[i].t==2 && entity[i].p1==5){
          entity[i].t=4;
        }else if(entity[i].t==2 && entity[i].p1==6){
          entity[i].t=4;
        }else if(entity[i].t==2 && entity[i].p1==7){
          entity[i].t=4;
        }else if(entity[i].t==2 && entity[i].p1==8){
          entity[i].t=4;
        }else if(entity[i].t==10 && entity[i].x==0 && entity[i].y==0){
          //This means use a different tileset, so don't output a savepoint
        }else{
          if(entity[i].t!=14){
            file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)
                 << ", " << entity[i].t;
          }
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
            file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)
                 << ", 3);  //Disappearing Platform";
            entity[i].t=2; //So that it doesn't disapear from the editor
          break;
          case 4:
            if(entity[i].p1==5){
              file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)
                   << ", 2, 8, " << entity[i].p2 << ");  //Threadmill, >>>";
            }else if(entity[i].p1==6){
              file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)
                   << ", 2, 9, " << entity[i].p2 << ");  //Threadmill, <<<";
            }else if(entity[i].p1==7){
              file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)
                   << ", 2, 10, " << entity[i].p2 << ");  //Big Threadmill, >>>>>>";
            }else if(entity[i].p1==8){
              file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)
                   << ", 2, 11, " << entity[i].p2 << ");  //Big Threadmill, <<<<<<";
            }

            entity[i].t=2; //So that it doesn't disapear from the editor
          break;
          case 8: //coin
            //file << ", " << entity[i].p1 << ", coin");  // (coins)";
            //ccount++;
          break;
          case 9: //shiny
            file << ", " << ccount <<");  // (shiny trinket)";
            ccount++;
          break;
          case 10: //Save point
            if(entity[i].x==0 && entity[i].y==0){
              file << "roomtileset = 0; // (Use space station tileset)";
            }else{
              file << ", " << entity[i].p1 << ", " << int(scount+((mapx+(mapy*100))*10)) << ");  // (savepoint)";
              scount++;
            }
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
          case 13: //Warp Token
            entity[i].t=9;
            file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)
                << ", 13); //Warp Token";
          break;
          case 14: //Warp Token aid
            entity[i].t=8;
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
    for(int j=0; j<30; j++){
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

    file << "break;\n\n";


      }
    }
  }
  mapx=tmpx; mapy=tmpy;

  file.close();
}

void supersaveminimap(){
  //holy crap
  int tmpx, tmpy;
  int ccount=0, scount=0;
  string s = "overworld.txt";
  ofstream file;
  file.open(s.c_str(), ios::trunc);

  tmpx=mapx; tmpy=mapy;

  clear_to_color(worldmap, makecol(0,0,0));

  for(int i=0; i<20; i++){
    for(int j=0; j<20; j++){
      if(existmap(i,j)){
        mapx=i; mapy=j;
        loadmap(i+100,j+100);
        scount=0;
        //Output pixels to bitmap here
        for(int j2=0;j2<30; j2++){
          for(int i2=0;i2<40; i2++){
            if(mapcontents[i2][j2]<80){
              putpixel(worldmap, i2+(i*40), j2+(j*30), makecol(0,0,0));
            }else if(mapcontents[i2][j2]>=80 && mapcontents[i2][j2]<280){
              putpixel(worldmap, i2+(i*40), j2+(j*30), makecol(164,164,164));
            }else if(mapcontents[i2][j2]>=280 && mapcontents[i2][j2]<680){
              putpixel(worldmap, i2+(i*40), j2+(j*30), makecol(128,128,128));
            }else{
              putpixel(worldmap, i2+(i*40), j2+(j*30), makecol(64,64,64));
            }
          }
        }
      }
    }
  }

  //Ok, we have a complete map, now output it!
  savescreen(worldmap);

  mapx=tmpx; mapy=tmpy;

  file.close();
}


void savemap(){
  int ccount=0, scount=0;
  temp=areamap[mapx-100][mapy-100];
  switch(temp){
    case 0: currentarea="unset"; break;
    case 1: currentarea="other"; break;
    case 2: currentarea="lab"; break;
    case 3: currentarea="tower"; break;
    case 4: currentarea="warp"; break;
    case 5: currentarea="spacestation2"; break;
  }

  if(temp==0){
    temp=1;
    areamap[mapx-100][mapy-100]=1;
    currentarea="other";
    saveareamap();
  }
  if(temp<2){
    savemapsimple();
    string s = "maps/"+currentarea+"/x"+its(mapx)+"y"+its(mapy)+".txt";
    ofstream file;
    file.open(s.c_str(), ios::trunc);

    file << "case rn("<< its(mapx) << ","<< its(mapy) << "):\n";
    file << "tmap = new Array();\n";

    for(int j=0; j<30; j++){
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
        }else if(entity[i].t==8){
          file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)
                << ", 14); //Teleporter!";
        }else if(entity[i].t==2 && entity[i].p1==4){
          entity[i].t=3;
        }else if(entity[i].t==2 && entity[i].p1==5){
          entity[i].t=4;
        }else if(entity[i].t==2 && entity[i].p1==6){
          entity[i].t=4;
        }else if(entity[i].t==2 && entity[i].p1==7){
          entity[i].t=4;
        }else if(entity[i].t==2 && entity[i].p1==8){
          entity[i].t=4;
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
            file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)
                 << ", 3);  //Disappearing Platform";
            entity[i].t=2; //So that it doesn't disapear from the editor
          break;
          case 4:
            if(entity[i].p1==5){
              file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)
                   << ", 2, 8, " << entity[i].p2 << ");  //Threadmill, >>>";
            }else if(entity[i].p1==6){
              file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)
                   << ", 2, 9, " << entity[i].p2 << ");  //Threadmill, <<<";
            }else if(entity[i].p1==7){
              file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)
                   << ", 2, 10, " << entity[i].p2 << ");  //Big Threadmill, >>>>>>";
            }else if(entity[i].p1==8){
              file << "obj.createentity(game, " << (entity[i].x*8) << ", " << (entity[i].y*8)
                   << ", 2, 11, " << entity[i].p2 << ");  //Big Threadmill, <<<<<<";
            }

            entity[i].t=2; //So that it doesn't disapear from the editor
          break;
          case 8: //coin
            //file << ", " << entity[i].p1 << ", coin+"<<ccount<<");  // (coins)";
            //ccount++;
          break;
          case 9: //shiny
            file << ", "<<ccount<<");  // (shiny trinket)";
            ccount++;
          break;
          case 10: //Save point
            if(currentarea=="otherlevel") mapy+=100;
            if(currentarea=="lab") mapy+=200;
            if(currentarea=="spacestation") mapy+=300;
            if(currentarea=="spacestation2") mapy+=400;


            file << ", " << entity[i].p1 << ", " << int(scount+((mapx+(mapy*100))*10)) << ");  // (savepoint)";
            scount++;

            if(currentarea=="otherlevel") mapy-=100;
            if(currentarea=="lab") mapy-=200;
            if(currentarea=="spacestation") mapy-=300;
            if(currentarea=="spacestation2") mapy-=400;
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
    for(int j=0; j<30; j++){
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
}

void copymap(){
  for(int j=0; j<30; j++){
    for(int i=0; i<40; i++){
       mapcopycontents[i][j]=mapcontents[i][j];
    }
  }
}

void mapput(int x, int y, int t){
  if(x>=0 && x<40 && y>=0 && y<30){
    mapcontents[x][y]=t;
  }
}

int at(int x, int y){
  if(x>=0 && x<40 && y>=0 && y<30){
    return mapcopycontents[x][y];
  }
  if(x==-1) return at(x+1,y);
  if(x==40) return at(x-1,y);
  if(y==-1) return at(x,y+1);
  if(y==30) return at(x,y-1);
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

  for(int j=0; j<30; j++){
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
  for(int i=0; i<13; i++){
    if(inbox(0+(i*3),2,3+(i*3),7,x,y)) return 80+(i*3);
    if(inbox(0+(i*3),7,3+(i*3),12,x,y)) return 280+(i*3);
    if(inbox(0+(i*3),12,3+(i*3),17,x,y)) return 480+(i*3);
    if(inbox(0+(i*3),17,3+(i*3),22,x,y)) return 680+(i*3);
  }
  return t;
}

int edge(int x, int y, int t){
  temp=1;
  if(t>=880 && t<920){
    temp=t-880;
  }else if(t>=680 && t<720){
    temp=t-680;
  }else if(t>=480 && t<520){
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

void drawentities(){
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
        if(entity[i].p1==5) teststring=">>>";
        if(entity[i].p1==6) teststring="<<<";
        if(entity[i].p1==7) teststring="> > > >";
        if(entity[i].p1==8) teststring="< < < <";

        textout_ex(buffer, font, teststring.c_str(), (entity[i].x*8)+4,(entity[i].y*8),
                     makecol(255,255,255), -1);
        if(i==temp){
          teststring=its(entity[i].p2);
          textout_ex(buffer, font, teststring.c_str(), (entity[i].x*8)+4,(entity[i].y*8)+9,
                       makecol(255,255,255), -1);

          if(entity[i].p1<7){
            rect(buffer,entity[i].x*8,entity[i].y*8,(entity[i].x*8)+31,(entity[i].y*8)+7,
                 makecol(0,0,255));
            rect(buffer,entity[i].p3,entity[i].p4,entity[i].p5-1,entity[i].p6-1,
                 makecol(255,128,0));
          }else{
            rect(buffer,entity[i].x*8,entity[i].y*8,(entity[i].x*8)+31+32,(entity[i].y*8)+7,
                 makecol(0,0,255));
            rect(buffer,entity[i].p3,entity[i].p4,entity[i].p5-1,entity[i].p6-1,
                 makecol(255,128,0));
          }
        }else{

          if(entity[i].p1<7){
            rect(buffer,entity[i].x*8,entity[i].y*8,(entity[i].x*8)+31,(entity[i].y*8)+7,
               makecol(164,164,255));
          }else{
            rect(buffer,entity[i].x*8,entity[i].y*8,(entity[i].x*8)+31+32,(entity[i].y*8)+7,
               makecol(164,164,255));
          }
        }
      break;
      case 8: //Small Pickup
        rect(buffer,entity[i].x*8,entity[i].y*8,(entity[i].x*8)+95,(entity[i].y*8)+95,
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
}
