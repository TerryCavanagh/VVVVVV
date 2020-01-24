void mapeditrender(){
  if(tileset==0){
    clear_to_color(buffer, makecol(8,8,8));
    for(int j=0; j<30; j++){
    for(int i=0; i<40; i++){
      rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(8,8,8)); //a simple grid
      if(i%4==0) rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(16,16,16));
      if(j%4==0) rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(16,16,16));

      //Minor guides
      if(i==9) rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(24,24,24));
      if(i==30) rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(24,24,24));
      if(j==6 || j==7) rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(24,24,24));
      if(j==21 || j==22) rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(24,24,24));

      //Major guides
      if(i==20 || i==19) rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(32,32,32));
      if(j==14) rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(32,32,32));
    }
  }

  }else{
    clear_to_color(buffer, makecol(0,0,0));
    for(int j=0; j<30; j++){
    for(int i=0; i<40; i++){
      rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(16,16,16)); //a simple grid
      if(i%4==0) rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(32,32,32));
      if(j%4==0) rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(32,32,32));

      //Minor guides
      if(i==9) rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(48,48,48));
      if(i==30) rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(48,48,48));
      if(j==6 || j==7) rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(48,48,48));
      if(j==21 || j==22) rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(48,48,48));

      //Major guides
      if(i==20 || i==19) rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(64,64,64));
      if(j==14) rect(buffer, i*8, j*8, (i*8)+7, (j*8)+7, makecol(64,64,64));
    }
  }

  }


  for(int j=0; j<30; j++){
    for(int i=0; i<40; i++){
      if(mapcontents[i][j]>0) drawtile(i*8, j*8, mapcontents[i][j]);
    }
  }


  //Draw entities
  drawentities();
  //GUI
  rect(buffer, 4,230,13,239,makecol(128,128,128));
  drawtile(5, 231, ctile);
  print(buffer, 24, 231, "Tile " + its(ctile), 255,255,255);
  tempstring="("+its(xp)+","+its(yp)+") ["+its(mapx-100)+","+its(mapy-100)+"]";
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
  r2print(buffer, 5, 5, "[" + currentarea+"]", 128,128, 255);
  if(test) print(buffer, 5, 5, teststring, 255,255,255);

  //Instructions
  //print(buffer, 5, 5, "Simple Tool framework", 255,255,255);

  stretch_blit(buffer, biggerbuffer, 0, 0, 320, 240, 0, 0, 640, 480);
  rprint(biggerbuffer, 5, 35, "R[<<<] T[>>>] Y[Plat] U[Coins] I[Shiny] O[Enemy] P[Save]", 196,196,196);
  rprint(biggerbuffer, 5, 45, "Q[Edge] W[AntiEdge] A+[Move] S+[Invert] F[Fill]", 196,196,196);

  rprint(biggerbuffer, 5, 35, "R      T      Y       U        I        O        P      ", 128,128, 255);
  rprint(biggerbuffer, 5, 45, "Q       W           A+       S+         F      ", 128,128, 255);

  //print(biggerbuffer, 5, 5, its(numentities), 255,255,255);
  //for(int i=0; i<numentities; i++){
  //  tempstring="entity["+its(i)+"]=("+its(entity[i].x)+","+its(entity[i].y)+","+its(entity[i].t)+")";
  //  print(biggerbuffer, 5, 15+(i*10), tempstring, 255,255,255);
 // }

  blit(biggerbuffer, page[currentpage], 0, 0, 0, 0, 640, 480);
  //Instructions, more info
  show_video_bitmap(page[currentpage]);
  currentpage = (currentpage+1)%3;
}

void arearender(){
  clear_to_color(buffer, makecol(0,0,0));
  for(int i=0;i<20; i++){
    for(int j=0; j<20; j++){
      switch(areamap[i][j]){
        case 0:
          for(int i2=0; i2<12; i2++){
            for(int j2=0; j2<12; j2++){
              temp=int(rand()%96);
              putpixel(buffer, i2+(i*12), j2+(j*12), makecol(temp,temp,temp));
            }
          }
          //rect(buffer, i*12, j*12, (i*12)+11, (j*12)+11, makecol(16,16,16));
        break;
        case 1:
          rectfill(buffer, i*12, j*12, (i*12)+11, (j*12)+11, makecol(16,16,16));
          rect(buffer, i*12, j*12, (i*12)+11, (j*12)+11, makecol(64,64,64));
        break;
        case 2:
          rectfill(buffer, i*12, j*12, (i*12)+11, (j*12)+11, makecol(16,16,16));
          rect(buffer, i*12, j*12, (i*12)+11, (j*12)+11, makecol(16,16,164));
        break;
        case 3:
          rectfill(buffer, i*12, j*12, (i*12)+11, (j*12)+11, makecol(16,16,16));
          rect(buffer, i*12, j*12, (i*12)+11, (j*12)+11, makecol(164,16,16));
        break;
        case 4:
          rectfill(buffer, i*12, j*12, (i*12)+11, (j*12)+11, makecol(16,16,16));
          rect(buffer, i*12, j*12, (i*12)+11, (j*12)+11, makecol(16,164,16));
        break;
        case 5:
          rectfill(buffer, i*12, j*12, (i*12)+11, (j*12)+11, makecol(16,16,16));
          rect(buffer, i*12, j*12, (i*12)+11, (j*12)+11, makecol(164,164,16));
        break;
      }
    }
  }

  rect(buffer, (mapx-100)*12, (mapy-100)*12, ((mapx-100)*12)+11, ((mapy-100)*12)+11, makecol(196,196,196));

  tempstring="("+its(xp)+","+its(yp)+") ["+its(mapx-100)+","+its(mapy-100)+"]";
  print(buffer, 200, 231, tempstring, 196,196,255);
  r2print(buffer, 5, 5, "[" + currentarea+"]", 128,128, 255);

  if(savednote>0){
    if(savednote>70){
      temp=200-((90-savednote)*10);
      print(buffer, 24, 115, "[Super Export Complete!]", 255-temp,255-temp,255-temp, true);
    }else if(savednote<20){
      temp=(20-savednote)*10;
      print(buffer, 24, 115, "[Super Export Complete!]", 255-temp,255-temp,255-temp, true);
    }else{
      print(buffer, 24, 115, "[Super Export Complete!]", 255,255,255, true);
    }

    savednote--;
  }

  stretch_blit(buffer, biggerbuffer, 0, 0, 320, 240, 0, 0, 640, 480);
  /*rprint(biggerbuffer, 5, 35, "R[<<<] T[>>>] Y[Plat] U[Coins] I[Shiny] O[Enemy] P[Save]", 196,196,196);
  rprint(biggerbuffer, 5, 45, "Q[Edge] W[AntiEdge] A+[Move] S+[Invert] F[Fill]", 196,196,196);

  rprint(biggerbuffer, 5, 35, "R      T      Y       U        I        O        P      ", 128,128, 255);
  rprint(biggerbuffer, 5, 45, "Q       W           A+       S+         F      ", 128,128, 255);
*/
  //print(biggerbuffer, 5, 5, its(numentities), 255,255,255);
  //for(int i=0; i<numentities; i++){
  //  tempstring="entity["+its(i)+"]=("+its(entity[i].x)+","+its(entity[i].y)+","+its(entity[i].t)+")";
  //  print(biggerbuffer, 5, 15+(i*10), tempstring, 255,255,255);
 // }

  blit(biggerbuffer, page[currentpage], 0, 0, 0, 0, 640, 480);
  //Instructions, more info
  show_video_bitmap(page[currentpage]);
  currentpage = (currentpage+1)%3;
}
