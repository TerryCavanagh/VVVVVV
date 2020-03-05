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
        commands[scriptlength] = t;
        scriptlength++;
    }

    void clearcustom();

    void tokenize(std::string t);

    void run();

    void resetgametomenu();

    void startgamemode(int t);

    void teleport();

    void hardreset();

    //Script contents
    std::vector<std::string> commands;
    std::vector<std::string> words;
    std::vector<std::string> txt;
    std::string scriptname;
    int position, scriptlength;
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
