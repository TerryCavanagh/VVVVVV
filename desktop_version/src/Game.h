#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include "SDL.h"
#include "Maths.h"
#include "UtilityClass.h"
#include "GraphicsUtil.h"

struct MenuOption
{
    std::string text;
    bool active;
};

//Menu IDs
namespace Menu
{
    enum MenuName
    {
        mainmenu,
        playerworlds,
        levellist,
        quickloadlevel,
        youwannaquit,
        errornostart,
        graphicoptions,
        ed_settings,
        ed_desc,
        ed_music,
        ed_quit,
        options,
        accessibility,
        controller,
        cleardatamenu,
        setinvincibility,
        setslowdown,
        unlockmenu,
        credits,
        credits2,
        credits25,
        credits3,
        credits4,
        credits5,
        credits6,
        play,
        unlocktimetrial,
        unlocktimetrials,
        unlocknodeathmode,
        unlockintermission,
        unlockflipmode,
        newgamewarning,
        playmodes,
        intermissionmenu,
        playint1,
        playint2,
        continuemenu,
        startnodeathmode,
        gameover,
        gameover2,
        unlockmenutrials,
        timetrials,
        nodeathmodecomplete,
        nodeathmodecomplete2,
        timetrialcomplete,
        timetrialcomplete2,
        timetrialcomplete3,
        gamecompletecontinue,
    };
};

struct MenuStackFrame
{
    int option;
    enum Menu::MenuName name;
};


class Game
{
public:
    void init(void);
    ~Game(void);


    int crewrescued();

    std::string unrescued();

    void resetgameclock();

    void customsavequick(std::string savfile);
    void savequick();

    void gameclock();

    std::string giventimestring(int hrs, int min, int sec);

    std::string  timestring();

    std::string partimestring();

    std::string resulttimestring();

    std::string timetstring(int t);

    void returnmenu();
    void returntomenu(enum Menu::MenuName t);
    void  createmenu(enum Menu::MenuName t, bool samemenu = false);

    void lifesequence();

    void gethardestroom();

    void updatestate();

    void unlocknum(int t);

    void loadstats();

    void  savestats();

    void deletestats();

    void deletequick();

    void savetele();

    void loadtele();

    void deletetele();

    void customstart();

    void start();

    void startspecial(int t);

    void starttrial(int t);

    void swnpenalty();

    void deathsequence();

    void customloadquick(std::string savfile);
    void loadquick();

    void loadsummary();

    void initteleportermode();

    std::string saveFilePath;


    int door_left;
    int door_right;
    int door_up;
    int door_down;
    int roomx, roomy, roomchangedir;
    int prevroomx, prevroomy;
    int j, k;

    int savex, savey, saverx, savery;
    int savegc, savedir;

    //Added for port
    int edsavex, edsavey, edsaverx, edsavery;
    int edsavegc, edsavedir;

    //State logic stuff
    int state, statedelay;

    bool glitchrunkludge;

    int usingmmmmmm;

    int gamestate;
    bool hascontrol, jumpheld;
    int jumppressed;
    int gravitycontrol;

    bool infocus;
    bool muted;
    int mutebutton;
    bool musicmuted;
    int musicmutebutton;

    int tapleft, tapright;

    //Menu interaction stuff
    bool mapheld;
    int menupage;
    //public var crewstats:Array = new Array();
    int lastsaved;
    int deathcounts;

    int frames, seconds, minutes, hours;
    bool gamesaved;
    std::string savetime;
    std::string savearea;
    int savetrinkets;
    bool startscript;
    std::string newscript;

    int mainmenu;
    bool menustart;

    //Teleporting
    bool teleport_to_new_area;
    int teleport_to_x, teleport_to_y;
    std::string teleportscript;
    bool useteleporter;
    int teleport_to_teleporter;

    //Main Menu Variables
    std::vector<MenuOption> menuoptions;
    int currentmenuoption ;
    enum Menu::MenuName currentmenuname;
    int current_credits_list_index;
    int menuxoff, menuyoff;
    std::vector<MenuStackFrame> menustack;

    void inline option(std::string text, bool active = true)
    {
        MenuOption menuoption;
        menuoption.text = text;
        menuoption.active = active;
        menuoptions.push_back(menuoption);
    }

    int menucountdown;
    enum Menu::MenuName menudest;

    int creditposx, creditposy, creditposdelay;


    //Sine Wave Ninja Minigame
    bool swnmode;
    int swngame, swnstate, swnstate2, swnstate3, swnstate4, swndelay, swndeaths;
    int swntimer, swncolstate, swncoldelay;
    int  swnrecord, swnbestrank, swnrank, swnmessage;

    //SuperCrewMate Stuff
    bool supercrewmate, scmhurt, scmmoveme;
    int scmprogress;

    //Accessibility Options
    bool  colourblindmode;
    bool noflashingmode;
    int slowdown;
    Uint32 gameframerate;

    bool nodeathmode;
    int gameoverdelay;
    bool nocutscenes;

    //Time Trials
    bool intimetrial, timetrialparlost;
    int timetrialcountdown, timetrialshinytarget, timetriallevel;
    int timetrialpar, timetrialresulttime, timetrialrank;

    int creditposition;
    int creditmaxposition;
    std::vector<const char*> superpatrons;
    std::vector<const char*> patrons;
    std::vector<const char*> githubfriends;
    bool insecretlab;

    bool inintermission;

    std::vector<bool> crewstats;

    bool alarmon;
    int alarmdelay;
    bool blackout;

    std::vector<bool> tele_crewstats;

    std::vector<bool> quick_crewstats;

    std::vector<int> unlock;
    std::vector<int> unlocknotify;
    bool anything_unlocked();
    int stat_trinkets;
    bool fullscreen;
    int bestgamedeaths;


    std::vector<int>besttimes;
    std::vector<int>besttrinkets;
    std::vector<int>bestlives;
    std::vector<int> bestrank;

    std::string tele_gametime;
    int tele_trinkets;
    std::string tele_currentarea;
    std::string quick_gametime;
    int quick_trinkets;
    std::string quick_currentarea;

    int mx, my;
    int screenshake, flashlight;
    bool advancetext, pausescript;

    int deathseq, lifeseq;

    int trinkets();
    int crewmates();
    int savepoint, teleportxpos;
    bool teleport;
    int edteleportent;
    bool completestop;

    float inertia;

    int companion;
    bool roomchange;
    SDL_Rect teleblock;
    bool activetele;
    int readytotele;
    int activity_r, activity_g, activity_b;
    std::string activity_lastprompt;

    std::string telesummary, quicksummary, customquicksummary;
    bool save_exists();

    bool backgroundtext;

    int activeactivity, act_fade;

    bool press_left, press_right, press_action, press_map;

    //Some stats:
    int totalflips;
    std::string hardestroom;
    int hardestroomdeaths, currentroomdeaths;

    bool savemystats;


    bool advanced_mode;
    bool fullScreenEffect_badSignal;
    bool useLinearFilter;
    int stretchMode;
    int controllerSensitivity;

    bool quickrestartkludge;

    //Custom stuff
    std::string customscript[50];
    int customcol;
    int levelpage;
    int playcustomlevel;
    std::string customleveltitle;
    std::string customlevelfilename;

    void clearcustomlevelstats();
    void loadcustomlevelstats();
    void savecustomlevelstats();
    void updatecustomlevelstats(std::string clevel, int cscore);

    std::string customlevelstats[200]; //string array containing level filenames
    int customlevelscore[200];//0 - not played, 1 - finished, 2 - all trinkets, 3 - finished, all trinkets
    int numcustomlevelstats;
    bool customlevelstatsloaded;


    std::vector<SDL_GameControllerButton> controllerButton_map;
    std::vector<SDL_GameControllerButton> controllerButton_flip;
    std::vector<SDL_GameControllerButton> controllerButton_esc;

    bool skipfakeload;
    bool ghostsenabled;

    bool cliplaytest;
    int playx;
    int playy;
    int playrx;
    int playry;
    int playgc;

    void quittomenu();
    void returntolab();
    bool fadetomenu;
    int fadetomenudelay;
    bool fadetolab;
    int fadetolabdelay;

#if !defined(NO_CUSTOM_LEVELS)
    void returntoeditor();
    bool shouldreturntoeditor;
#endif

    int gametimer;

    bool inline inspecial()
    {
        return inintermission || insecretlab || intimetrial || nodeathmode;
    }
};

extern Game game;

#endif /* GAME_H */
