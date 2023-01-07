#include "Constants.h"
#include "Enums.h"
#include "Game.h"
#include "Graphics.h"
#include "GraphicsUtil.h"
#include "Localization.h"
#include "KeyPoll.h"
#include "UtilityClass.h"
#include "VFormat.h"

static int pre_fakepercent=0, pre_transition=30;
static bool pre_startgame=false;
static SDL_Color pre_darkcol = {0, 0, 0, 0};
static SDL_Color pre_lightcol = {0, 0, 0, 0};
static int pre_curcol = 0, pre_coltimer = 0, pre_offset = 0;

static int pre_frontrectx=30, pre_frontrecty=20, pre_frontrectw=260, pre_frontrecth=200;
static int pre_temprectx=0, pre_temprecty=0, pre_temprectw=320, pre_temprecth=240;

void preloaderinput(void)
{
  game.press_action = false;

  if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v) || key.isDown(game.controllerButton_flip)) {
    game.press_action = true;
  }

  if (game.press_action) {
    //Skip to TITLEMODE immediately
    game.gamestate = TITLEMODE;
    game.jumpheld = true;
  }
}

void preloaderrenderfixed(void)
{
  if (pre_transition < 30) pre_transition--;
  if(pre_transition>=30){
    pre_fakepercent++;
    if (pre_fakepercent >= 100) {
      pre_fakepercent = 100;
      pre_startgame = true;
    }

    pre_offset = (pre_offset + 4 + int(fRandom() * 5.0f))%32;
    pre_coltimer--;
    if (pre_coltimer <= 0) {
      pre_curcol = (pre_curcol + int(fRandom() * 5.0f)) % 6;
      pre_coltimer = 8;
    }
  }

  if (pre_transition <= -10) {
    game.gamestate = TITLEMODE;
  }
}

void preloaderrender(void)
{
  bool print_percentage = false;

  if(pre_transition>=30){
    switch(pre_curcol) {
    case 0:
      pre_lightcol = graphics.getRGB(0xBF,0x59,0x6F);
      pre_darkcol = graphics.getRGB(0x88,0x3E,0x53);
      break;
    case 1:
      pre_lightcol = graphics.getRGB(0x6C,0xBC,0x5C);
      pre_darkcol = graphics.getRGB(0x50,0x86,0x40);
      break;
    case 2:
      pre_lightcol = graphics.getRGB(0x5D,0x57,0xAA);
      pre_darkcol = graphics.getRGB(0x2F,0x2F,0x6C);
      break;
    case 3:
      pre_lightcol = graphics.getRGB(0xB7,0xBA,0x5E);
      pre_darkcol = graphics.getRGB(0x84,0x83,0x42);
      break;
    case 4:
      pre_lightcol = graphics.getRGB(0x57,0x90,0xAA);
      pre_darkcol = graphics.getRGB(0x2F,0x5B,0x6C);
      break;
    case 5:
      pre_lightcol = graphics.getRGB(0x90,0x61,0xB1);
      pre_darkcol = graphics.getRGB(0x58,0x3D,0x71);
      break;
    default:
      pre_lightcol = graphics.getRGB(0x00,0x00,0x00);
      pre_darkcol = graphics.getRGB(0x08,0x00,0x00);
      break;
    }

    for (int i = 0; i < 18; i++) {
      pre_temprecty = (i * 16)- pre_offset;
      if (i % 2 == 0)
      {
        graphics.fill_rect(pre_temprectx, pre_temprecty, pre_temprectw,pre_temprecth, pre_lightcol);
      }
      else
      {
        graphics.fill_rect(pre_temprectx, pre_temprecty, pre_temprectw,pre_temprecth, pre_darkcol);
      }
    }

    graphics.fill_rect(pre_frontrectx, pre_frontrecty, pre_frontrectw,pre_frontrecth, graphics.getRGB(0x3E,0x31,0xA2));

    print_percentage = true;

    //Render
    if (pre_startgame) {
      pre_transition = 29;
    }
  }else if (pre_transition <= -10) {
    //Switch to TITLEMODE (handled by preloaderrenderfixed)
  }else if (pre_transition < 5) {
    graphics.fill_rect(0, 0, 0);
  }else if (pre_transition < 20) {
    pre_temprecty = 0;
    pre_temprecth = 240;
    graphics.fill_rect(0, 0, 0);
    graphics.fill_rect(pre_frontrectx, pre_frontrecty, pre_frontrectw,pre_frontrecth, graphics.getRGB(0x3E,0x31,0xA2));

    print_percentage = true;
  }

  if (print_percentage) {
    char buffer[SCREEN_WIDTH_CHARS + 1];
    vformat_buf(
      buffer, sizeof(buffer),
      loc::gettext("LOADING... {percent|digits=2|spaces}%"),
      "percent:int",
      pre_fakepercent
    );

    int percentage_len = graphics.len(buffer);
    graphics.Print(282-percentage_len, 204, buffer, 124, 112, 218, false);
  }

  graphics.drawfade();

  graphics.render();
}
