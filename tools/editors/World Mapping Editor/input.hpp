void mapeditinput(){
  if(mouse_b & 1){
    if(yp<30){
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
    if(yp<30){
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
      if(key[KEY_UP])   { mapy--; if(mapy<100) mapy=119; loadmap(mapx,mapy); tileselectdelay=10; }
      if(key[KEY_DOWN]) { mapy++; if(mapy>119) mapy=100; loadmap(mapx,mapy); tileselectdelay=10; }
      if(key[KEY_LEFT]) { mapx--; if(mapx<100) mapx=119; loadmap(mapx,mapy); tileselectdelay=10; }
      if(key[KEY_RIGHT]){ mapx++; if(mapx>119) mapx=100; loadmap(mapx,mapy); tileselectdelay=10; }
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
    else if(key[KEY_9]){p=104;if(ctile==200+p){ctile=400+p;}else if(ctile==p){ctile=200+p;}else{ctile=p;}}
    else if(key[KEY_0]){p=107;if(ctile==200+p){ctile=400+p;}else if(ctile==p){ctile=200+p;}else{ctile=p;}}
    else if(key[KEY_MINUS]){p=110;if(ctile==200+p){ctile=400+p;}else if(ctile==p){ctile=200+p;}else{ctile=p;}}
    else if(key[KEY_EQUALS]){p=113;if(ctile==200+p){ctile=400+p;}else if(ctile==p){ctile=200+p;}else{ctile=p;}}
    else if(key[KEY_BACKSPACE]){p=116;if(ctile==200+p){ctile=400+p;}else if(ctile==p){ctile=200+p;}else{ctile=p;}}
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
      for(int j=0; j<30; j++){
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
    for(int j=0; j<30; j++){
      for(int i=0; i<40; i++){
        if(mapcontents[i][j]==temp) mapcontents[i][j]=ctile;
      }
    }
  }

  if(key[KEY_G]){
    for(int j=0; j<30; j++){
      for(int i=0; i<40; i++){
        mapcontents[i][j]=ctile;
      }
    }
  }



  if(key[KEY_Q]){
    //Autoedge! Experimental :O
    copymap();
    for(int j=0; j<30; j++){
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
    for(int j=0; j<30; j++){
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

  if(key[KEY_L]){
    tileset=(tileset+1)%2;
    while(key[KEY_L]);
  }

  if(entclear(xp,yp)){
    if(key[KEY_R]){ addentity(xp,yp,2,6,4); while(key[KEY_R]); }
    if(key[KEY_T]){ addentity(xp,yp,2,5,4); while(key[KEY_T]); }

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

      if(key[KEY_T]){
        if(entity[temp].t==2){
          if(entity[temp].p1==5){
            entity[temp].p1=7;
          }else if(entity[temp].p1==7){
            entity[temp].p1=5;
          }
          while(key[KEY_T]);
        }
      }

      if(key[KEY_R]){
        if(entity[temp].t==2){
          if(entity[temp].p1==6){
            entity[temp].p1=8;
          }else if(entity[temp].p1==8){
            entity[temp].p1=6;
          }
          while(key[KEY_R]);
        }
      }


      if(key[KEY_Y]){
        if(entity[temp].t==2){
          entity[temp].p1++;
          if(entity[temp].p1>=9) entity[temp].p1=0;
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

  if(key[KEY_F1]){
    while(key[KEY_F1]);
    state=1;
  }
}

void areainput(){
  if(tileselectdelay<=0){
    if(key[KEY_A]){
      if(key[KEY_UP])   { mapy--; if(mapy<100) mapy=119; loadmap(mapx,mapy); tileselectdelay=4; }
      if(key[KEY_DOWN]) { mapy++; if(mapy>119) mapy=100; loadmap(mapx,mapy); tileselectdelay=4; }
      if(key[KEY_LEFT]) { mapx--; if(mapx<100) mapx=119; loadmap(mapx,mapy); tileselectdelay=4; }
      if(key[KEY_RIGHT]){ mapx++; if(mapx>119) mapx=100; loadmap(mapx,mapy); tileselectdelay=4; }
    }
  }else{
    tileselectdelay--;
  }

  if(key[KEY_F1]){
    while(key[KEY_F1]);
    state=0;
  }

  if(key[KEY_F8]){
    supersavemap();
    while(key[KEY_F8]);
    savednote=90;
  }


  if(key[KEY_F12]){
    supersaveminimap();
    while(key[KEY_F12]);
    savednote=90;
  }
}
