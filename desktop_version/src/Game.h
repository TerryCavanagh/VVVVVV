#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include "SDL.h"
#include "Maths.h"
#include "UtilityClass.h"
#include "GraphicsUtil.h"


class entityclass;
class mapclass;
class Graphics;
class musicclass;

class Game
{
public:
    Game(void);
    ~Game(void);


    void setGlobalSoundVol(const float _vol)
    {
        m_globalVol = _vol;
    }
    float getGlobalSoundVol()
    {
        return m_globalVol;
    }


    int crewrescued();

    std::string unrescued();

    void resetgameclock();

    void customsavequick(std::string savfile, mapclass& map, entityclass& obj, musicclass& music);
    void savequick(mapclass& map, entityclass& obj, musicclass& music);

    void gameclock();

    std::string giventimestring(int hrs, int min, int sec, UtilityClass& help );

    std::string  timestring(UtilityClass& help);

    std::string partimestring(UtilityClass& help);

    std::string resulttimestring(UtilityClass& help);

    std::string timetstring(int t, UtilityClass& help);

    void  createmenu(std::string t);

    void lifesequence(entityclass& obj);

    void gethardestroom(mapclass& map);

    void updatestate(Graphics& dwgfx, mapclass& map, entityclass& obj, UtilityClass& help, musicclass& music);

    void unlocknum(int t, mapclass& map, Graphics& dwgfx);

    void loadstats(mapclass& map, Graphics& dwgfx);

    void  savestats(mapclass& map, Graphics& dwgfx);

    void deletestats(mapclass& map, Graphics& dwgfx);

    void deletequick();

    void savetele(mapclass& map, entityclass& obj, musicclass& music);

    void loadtele(mapclass& map, entityclass& obj, musicclass& music);

    void deletetele();

    void customstart(entityclass& obj, musicclass& music );

    void start(entityclass& obj, musicclass& music );

    void startspecial(int t, entityclass& obj, musicclass& music);

    void starttrial(int t, entityclass& obj, musicclass& music);

    void telegotoship()
    {
        //Special function to move the telesave to the ship teleporter.
        //telecookie.data.savex = 13*8;
        //telecookie.data.savey = 129;
        //telecookie.data.saverx = 102;
        //telecookie.data.savery = 111;
        //telecookie.data.savegc = 0;
        //telecookie.data.savedir = 1;
        //telecookie.data.savepoint = 0;

        //telecookie.data.currentsong = 4;
        //telecookie.data.companion = 0;

        //telecookie.data.finalmode = false;
        //telecookie.data.finalstretch = false;
    }

    void swnpenalty();

    void deathsequence(mapclass& map, entityclass& obj, musicclass& music);

    void customloadquick(std::string savfile, mapclass& map, entityclass& obj, musicclass& music);
    void loadquick(mapclass& map, entityclass& obj, musicclass& music);

    void loadsummary(mapclass& map, UtilityClass& help);

    void initteleportermode(mapclass& map);

	std::string saveFilePath;


    int door_left;
    int door_right;
    int door_up;
    int door_down;
    int roomx, roomy, roomchangedir;
    int temp, j, k;

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
	private:
    float m_globalVol;

	public:

    int tapleft, tapright;

    //Menu interaction stuff
    bool mapheld;
    int menupage;
    //public var crewstats:Array = new Array();
    int lastsaved;
    int deathcounts;
	int timerStartTime;

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
    std::vector<std::string> menuoptions;
    std::vector<bool> menuoptionsactive;
    int nummenuoptions, currentmenuoption ;
    std::string menuselection, currentmenuname, previousmenuname;
    int menuxoff, menuyoff;

    int menucountdown;
    std::string menudest;

    int creditposx, creditposy, creditposdelay;


    //60 fps mode!
    bool sfpsmode;

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
    std::vector<int> temp_unlock;
    std::vector<int> temp_unlocknotify;
    int stat_trinkets;
    bool fullscreen;
    int bestgamedeaths;

    bool stat_screenshakes;
    bool stat_backgrounds;
    bool stat_flipmode;
    bool stat_invincibility;
    int stat_slowdown;


    std::vector<int>besttimes;
    std::vector<int>besttrinkets;
    std::vector<int>bestlives;
    std::vector<int> bestrank;

    bool telecookieexists;
    bool quickcookieexists;

    std::string tele_gametime;
    int tele_trinkets;
    std::string tele_currentarea;
    std::string quick_gametime;
    int quick_trinkets;
    std::string quick_currentarea;

    int mx, my;
    int screenshake, flashlight;
    bool test;
    std::string teststring, tempstring;
    bool advancetext, pausescript;

    int deathseq, lifeseq;

    int coins, trinkets, crewmates, trinkencollect;
    int savepoint, teleport, teleportxpos;
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

    //Screenrecording stuff, for beta/trailer
    int recording;
    std::string recordstring;
    bool combomode;
    int combolen;
    std::string comboaction;
    std::string currentaction;
    bool recordinit;

    std::vector<int> playback;
    int playbackpos;
    int playbacksize;
    int playmove;
    int playcombo;
    bool playbackfinished;

    bool menukludge;
    bool quickrestartkludge;

    bool paused;
    int globalsound;

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
};

#endif /* GAME_H */
