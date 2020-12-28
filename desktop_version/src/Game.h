#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <string>
#include <vector>

#include "ScreenSettings.h"

// Forward decl without including all of <tinyxml2.h>
namespace tinyxml2
{
    class XMLDocument;
    class XMLElement;
}

struct MenuOption
{
    char text[161]; // 40 chars (160 bytes) covers the entire screen, + 1 more for null terminator
    // WARNING: should match Game::menutextbytes below
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
        errorsavingsettings,
        graphicoptions,
        ed_settings,
        ed_desc,
        ed_music,
        ed_quit,
        options,
        advancedoptions,
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

struct CustomLevelStat
{
    std::string name;
    int score; //0 - not played, 1 - finished, 2 - all trinkets, 3 - finished, all trinkets
};


class Game
{
public:
    void init(void);


    int crewrescued();

    std::string unrescued();

    void resetgameclock();

    bool customsavequick(std::string savfile);
    bool savequick();

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

    void loadstats(ScreenSettings* screen_settings);

    bool savestats(const ScreenSettings* screen_settings);
    bool savestats();

    void deletestats();

    void deserializesettings(tinyxml2::XMLElement* dataNode, ScreenSettings* screen_settings);

    void serializesettings(tinyxml2::XMLElement* dataNode, const ScreenSettings* screen_settings);

    void loadsettings(ScreenSettings* screen_settings);

    bool savesettings(const ScreenSettings* screen_settings);
    bool savesettings();

    bool savestatsandsettings();

    void savestatsandsettings_menu();

    void deletesettings();

    void deletequick();

    bool savetele();

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

    void readmaingamesave(tinyxml2::XMLDocument& doc);
    std::string writemaingamesave(tinyxml2::XMLDocument& doc);

    void initteleportermode();

    std::string saveFilePath;


    int door_left;
    int door_right;
    int door_up;
    int door_down;
    int roomx, roomy, roomchangedir;
    int prevroomx, prevroomy;

    int savex, savey, saverx, savery;
    int savegc, savedir;

    //Added for port
    int edsavex, edsavey, edsaverx, edsavery;
    int edsavegc, edsavedir;

    //State logic stuff
    int state, statedelay;

    bool glitchrunkludge;

    int gamestate;
    bool hascontrol, jumpheld;
    int jumppressed;
    int gravitycontrol;

    bool muted;
    int mutebutton;
    bool musicmuted;
    int musicmutebutton;

    int tapleft, tapright;

    //Menu interaction stuff
    void mapmenuchange(const int newgamestate);
    bool mapheld;
    int menupage;
    int lastsaved;
    int deathcounts;

    int frames, seconds, minutes, hours;
    bool gamesaved;
    bool gamesavefailed;
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
    enum Menu::MenuName kludge_ingametemp;
    int current_credits_list_index;
    int menuxoff, menuyoff;
    int menuspacing;
    static const int menutextbytes = 161; // this is just sizeof(MenuOption::text), but doing that is non-standard
    std::vector<MenuStackFrame> menustack;

    void inline option(const char* text, bool active = true)
    {
        MenuOption menuoption;
        SDL_strlcpy(menuoption.text, text, sizeof(menuoption.text));
        menuoption.active = active;
        menuoptions.push_back(menuoption);
    }

    int menucountdown;
    enum Menu::MenuName menudest;

    int creditposx, creditposy, creditposdelay;
    int oldcreditposx;

    bool silence_settings_error;


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

    bool nodeathmode;
    int gameoverdelay;
    bool nocutscenes;

    //Time Trials
    bool intimetrial, timetrialparlost;
    int timetrialcountdown, timetrialshinytarget, timetriallevel;
    int timetrialpar, timetrialresulttime, timetrialresultframes, timetrialrank;

    int creditposition;
    int oldcreditposition;
    bool insecretlab;

    bool inintermission;

    static const int numcrew = 6;
    bool crewstats[numcrew];

    bool alarmon;
    int alarmdelay;
    bool blackout;

    bool tele_crewstats[numcrew];

    bool quick_crewstats[numcrew];

    static const int numunlock = 25;
    bool unlock[numunlock];
    bool unlocknotify[numunlock];
    bool anything_unlocked();
    int stat_trinkets;
    int bestgamedeaths;


    static const int numtrials = 6;
    int besttimes[numtrials];
    int bestframes[numtrials];
    int besttrinkets[numtrials];
    int bestlives[numtrials];
    int bestrank[numtrials];

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
    int oldreadytotele;
    int activity_r, activity_g, activity_b;
    std::string activity_lastprompt;

    std::string telesummary, quicksummary, customquicksummary;
    bool save_exists();

    bool backgroundtext;

    int activeactivity, act_fade;
    int prev_act_fade;

    bool press_left, press_right, press_action, press_map;

    //Some stats:
    int totalflips;
    std::string hardestroom;
    int hardestroomdeaths, currentroomdeaths;

    bool savemystats;


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

    std::vector<CustomLevelStat> customlevelstats;
    bool customlevelstatsloaded;


    std::vector<SDL_GameControllerButton> controllerButton_map;
    std::vector<SDL_GameControllerButton> controllerButton_flip;
    std::vector<SDL_GameControllerButton> controllerButton_esc;
    std::vector<SDL_GameControllerButton> controllerButton_restart;

    bool skipfakeload;
    bool ghostsenabled;

    bool cliplaytest;
    int playx;
    int playy;
    int playrx;
    int playry;
    int playgc;
    int playmusic;
    std::string playassets;

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

    bool over30mode;
    bool glitchrunnermode; // Have fun speedrunners! <3 Misa

    bool ingame_titlemode;

    void returntopausemenu();
    void unlockAchievement(const char *name);

    bool disablepause;
};

#ifndef GAME_DEFINITION
extern Game game;
#endif

#endif /* GAME_H */
