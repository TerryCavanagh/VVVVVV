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

#define NUM_SCRIPT_ARGS 40

enum StartMode
{
    Start_MAINGAME,
    Start_MAINGAME_TELESAVE,
    Start_MAINGAME_QUICKSAVE,
    Start_TIMETRIAL_SPACESTATION1,
    Start_TIMETRIAL_LABORATORY,
    Start_TIMETRIAL_TOWER,
    Start_TIMETRIAL_SPACESTATION2,
    Start_TIMETRIAL_WARPZONE,
    Start_TIMETRIAL_FINALLEVEL,
    Start_NODEATHMODE_WITHCUTSCENES,
    Start_NODEATHMODE_NOCUTSCENES,
    Start_SECRETLAB,
    Start_INTERMISSION1_VITELLARY,
    Start_INTERMISSION1_VERMILION,
    Start_INTERMISSION1_VERDIGRIS,
    Start_INTERMISSION1_VICTORIA,
    Start_INTERMISSION2_VITELLARY,
    Start_INTERMISSION2_VERMILION,
    Start_INTERMISSION2_VERDIGRIS,
    Start_INTERMISSION2_VICTORIA,
    Start_EDITOR,
    Start_EDITORPLAYTESTING,
    Start_CUSTOM,
    Start_CUSTOM_QUICKSAVE,
    Start_QUIT,
    Start_CUTSCENETEST,

    Start_FIRST_NODEATHMODE = Start_NODEATHMODE_WITHCUTSCENES,
    Start_LAST_NODEATHMODE = Start_NODEATHMODE_NOCUTSCENES,
    Start_FIRST_INTERMISSION1 = Start_INTERMISSION1_VITELLARY,
    Start_LAST_INTERMISSION1 = Start_INTERMISSION1_VICTORIA,
    Start_FIRST_INTERMISSION2 = Start_INTERMISSION2_VITELLARY,
    Start_LAST_INTERMISSION2 = Start_INTERMISSION2_VICTORIA,

    Start_FIRST_TIMETRIAL = Start_TIMETRIAL_SPACESTATION1
};

class scriptclass
{
public:


    scriptclass(void);

    bool load(const std::string& name);
    void loadother(const char* t);
    bool loadcustom(const std::string& t);
    void loadalts(const std::string& processed, const std::string& raw);

    void add_test_line(const std::string& speaker, const std::string& english, char textcase, bool textbuttons);
    void loadtest(const std::string& name);

    void inline add(const std::string& t)
    {
        commands.push_back(t);
    }

    void clearcustom(void);

    void tokenize(const std::string& t);

    void run(void);

    void translate_dialogue(void);

    void startgamemode(enum StartMode mode);

    void teleport(void);

    void hardreset(void);

    //Script contents
    std::vector<std::string> commands;
    std::string words[NUM_SCRIPT_ARGS];
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
    bool textflipme;
    bool textcentertext;
    size_t textpad_left;
    size_t textpad_right;
    size_t textpadtowidth;
    char textcase;
    bool textbuttons;
    bool textlarge;

    //Misc
    int i, j, k;

    //Custom level stuff
    std::vector<Script> customscripts;
};

#ifndef SCRIPT_DEFINITION
extern scriptclass script;
#endif

#endif /* SCRIPT_H */
