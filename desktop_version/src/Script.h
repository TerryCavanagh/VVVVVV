#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>
#include <vector>

#include "Enums.h"


class KeyPoll; class Graphics; class Game; class mapclass; class entityclass; class UtilityClass;class musicclass;


class scriptclass
{
public:


    scriptclass();

	void load(std::string t);
	void loadother(std::string t);


    void inline add(std::string t)
    {
        commands.push_back(t);
    }

    void clearcustom();

    void tokenize(std::string t);

    void run(KeyPoll& key, Graphics& dwgfx, Game& game, mapclass& map,
             entityclass& obj, UtilityClass& help, musicclass& music);

    void resetgametomenu(Graphics& dwgfx, Game& game,mapclass& map,
                         entityclass& obj, UtilityClass& help, musicclass& music);

    void startgamemode(int t, KeyPoll& key, Graphics& dwgfx, Game& game, mapclass& map,
                       entityclass& obj, UtilityClass& help, musicclass& music);

    void teleport(Graphics& dwgfx, Game& game, mapclass& map,
                  entityclass& obj, UtilityClass& help, musicclass& music);

    void hardreset(KeyPoll& key, Graphics& dwgfx, Game& game,mapclass& map,
                   entityclass& obj, UtilityClass& help, musicclass& music);

    //Script contents
    std::vector<std::string> commands;
    std::vector<std::string> words;
    std::vector<std::string> txt;
    std::string scriptname;
    int position;
    int looppoint, loopcount;

    int scriptdelay;
    bool running;
    std::string tempword;
    std::string currentletter;

    //Textbox stuff
    int textx;
    int texty;
    int r,g,b;
    int txtnumlines;

    //Misc
    int i, j, k;

    //Custom level stuff
     std::vector <std::string>  customscript;
};

#endif /* SCRIPT_H */
