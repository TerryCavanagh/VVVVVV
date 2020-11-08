#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>
#include <vector>

#include <SDL.h>

#define filllines(lines) commands.insert(commands.end(), lines, lines + SDL_arraysize(lines))


struct Script
{
    std::string name;
    std::vector<std::string> contents;
};

class scriptclass
{
public:


    scriptclass(void);

    void load(const std::string& name);
    void loadother(const char* t);
    void loadcustom(const std::string& t);

    void inline add(const std::string& t)
    {
        commands.push_back(t);
    }

    void clearcustom(void);

    void tokenize(const std::string& t);

    void run(void);

    void resetgametomenu(void);

    void startgamemode(int t);

    void teleport(void);

    void hardreset(void);

    //Script contents
    std::vector<std::string> commands;
    std::string words[40];
    std::vector<std::string> txt;
    std::string scriptname;
    int position;
    int looppoint, loopcount;

    int scriptdelay;
    bool running;

    //Textbox stuff
    int textx;
    int texty;
    int r,g,b;

    //Misc
    int i, j, k;

    //Custom level stuff
    std::vector<Script> customscripts;
};

#ifndef SCRIPT_DEFINITION
extern scriptclass script;
#endif

#endif /* SCRIPT_H */
