#include <string>
#include <sstream>
#include <fstream>
#include <allegro.h>
#include <loadpng.h>
using namespace std;

volatile int speed_counter=0;
void increment_speed_counter(){speed_counter++;}
END_OF_FUNCTION(increment_speed_counter);

#include "misc.hpp"
#include "map.hpp"
#include "logic.hpp"
#include "render.hpp"
#include "input.hpp"

int main(){
  init();

  //state=1;

  //Starting main loop:
  while(!key[KEY_ESC]){
    xp=mouse_x/16;  yp=mouse_y/16;
    switch(state){
      case 0: //Map Edit
        //Render
        mapeditrender();
        //Input
        mapeditinput();
        //Logic
      break; //World Edit
      case 1:
        //Render
        arearender();
        //Input
        areainput();
        //Logic
      break;
    }

    /*if(key[KEY_F1]) currentarea="otherlevel";
    if(key[KEY_F2]) currentarea="spacestation";
    if(key[KEY_F3]) currentarea="lab";*/
    gt++;
    while(speed_counter<1) rest(1); speed_counter = 0;
  }

	return 0;
}
END_OF_MAIN()
