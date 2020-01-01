#ifndef INPUT_H
#define INPUT_H

#include "KeyPoll.h"
#include "Graphics.h"
#include "Game.h"
#include "Entity.h"
#include "UtilityClass.h"
#include "Music.h"
#include "Map.h"

void titleinput(KeyPoll& key, Graphics& dwgfx, mapclass& map, Game& game,
                entityclass& obj, UtilityClass& help, musicclass& music);

void gameinput(KeyPoll& key, Graphics& dwgfx, Game& game, mapclass& map,
               entityclass& obj, UtilityClass& help, musicclass& music);

void mapinput(KeyPoll& key, Graphics& dwgfx, Game& game, mapclass& map,
              entityclass& obj, UtilityClass& help, musicclass& music);

void teleporterinput(KeyPoll& key, Graphics& dwgfx, Game& game, mapclass& map,
                     entityclass& obj, UtilityClass& help, musicclass& music);

void gamecompleteinput(KeyPoll& key, Graphics& dwgfx, Game& game, mapclass& map,
                       entityclass& obj, UtilityClass& help, musicclass& music);

void gamecompleteinput2(KeyPoll& key, Graphics& dwgfx, Game& game, mapclass& map,
                        entityclass& obj, UtilityClass& help, musicclass& music);

#endif /* INPUT_H */
