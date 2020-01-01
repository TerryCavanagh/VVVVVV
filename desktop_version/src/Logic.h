#ifndef LOGIC_H
#define LOGIC_H

#include "Graphics.h"
#include "Game.h"
#include "Entity.h"
#include "UtilityClass.h"
#include "Music.h"
#include "Map.h"

void titlelogic(Graphics& dwgfx, Game& game, entityclass& obj, UtilityClass& help, musicclass& music, mapclass& map);

void maplogic(Graphics& dwgfx, Game& game, entityclass& obj,  musicclass& music, mapclass& map, UtilityClass& help);

void gamecompletelogic(Graphics& dwgfx, Game& game, entityclass& obj,  musicclass& music, mapclass& map, UtilityClass& help);

void gamecompletelogic2(Graphics& dwgfx, Game& game, entityclass& obj,  musicclass& music, mapclass& map, UtilityClass& help);

void towerlogic(Graphics& dwgfx, Game& game, entityclass& obj,  musicclass& music, mapclass& map, UtilityClass& help);

void gamelogic(Graphics& dwgfx, Game& game, entityclass& obj,  musicclass& music, mapclass& map, UtilityClass& help);

#endif /* LOGIC_H */
