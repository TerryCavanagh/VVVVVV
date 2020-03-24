#ifndef TITLERENDERER_H
#define TITLERENDERER_H

#include "Graphics.h"
#include "UtilityClass.h"
#include "Maths.h"
#include "Entity.h"
#include "Map.h"
#include "Script.h"

class Stage
{
public:
    int frameRate;
};

extern Stage stage;
extern Stage swfStage;
extern int temp;

void titlerender();

void towerrender();

void gamerender();

void maprender();

void teleporterrender();

void gamecompleterender();

void gamecompleterender2();

#endif /* TITLERENDERER_H */
