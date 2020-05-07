#include "Enums.h"

#include "Game.h"
#include "Graphics.h"
#include "Entity.h"
#include "Map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sstream>

#include "FileSystemUtils.h"

#include "tinyxml.h"

#include "Network.h"

#include "MakeAndPlay.h"

// lol, Win32 -flibit
#ifdef _WIN32
#define strcasecmp stricmp
#endif

//TODO: Non Urgent code cleanup
const char* BoolToString(bool _b)
{
    if(_b)
    {
        return "1";
    }
    else
    {
        return "0";
    }
}

bool GetButtonFromString(const char *pText, SDL_GameControllerButton *button)
{
    if (*pText == '0' ||
        *pText == 'a' ||
        *pText == 'A')
    {
        *button = SDL_CONTROLLER_BUTTON_A;
        return true;
    }
    if (strcmp(pText, "1") == 0 ||
        *pText == 'b' ||
        *pText == 'B')
    {
        *button = SDL_CONTROLLER_BUTTON_B;
        return true;
    }
    if (*pText == '2' ||
        *pText == 'x' ||
        *pText == 'X')
    {
        *button = SDL_CONTROLLER_BUTTON_X;
        return true;
    }
    if (*pText == '3' ||
        *pText == 'y' ||
        *pText == 'Y')
    {
        *button = SDL_CONTROLLER_BUTTON_Y;
        return true;
    }
    if (*pText == '4' ||
        strcasecmp(pText, "BACK") == 0)
    {
        *button = SDL_CONTROLLER_BUTTON_BACK;
        return true;
    }
    if (*pText == '5' ||
        strcasecmp(pText, "GUIDE") == 0)
    {
        *button = SDL_CONTROLLER_BUTTON_GUIDE;
        return true;
    }
    if (*pText == '6' ||
        strcasecmp(pText, "START") == 0)
    {
        *button = SDL_CONTROLLER_BUTTON_START;
        return true;
    }
    if (*pText == '7' ||
        strcasecmp(pText, "LS") == 0)
    {
        *button = SDL_CONTROLLER_BUTTON_LEFTSTICK;
        return true;
    }
    if (*pText == '8' ||
        strcasecmp(pText, "RS") == 0)
    {
        *button = SDL_CONTROLLER_BUTTON_RIGHTSTICK;
        return true;
    }
    if (*pText == '9' ||
        strcasecmp(pText, "LB") == 0)
    {
        *button = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
        return true;
    }
    if (strcmp(pText, "10") == 0 ||
        strcasecmp(pText, "RB") == 0)
    {
        *button = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
        return true;
    }
    return false;
}


void Game::init(void)
{
    mutebutton = 0;
    infocus = true;
    muted = false;
    musicmuted = false;
    musicmutebutton = 0;

    glitchrunkludge = false;
    hascontrol = true;
    jumpheld = false;
    advancetext = false;
    jumppressed = 0;
    gravitycontrol = 0;
    teleport = false;
    edteleportent = 0; //Added in the port!
    companion = 0;
    roomchange = false;


    teleportscript = "";
    savemystats = false;
    quickrestartkludge = false;

    tapleft = 0;
    tapright = 0;

    press_right = 0;
    press_left = 0;


    advancetext = false;
    pausescript = false;
    completestop = false;
    activeactivity = -1;
    act_fade = 0;
    backgroundtext = false;
    startscript = false;
    newscript = "";
    inintermission = false;

    alarmon = false;
    alarmdelay = 0;
    blackout = false;
    creditposx = 0;
    creditposy = 0;
    creditposdelay = 0;

    useteleporter = false;
    teleport_to_teleporter = 0;

    activetele = false;
    readytotele = 0;
    activity_lastprompt = "";
    activity_r = 0;
    activity_g = 0;
    activity_b = 0;
    creditposition = 0;
    bestgamedeaths = -1;

    fullScreenEffect_badSignal = false;

    //Accessibility Options
    colourblindmode = false;
    noflashingmode = false;
    slowdown = 30;
    gameframerate=34;

    fullscreen = false;// true; //Assumed true at first unless overwritten at some point!
    stretchMode = 0;
    useLinearFilter = false;
    advanced_mode = false;
    fullScreenEffect_badSignal = false;
    // 0..5
    controllerSensitivity = 2;

    nodeathmode = false;
    nocutscenes = false;

    for(int i=0; i<50; i++)
    {
        customscript[i]="";
    }
    customcol=0;

    crewstats.resize(6);
    tele_crewstats.resize(6);
    quick_crewstats.resize(6);
    besttimes.resize(6, -1);
    besttrinkets.resize(6, -1);
    bestlives.resize(6, -1);
    bestrank.resize(6, -1);

    crewstats[0] = true;
    lastsaved = 0;

    tele_gametime = "00:00";
    tele_trinkets = 0;
    tele_currentarea = "Error! Error!";
    quick_gametime = "00:00";
    quick_trinkets = 0;
    quick_currentarea = "Error! Error!";

    //Menu stuff initiliased here:
    unlock.resize(25);
    unlocknotify.resize(25);

    currentmenuoption = 0;
    current_credits_list_index = 0;
    menuxoff = 0;
    menuyoff = 0;
    menucountdown = 0;
    levelpage=0;
    playcustomlevel=0;
    customleveltitle="";
    createmenu(Menu::mainmenu);

    deathcounts = 0;
    gameoverdelay = 0;
    frames = 0;
    seconds = 0;
    minutes = 0;
    hours = 0;
    gamesaved = false;
    savetime = "00:00";
    savearea = "nowhere";
    savetrinkets = 0;

    intimetrial = false;
    timetrialcountdown = 0;
    timetrialshinytarget = 0;
    timetrialparlost = false;
    timetrialpar = 0;
    timetrialresulttime = 0;

    totalflips = 0;
    hardestroom = "Welcome Aboard";
    hardestroomdeaths = 0;
    currentroomdeaths=0;

    inertia = 1.1f;
    swnmode = false;
    swntimer = 0;
    swngame = 0;//Not playing sine wave ninja!
    swnstate = 0;
    swnstate2 = 0;
    swnstate3 = 0;
    swnstate4 = 0;
    swndelay = 0;
    swndeaths = 0;
    supercrewmate = false;
    scmhurt = false;
    scmprogress = 0;
    scmmoveme = false;
    swncolstate = 0;
    swncoldelay = 0;
    swnrecord = 0;
    swnbestrank = 0;
    swnrank = 0;
    swnmessage = 0;

    clearcustomlevelstats();

    saveFilePath = FILESYSTEM_getUserSaveDirectory();

    TiXmlDocument doc;
    if (!FILESYSTEM_loadTiXmlDocument("saves/qsave.vvv", &doc))
    {
        quicksummary = "";
        printf("Quick Save Not Found\n");
    }
    else
    {
        TiXmlHandle hDoc(&doc);
        TiXmlElement* pElem;
        TiXmlHandle hRoot(0);

        pElem=hDoc.FirstChildElement().Element();
        if (!pElem)
        {
            printf("Quick Save Appears Corrupted: No XML Root\n");
        }

        // save this for later
        hRoot=TiXmlHandle(pElem);

        for( pElem = hRoot.FirstChild( "Data" ).FirstChild().Element(); pElem; pElem=pElem->NextSiblingElement())
        {
            std::string pKey(pElem->Value());
            const char* pText = pElem->GetText() ;

            if (pKey == "summary")
            {
                quicksummary = pText;
            }


        }
    }


    TiXmlDocument docTele;
    if (!FILESYSTEM_loadTiXmlDocument("saves/tsave.vvv", &docTele))
    {
        telesummary = "";
        printf("Teleporter Save Not Found\n");
    }
    else
    {
        TiXmlHandle hDoc(&docTele);
        TiXmlElement* pElem;
        TiXmlHandle hRoot(0);


        {
            pElem=hDoc.FirstChildElement().Element();
            // should always have a valid root but handle gracefully if it does
            if (!pElem)
            {
                printf("Teleporter Save Appears Corrupted: No XML Root\n");
            }

            // save this for later
            hRoot=TiXmlHandle(pElem);
        }

        for( pElem = hRoot.FirstChild( "Data" ).FirstChild().Element(); pElem; pElem=pElem->NextSiblingElement())
        {
            std::string pKey(pElem->Value());
            const char* pText = pElem->GetText() ;

            if (pKey == "summary")
            {
                telesummary = pText;
            }


        }
    }

    screenshake = flashlight = 0 ;

    stat_trinkets = 0;

    state = 1;
    statedelay = 0;
    //updatestate();

    skipfakeload = false;

    cliplaytest = false;
    playx = 0;
    playy = 0;
    playrx = 0;
    playry = 0;
    playgc = 0;

    fadetomenu = false;
    fadetomenudelay = 0;
    fadetolab = false;
    fadetolabdelay = 0;

    /* Terry's Patrons... */
    superpatrons.push_back("Anders Ekermo");
    superpatrons.push_back("Andreas K|mper");
    superpatrons.push_back("Anthony Burch");
    superpatrons.push_back("Bennett Foddy");
    superpatrons.push_back("Brendan O'Sullivan");
    superpatrons.push_back("Christopher Armstrong");
    superpatrons.push_back("Daniel Benmergui");
    superpatrons.push_back("David Pittman");
    superpatrons.push_back("Ian Bogost");
    superpatrons.push_back("Ian Poma");
    superpatrons.push_back("Jaz McDougall");
    superpatrons.push_back("John Faulkenbury");
    superpatrons.push_back("Jonathan Whiting");
    superpatrons.push_back("Kyle Pulver");
    superpatrons.push_back("Markus Persson");
    superpatrons.push_back("Nathan Ostgard");
    superpatrons.push_back("Nick Easler");
    superpatrons.push_back("Stephen Lavelle");
    patrons.push_back("Adam Wendt");
    patrons.push_back("Andreas J{rgensen");
    patrons.push_back("}ngel Louzao Penalva");
    patrons.push_back("Ashley Burton");
    patrons.push_back("Aubrey Hesselgren");
    patrons.push_back("Bradley Rose");
    patrons.push_back("Brendan Urquhart");
    patrons.push_back("Chris Ayotte");
    patrons.push_back("Christopher Zamanillo");
    patrons.push_back("Daniel Schuller");
    patrons.push_back("Hybrid Mind Studios");
    patrons.push_back("Emilie McGinley");
    patrons.push_back("Francisco Solares");
    patrons.push_back("Hal Helms");
    patrons.push_back("Hayden Scott-Baron");
    patrons.push_back("Hermit Games");
    patrons.push_back("Ido Yehieli");
    patrons.push_back("Jade Vault Games");
    patrons.push_back("James Andrews");
    patrons.push_back("James Riley");
    patrons.push_back("James Hsieh");
    patrons.push_back("Jasper Byrne");
    patrons.push_back("Jedediah Baker");
    patrons.push_back("Jens Bergensten");
    patrons.push_back("Jeremy J. Penner");
    patrons.push_back("Jeremy Peterson");
    patrons.push_back("Jim McGinley");
    patrons.push_back("Jonathan Cartwright");
    patrons.push_back("John Nesky");
    patrons.push_back("Jos Yule");
    patrons.push_back("Jose Flores");
    patrons.push_back("Josh Bizeau");
    patrons.push_back("Joshua Buergel");
    patrons.push_back("Joshua Hochner");
    patrons.push_back("Kurt Ostfeld");
    patrons.push_back("Magnus P~lsson");
    patrons.push_back("Mark Neschadimenko");
    patrons.push_back("Matt Antonellis");
    patrons.push_back("Matthew Reppert");
    patrons.push_back("Michael Falkensteiner");
    patrons.push_back("Michael Vendittelli");
    patrons.push_back("Mike Kasprzak");
    patrons.push_back("Mitchel Stein");
    patrons.push_back("Sean Murray");
    patrons.push_back("Simon Michael");
    patrons.push_back("Simon Schmid");
    patrons.push_back("Stephen Maxwell");
    patrons.push_back("Swing Swing Submarine");
    patrons.push_back("Tam Toucan");
    patrons.push_back("Terry Dooher");
    patrons.push_back("Tim W.");
    patrons.push_back("Timothy Bragan");

    /* CONTRIBUTORS.txt, again listed alphabetically (according to `sort`) by last name */
    githubfriends.push_back("Matt \"Stelpjo\" Aaldenberg");
    githubfriends.push_back("AlexApps99");
    githubfriends.push_back("Christoph B{hmwalder");
    githubfriends.push_back("Charlie Bruce");
    githubfriends.push_back("Brian Callahan");
    githubfriends.push_back("Dav999");
    githubfriends.push_back("Allison Fleischer");
    githubfriends.push_back("Daniel Lee");
    githubfriends.push_back("Fredrik Ljungdahl");
    githubfriends.push_back("Matt Penny");
    githubfriends.push_back("Elliott Saltar");
    githubfriends.push_back("Marvin Scholz");
    githubfriends.push_back("Keith Stellyes");
    githubfriends.push_back("Elijah Stone");
    githubfriends.push_back("Thomas S|nger");
    githubfriends.push_back("Info Teddy");
    githubfriends.push_back("Alexandra Tildea");
    githubfriends.push_back("leo60228");
    githubfriends.push_back("Emmanuel Vadot");
    githubfriends.push_back("Remi Verschelde"); // TODO: Change to "Rémi" if/when UTF-8 support is added
    githubfriends.push_back("viri");
    githubfriends.push_back("Wouter");

    /* Calculate credits length, finally. */
    creditmaxposition = 1050 + (10 * (
        superpatrons.size() + patrons.size() + githubfriends.size()
    ));
}

Game::~Game(void)
{
}

void Game::lifesequence()
{
    if (lifeseq > 0)
    {
        int i = obj.getplayer();
        obj.entities[i].invis = false;
        if (lifeseq == 2) obj.entities[i].invis = true;
        if (lifeseq == 6) obj.entities[i].invis = true;
        if (lifeseq >= 8) obj.entities[i].invis = true;
        if (lifeseq > 5) gravitycontrol = savegc;

        lifeseq--;
        if (lifeseq <= 0)
        {
            obj.entities[i].invis = false;
        }
    }
}

void Game::clearcustomlevelstats()
{
    //just clearing the arrays
    for(int i=0; i<200; i++)
    {
        customlevelstats[i]="";
        customlevelscore[i]=0;
    }
    numcustomlevelstats=0;

    customlevelstatsloaded=false; //To ensure we don't load it where it isn't needed
}


void Game::updatecustomlevelstats(std::string clevel, int cscore)
{
    if (clevel.find("levels/") != std::string::npos)
    {
        clevel = clevel.substr(7);
    }
    int tvar=-1;
    for(int j=0; j<numcustomlevelstats; j++)
    {
        if(clevel==customlevelstats[j])
        {
            tvar=j;
            j=numcustomlevelstats+1;
        }
    }
    if(tvar>=0 && cscore > customlevelscore[tvar])
    {
        //update existing entry
        customlevelscore[tvar]=cscore;
    }
    else
    {
        //add a new entry
        if(numcustomlevelstats<200)
        {
            customlevelstats[numcustomlevelstats]=clevel;
            customlevelscore[numcustomlevelstats]=cscore;
            numcustomlevelstats++;
        }
    }
    savecustomlevelstats();
}

void Game::loadcustomlevelstats()
{
    //testing
    if(!customlevelstatsloaded)
    {
        TiXmlDocument doc;
        if (!FILESYSTEM_loadTiXmlDocument("saves/levelstats.vvv", &doc))
        {
            //No levelstats file exists; start new
            numcustomlevelstats=0;
            savecustomlevelstats();
        }
        else
        {
            TiXmlHandle hDoc(&doc);
            TiXmlElement* pElem;
            TiXmlHandle hRoot(0);

            {
                pElem=hDoc.FirstChildElement().Element();
                // should always have a valid root but handle gracefully if it does
                if (!pElem)
                {
                    printf("Error: Levelstats file corrupted\n");
                }

                // save this for later
                hRoot=TiXmlHandle(pElem);
            }


            for( pElem = hRoot.FirstChild( "Data" ).FirstChild().Element(); pElem; pElem=pElem->NextSiblingElement())
            {
                std::string pKey(pElem->Value());
                const char* pText = pElem->GetText() ;
                if(pText == NULL)
                {
                    pText = "";
                }

                if (pKey == "numcustomlevelstats")
                {
                    numcustomlevelstats = atoi(pText);
                    if(numcustomlevelstats>=200) numcustomlevelstats=199;
                }

                if (pKey == "customlevelscore")
                {
                    std::string TextString = (pText);
                    if(TextString.length())
                    {
                        std::vector<std::string> values = split(TextString,',');
                        for(size_t i = 0; i < values.size(); i++)
                        {
                            if(i<200) customlevelscore[i]=(atoi(values[i].c_str()));
                        }
                    }
                }

                if (pKey == "customlevelstats")
                {
                    std::string TextString = (pText);
                    if(TextString.length())
                    {
                        std::vector<std::string> values = split(TextString,'|');
                        for(size_t i = 0; i < values.size(); i++)
                        {
                            if(i<200) customlevelstats[i]=values[i];
                        }
                    }
                }
            }
        }
    }
}

void Game::savecustomlevelstats()
{
    TiXmlDocument doc;
    TiXmlElement* msg;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );

    TiXmlElement * root = new TiXmlElement( "Levelstats" );
    doc.LinkEndChild( root );

    TiXmlComment * comment = new TiXmlComment();
    comment->SetValue(" Levelstats Save file " );
    root->LinkEndChild( comment );

    TiXmlElement * msgs = new TiXmlElement( "Data" );
    root->LinkEndChild( msgs );

    if(numcustomlevelstats>=200)numcustomlevelstats=199;
    msg = new TiXmlElement( "numcustomlevelstats" );
    msg->LinkEndChild( new TiXmlText( help.String(numcustomlevelstats).c_str() ));
    msgs->LinkEndChild( msg );

    std::string customlevelscorestr;
    for(int i = 0; i < numcustomlevelstats; i++ )
    {
        customlevelscorestr += help.String(customlevelscore[i]) + ",";
    }
    msg = new TiXmlElement( "customlevelscore" );
    msg->LinkEndChild( new TiXmlText( customlevelscorestr.c_str() ));
    msgs->LinkEndChild( msg );

    std::string customlevelstatsstr;
    for(int i = 0; i < numcustomlevelstats; i++ )
    {
        customlevelstatsstr += customlevelstats[i] + "|";
    }
    msg = new TiXmlElement( "customlevelstats" );
    msg->LinkEndChild( new TiXmlText( customlevelstatsstr.c_str() ));
    msgs->LinkEndChild( msg );

    if(FILESYSTEM_saveTiXmlDocument("saves/levelstats.vvv", &doc))
    {
        printf("Level stats saved\n");
    }
    else
    {
        printf("Could Not Save level stats!\n");
        printf("Failed: %s%s\n", saveFilePath.c_str(), "levelstats.vvv");
    }
}

void Game::updatestate()
{
    int i;
    statedelay--;
    if(statedelay<=0){
        statedelay=0;
        glitchrunkludge=false;
    }
    if (statedelay <= 0)
    {
        switch(state)
        {
        case 0:
            //Do nothing here! Standard game state
            break;
        case 1:
            //Game initilisation
            state = 0;
            break;
        case 2:
            //Opening cutscene
            advancetext = true;
            hascontrol = false;
            state = 3;
            graphics.createtextbox("To do: write quick", 50, 80, 164, 164, 255);
            graphics.addline("intro to story!");
            //Oh no! what happen to rest of crew etc crash into dimension
            break;
        case 4:
            //End of opening cutscene for now
            graphics.createtextbox("  Press arrow keys or WASD to move  ", -1, 195, 174, 174, 174);
            graphics.textboxtimer(60);
            state = 0;
            break;
        case 5:
            //Demo over
            advancetext = true;
            hascontrol = false;
            /*graphics.createtextbox("   Prototype Complete    ", 50, 80, 164, 164, 255);
            graphics.addline("Congrats! More Info Soon!");
            graphics.textboxcenter();
            */

            startscript = true;
            newscript="returntohub";
            obj.removetrigger(5);
            state = 6;
            break;
        case 7:
            //End of opening cutscene for now
            graphics.textboxremove();
            hascontrol = true;
            advancetext = false;
            state = 0;
            break;
        case 8:
            //Enter dialogue
            obj.removetrigger(8);
            if (!obj.flags[13])
            {
                obj.flags[13] = true;
                graphics.createtextbox("  Press ENTER to view map  ", -1, 155, 174, 174, 174);
                graphics.addline("      and quicksave");
                graphics.textboxtimer(60);
            }
            state = 0;
            break;

        case 9:
            //Start SWN Minigame Mode B
            obj.removetrigger(9);

            swnmode = true;
            swngame = 6;
            swndelay = 150;
            swntimer = 60 * 30;

            //set the checkpoint in the middle of the screen
            savepoint = 0;
            savex = 148;
            savey = 100;
            savegc = 0;
            saverx = roomx;
            savery = roomy;
            savedir = 0;

            state = 0;
            break;

        case 10:
            //Start SWN Minigame Mode A
            obj.removetrigger(10);

            swnmode = true;
            swngame = 4;
            swndelay = 150;
            swntimer = 60 * 30;

            //set the checkpoint in the middle of the screen
            savepoint = 0;
            savex = 148;
            savey = 100;
            savegc = 0;
            saverx = roomx;
            savery = roomy;
            savedir = 0;

            state = 0;
            break;

        case 11:
            //Intermission 1 instructional textbox, depends on last saved
            graphics.textboxremovefast();
            graphics.createtextbox("   When you're NOT standing on   ", -1, 3, 174, 174, 174);
            if (graphics.flipmode)
            {
                if (lastsaved == 2)
                {
                    graphics.addline("   the ceiling, Vitellary will");
                }
                else if (lastsaved == 3)
                {
                    graphics.addline("   the ceiling, Vermilion will");
                }
                else if (lastsaved == 4)
                {
                    graphics.addline("   the ceiling, Verdigris will");
                }
                else if (lastsaved == 5)
                {
                    graphics.addline("   the ceiling, Victoria will");
                }
            }
            else
            {
                if (lastsaved == 2)
                {
                    graphics.addline("    the floor, Vitellary will");
                }
                else if (lastsaved == 3)
                {
                    graphics.addline("    the floor, Vermilion will");
                }
                else if (lastsaved == 4)
                {
                    graphics.addline("    the floor, Verdigris will");
                }
                else if (lastsaved == 5)
                {
                    graphics.addline("    the floor, Victoria will");
                }
            }

            graphics.addline("     stop and wait for you.");
            graphics.textboxtimer(180);
            state = 0;
            break;
        case 12:
            //Intermission 1 instructional textbox, depends on last saved
            obj.removetrigger(12);
            if (!obj.flags[61])
            {
                obj.flags[61] = true;
                graphics.textboxremovefast();
                graphics.createtextbox("  You can't continue to the next   ", -1, 8, 174, 174, 174);
                if (lastsaved == 5)
                {
                    graphics.addline("  room until she is safely across. ");
                }
                else
                {
                    graphics.addline("  room until he is safely across.  ");
                }
                graphics.textboxtimer(120);
            }
            state = 0;
            break;
        case 13:
            //textbox removal
            obj.removetrigger(13);
            graphics.textboxremovefast();
            state = 0;
            break;
        case 14:
            //Intermission 1 instructional textbox, depends on last saved
            if (graphics.flipmode)
            {
                graphics.createtextbox(" When you're standing on the ceiling, ", -1, 3, 174, 174, 174);
            }
            else
            {
                graphics.createtextbox(" When you're standing on the floor, ", -1, 3, 174, 174, 174);
            }
            if (lastsaved == 2)
            {
                graphics.addline(" Vitellary will try to walk to you. ");
            }
            else if (lastsaved == 3)
            {
                graphics.addline(" Vermilion will try to walk to you. ");
            }
            else if (lastsaved == 4)
            {
                graphics.addline(" Verdigris will try to walk to you. ");
            }
            else if (lastsaved == 5)
            {
                graphics.addline(" Victoria will try to walk to you. ");
            }
            graphics.textboxtimer(280);

            state = 0;
            break;

        case 15:
            //leaving the naughty corner
            obj.entities[obj.getplayer()].tile = 0;
            state = 0;
            break;
        case 16:
            //entering the naughty corner
            if(obj.entities[obj.getplayer()].tile == 0)
            {
                obj.entities[obj.getplayer()].tile = 144;
                music.playef(2);
            }
            state = 0;
            break;

        case 17:
            //Arrow key tutorial
            obj.removetrigger(17);
            graphics.createtextbox(" If you prefer, you can press UP or ", -1, 195, 174, 174, 174);
            graphics.addline("   DOWN instead of ACTION to flip.");
            graphics.textboxtimer(100);
            state = 0;
            break;

        case 20:
            if (!obj.flags[1])
            {
                obj.flags[1] = true;
                state = 0;
                graphics.textboxremove();
            }
            obj.removetrigger(20);
            break;
        case 21:
            if (!obj.flags[2])
            {
                obj.flags[2] = true;
                state = 0;
                graphics.textboxremove();
            }
            obj.removetrigger(21);
            break;
        case 22:
            if (!obj.flags[3])
            {
                graphics.textboxremovefast();
                obj.flags[3] = true;
                state = 0;
                graphics.createtextbox("  Press ACTION to flip  ", -1, 25, 174, 174, 174);
                graphics.textboxtimer(60);
            }
            obj.removetrigger(22);
            break;

        case 30:
            //Generic "run script"
            if (!obj.flags[4])
            {
                obj.flags[4] = true;
                startscript = true;
                newscript="firststeps";
                state = 0;
            }
            obj.removetrigger(30);
            state = 0;
            break;
        case 31:
            //state = 55;  statedelay = 50;
            state = 0;
            statedelay = 0;
            if (!obj.flags[6])
            {
                obj.flags[6] = true;

                obj.flags[5] = true;
                startscript = true;
                newscript="communicationstation";
                state = 0;
                statedelay = 0;
            }
            obj.removetrigger(31);
            break;
        case 32:
            //Generic "run script"
            if (!obj.flags[7])
            {
                obj.flags[7] = true;
                startscript = true;
                newscript="teleporterback";
                state = 0;
            }
            obj.removetrigger(32);
            state = 0;
            break;
        case 33:
            //Generic "run script"
            if (!obj.flags[9])
            {
                obj.flags[9] = true;
                startscript = true;
                newscript="rescueblue";
                state = 0;
            }
            obj.removetrigger(33);
            state = 0;
            break;
        case 34:
            //Generic "run script"
            if (!obj.flags[10])
            {
                obj.flags[10] = true;
                startscript = true;
                newscript="rescueyellow";
                state = 0;
            }
            obj.removetrigger(34);
            state = 0;
            break;
        case 35:
            //Generic "run script"
            if (!obj.flags[11])
            {
                obj.flags[11] = true;
                startscript = true;
                newscript="rescuegreen";
                state = 0;
            }
            obj.removetrigger(35);
            state = 0;
            break;
        case 36:
            //Generic "run script"
            if (!obj.flags[8])
            {
                obj.flags[8] = true;
                startscript = true;
                newscript="rescuered";
                state = 0;
            }
            obj.removetrigger(36);
            state = 0;
            break;

        case 37:
            //Generic "run script"
            if (companion == 0)
            {
                startscript = true;
                newscript="int2_yellow";
                state = 0;
            }
            obj.removetrigger(37);
            state = 0;
            break;
        case 38:
            //Generic "run script"
            if (companion == 0)
            {
                startscript = true;
                newscript="int2_red";
                state = 0;
            }
            obj.removetrigger(38);
            state = 0;
            break;
        case 39:
            //Generic "run script"
            if (companion == 0)
            {
                startscript = true;
                newscript="int2_green";
                state = 0;
            }
            obj.removetrigger(39);
            state = 0;
            break;
        case 40:
            //Generic "run script"
            if (companion == 0)
            {
                startscript = true;
                newscript="int2_blue";
                state = 0;
            }
            obj.removetrigger(40);
            state = 0;
            break;

        case 41:
            //Generic "run script"
            if (!obj.flags[60])
            {
                obj.flags[60] = true;
                startscript = true;
                if (lastsaved == 2)
                {
                    newscript = "int1yellow_2";
                }
                else if (lastsaved == 3)
                {
                    newscript = "int1red_2";
                }
                else if (lastsaved == 4)
                {
                    newscript = "int1green_2";
                }
                else if (lastsaved == 5)
                {
                    newscript = "int1blue_2";
                }
                state = 0;
            }
            obj.removetrigger(41);
            state = 0;
            break;
        case 42:
            //Generic "run script"
            if (!obj.flags[62])
            {
                obj.flags[62] = true;
                startscript = true;
                if (lastsaved == 2)
                {
                    newscript = "int1yellow_3";
                }
                else if (lastsaved == 3)
                {
                    newscript = "int1red_3";
                }
                else if (lastsaved == 4)
                {
                    newscript = "int1green_3";
                }
                else if (lastsaved == 5)
                {
                    newscript = "int1blue_3";
                }
                state = 0;
            }
            obj.removetrigger(42);
            state = 0;
            break;
        case 43:
            //Generic "run script"
            if (!obj.flags[63])
            {
                obj.flags[63] = true;
                startscript = true;
                if (lastsaved == 2)
                {
                    newscript = "int1yellow_4";
                }
                else if (lastsaved == 3)
                {
                    newscript = "int1red_4";
                }
                else if (lastsaved == 4)
                {
                    newscript = "int1green_4";
                }
                else if (lastsaved == 5)
                {
                    newscript = "int1blue_4";
                }
                state = 0;
            }
            obj.removetrigger(43);
            state = 0;
            break;
        case 44:
            //Generic "run script"
            if (!obj.flags[64])
            {
                obj.flags[64] = true;
                startscript = true;
                if (lastsaved == 2)
                {
                    newscript = "int1yellow_5";
                }
                else if (lastsaved == 3)
                {
                    newscript = "int1red_5";
                }
                else if (lastsaved == 4)
                {
                    newscript = "int1green_5";
                }
                else if (lastsaved == 5)
                {
                    newscript = "int1blue_5";
                }
                state = 0;
            }
            obj.removetrigger(44);
            state = 0;
            break;
        case 45:
            //Generic "run script"
            if (!obj.flags[65])
            {
                obj.flags[65] = true;
                startscript = true;
                if (lastsaved == 2)
                {
                    newscript = "int1yellow_6";
                }
                else if (lastsaved == 3)
                {
                    newscript = "int1red_6";
                }
                else if (lastsaved == 4)
                {
                    newscript = "int1green_6";
                }
                else if (lastsaved == 5)
                {
                    newscript = "int1blue_6";
                }
                state = 0;
            }
            obj.removetrigger(45);
            state = 0;
            break;
        case 46:
            //Generic "run script"
            if (!obj.flags[66])
            {
                obj.flags[66] = true;
                startscript = true;
                if (lastsaved == 2)
                {
                    newscript = "int1yellow_7";
                }
                else if (lastsaved == 3)
                {
                    newscript = "int1red_7";
                }
                else if (lastsaved == 4)
                {
                    newscript = "int1green_7";
                }
                else if (lastsaved == 5)
                {
                    newscript = "int1blue_7";
                }
                state = 0;
            }
            obj.removetrigger(46);
            state = 0;
            break;

        case 47:
            //Generic "run script"
            if (!obj.flags[69])
            {
                obj.flags[69] = true;
                startscript = true;
                newscript="trenchwarfare";
                state = 0;
            }
            obj.removetrigger(47);
            state = 0;
            break;
        case 48:
            //Generic "run script"
            if (!obj.flags[70])
            {
                obj.flags[70] = true;
                startscript = true;
                newscript="trinketcollector";
                state = 0;
            }
            obj.removetrigger(48);
            state = 0;
            break;
        case 49:
            //Start final level music
            if (!obj.flags[71])
            {
                obj.flags[71] = true;
                music.niceplay(15);  //Final level remix
                state = 0;
            }
            obj.removetrigger(49);
            state = 0;
            break;

        case 50:
            music.playef(15);
            graphics.createtextbox("Help! Can anyone hear", 35, 15, 255, 134, 255);
            graphics.addline("this message?");
            graphics.textboxtimer(60);
            state++;
            statedelay = 100;
            break;
        case 51:
            music.playef(15);
            graphics.createtextbox("Verdigris? Are you out", 30, 12, 255, 134, 255);
            graphics.addline("there? Are you ok?");
            graphics.textboxtimer(60);
            state++;
            statedelay = 100;
            break;
        case 52:
            music.playef(15);
            graphics.createtextbox("Please help us! We've crashed", 5, 22, 255, 134, 255);
            graphics.addline("and need assistance!");
            graphics.textboxtimer(60);
            state++;
            statedelay = 100;
            break;
        case 53:
            music.playef(15);
            graphics.createtextbox("Hello? Anyone out there?", 40, 15, 255, 134, 255);
            graphics.textboxtimer(60);
            state++;
            statedelay = 100;
            break;
        case 54:
            music.playef(15);
            graphics.createtextbox("This is Doctor Violet from the", 5, 8, 255, 134, 255);
            graphics.addline("D.S.S. Souleye! Please respond!");
            graphics.textboxtimer(60);
            state++;
            statedelay = 100;
            break;
        case 55:
            music.playef(15);
            graphics.createtextbox("Please... Anyone...", 45, 14, 255, 134, 255);
            graphics.textboxtimer(60);
            state++;
            statedelay = 100;
            break;
        case 56:
            music.playef(15);
            graphics.createtextbox("Please be alright, everyone...", 25, 18, 255, 134, 255);
            graphics.textboxtimer(60);
            state=50;
            statedelay = 100;
            break;


        case 80:
            //Used to return to menu from the game
            if(graphics.fademode == 1)	state++;
            break;
        case 81:
            quittomenu();
            state = 0;
            break;

        case 82:
            //Time Trial Complete!
            obj.removetrigger(82);
            hascontrol = false;
            timetrialresulttime = seconds + (minutes * 60);
            timetrialrank = 0;
            if (timetrialresulttime <= timetrialpar) timetrialrank++;
            if (trinkets() >= timetrialshinytarget) timetrialrank++;
            if (deathcounts == 0) timetrialrank++;

            if (timetrialresulttime < besttimes[timetriallevel] || besttimes[timetriallevel]==-1)
            {
                besttimes[timetriallevel] = timetrialresulttime;
            }
            if (trinkets() > besttrinkets[timetriallevel] || besttrinkets[timetriallevel]==-1)
            {
                besttrinkets[timetriallevel] = trinkets();
            }
            if (deathcounts < bestlives[timetriallevel] || bestlives[timetriallevel]==-1)
            {
                bestlives[timetriallevel] = deathcounts;
            }
            if (timetrialrank > bestrank[timetriallevel] || bestrank[timetriallevel]==-1)
            {
                bestrank[timetriallevel] = timetrialrank;
                if(timetrialrank>=3){
                    if(timetriallevel==0) NETWORK_unlockAchievement("vvvvvvtimetrial_station1_fixed");
                    if(timetriallevel==1) NETWORK_unlockAchievement("vvvvvvtimetrial_lab_fixed");
                    if(timetriallevel==2) NETWORK_unlockAchievement("vvvvvvtimetrial_tower_fixed");
                    if(timetriallevel==3) NETWORK_unlockAchievement("vvvvvvtimetrial_station2_fixed");
                    if(timetriallevel==4) NETWORK_unlockAchievement("vvvvvvtimetrial_warp_fixed");
                    if(timetriallevel==5) NETWORK_unlockAchievement("vvvvvvtimetrial_final_fixed");
                }
            }

            savestats();

            graphics.fademode = 2;
            music.fadeout();
            state++;
            break;
        case 83:
            frames--;
            if(graphics.fademode == 1)	state++;
            break;
        case 84:
            graphics.flipmode = false;
            gamestate = TITLEMODE;
            graphics.fademode = 4;
            graphics.backgrounddrawn = true;
            map.tdrawback = true;
            createmenu(Menu::timetrialcomplete);
            state = 0;
            break;


        case 85:
            //Cutscene skip version of final level change
            obj.removetrigger(85);
            //Init final stretch
            state++;
            music.playef(9);
            music.play(2);
            obj.flags[72] = true;

            screenshake = 10;
            flashlight = 5;
            map.finalstretch = true;
            map.warpx = false;
            map.warpy = false;
            map.background = 6;

            map.final_colormode = true;
            map.final_colorframe = 1;

            state = 0;
            break;

            //From 90-100 are run scripts for the eurogamer expo only, remove later
        case 90:
            //Generic "run script"
            startscript = true;
            newscript="startexpolevel_station1";
            obj.removetrigger(90);
            state = 0;
            break;
        case 91:
            //Generic "run script"
            startscript = true;
            newscript="startexpolevel_lab";
            obj.removetrigger(91);
            state = 0;
            break;
        case 92:
            //Generic "run script"
            startscript = true;
            newscript="startexpolevel_warp";
            obj.removetrigger(92);
            state = 0;
            break;
        case 93:
            //Generic "run script"
            startscript = true;
            newscript="startexpolevel_tower";
            obj.removetrigger(93);
            state = 0;
            break;
        case 94:
            //Generic "run script"
            startscript = true;
            newscript="startexpolevel_station2";
            obj.removetrigger(94);
            state = 0;
            break;
        case 95:
            //Generic "run script"
            startscript = true;
            newscript="startexpolevel_final";
            obj.removetrigger(95);
            state = 0;
            break;

        case 96:
            //Used to return to gravitron to game
            if(graphics.fademode == 1)	state++;
            break;
        case 97:
            returntolab();
            state = 0;
            break;

        case 100:
            //
            //                       Meeting crewmate in the warpzone
            //
            obj.removetrigger(100);
            if (!obj.flags[4])
            {
                obj.flags[4] = true;
                state++;
            }
            break;
        case 101:
        {


            i = obj.getplayer();
            hascontrol = false;
            if (obj.entities[i].onroof > 0 && gravitycontrol == 1)
            {
                gravitycontrol = 0;
                music.playef(1);
            }
            if (obj.entities[i].onground > 0)
            {
                state++;
            }
        }
        break;
        case 102:
        {


            companion = 6;
            i = obj.getcompanion();
            obj.entities[i].tile = 0;
            obj.entities[i].state = 1;

            advancetext = true;
            hascontrol = false;

            graphics.createtextbox("Captain! I've been so worried!", 60, 90, 164, 255, 164);
            state++;
            music.playef(12);
        }
        break;
        case 104:
            graphics.createtextbox("I'm glad you're ok!", 135, 152, 164, 164, 255);
            state++;
            music.playef(11);
            graphics.textboxactive();
            break;
        case 106:
        {
            graphics.createtextbox("I've been trying to find a", 74, 70, 164, 255, 164);
            graphics.addline("way out, but I keep going");
            graphics.addline("around in circles...");
            state++;
            music.playef(2);
            graphics.textboxactive();
            i = obj.getcompanion();
            obj.entities[i].tile = 54;
            obj.entities[i].state = 0;
        }
        break;
        case 108:
            graphics.createtextbox("Don't worry! I have a", 125, 152, 164, 164, 255);
            graphics.addline("teleporter key!");
            state++;
            music.playef(11);
            graphics.textboxactive();
            break;
        case 110:
        {

            i = obj.getcompanion();
            obj.entities[i].tile = 0;
            obj.entities[i].state = 1;
            graphics.createtextbox("Follow me!", 185, 154, 164, 164, 255);
            state++;
            music.playef(11);
            graphics.textboxactive();

        }
        break;
        case 112:
            graphics.textboxremove();
            hascontrol = true;
            advancetext = false;

            state = 0;
            break;

        case 115:
            //
            //                       Test script for space station, totally delete me!
            //
        {
            i = obj.getplayer();
            hascontrol = false;
            state++;
        }
        break;
        case 116:
            advancetext = true;
            hascontrol = false;

            graphics.createtextbox("Sorry Eurogamers! Teleporting around", 60 - 20, 200, 255, 64, 64);
            graphics.addline("the map doesn't work in this version!");
            graphics.textboxcenterx();
            state++;
            break;
        case 118:
            graphics.textboxremove();
            hascontrol = true;
            advancetext = false;

            state = 0;
            break;

        case 120:
            //
            //                       Meeting crewmate in the space station
            //
            obj.removetrigger(120);
            if (!obj.flags[5])
            {
                obj.flags[5] = true;
                state++;
            }
            break;
        case 121:
        {

            i = obj.getplayer();
            hascontrol = false;
            if (obj.entities[i].onground > 0 && gravitycontrol == 0)
            {
                gravitycontrol = 1;
                music.playef(1);
            }
            if (obj.entities[i].onroof > 0)
            {
                state++;
            }

        }
        break;
        case 122:
            companion = 7;
            i = obj.getcompanion();
            obj.entities[i].tile = 6;
            obj.entities[i].state = 1;

            advancetext = true;
            hascontrol = false;

            graphics.createtextbox("Captain! You're ok!", 60-10, 90-40, 255, 255, 134);
            state++;
            music.playef(14);
            break;
        case 124:
            graphics.createtextbox("I've found a teleporter, but", 60-20, 90 - 40, 255, 255, 134);
            graphics.addline("I can't get it to go anywhere...");
            state++;
            music.playef(2);
            graphics.textboxactive();
            i = obj.getcompanion();	//obj.entities[i].tile = 66;	obj.entities[i].state = 0;
            break;
        case 126:
            graphics.createtextbox("I can help with that!", 125, 152-40, 164, 164, 255);
            state++;
            music.playef(11);
            graphics.textboxactive();
            break;
        case 128:
            graphics.createtextbox("I have the teleporter", 130, 152-35, 164, 164, 255);
            graphics.addline("codex for our ship!");
            state++;
            music.playef(11);
            graphics.textboxactive();
            break;

        case 130:
            graphics.createtextbox("Yey! Let's go home!", 60-30, 90-35, 255, 255, 134);
            state++;
            music.playef(14);
            graphics.textboxactive();
            i = obj.getcompanion();
            obj.entities[i].tile = 6;
            obj.entities[i].state = 1;
            break;
        case 132:
            graphics.textboxremove();
            hascontrol = true;
            advancetext = false;

            state = 0;
            break;

        case 200:
            //Init final stretch
            state++;
            music.playef(9);
            //music.play(2);
            obj.flags[72] = true;

            screenshake = 10;
            flashlight = 5;
            map.finalstretch = true;
            map.warpx = false;
            map.warpy = false;
            map.background = 6;

            map.final_colormode = true;
            map.final_colorframe = 1;

            startscript = true;
            newscript="finalterminal_finish";
            state = 0;
            break;


        case 300:
            startscript = true;
            newscript="custom_"+customscript[0];
            obj.removetrigger(300);
            state = 0;
            break;
        case 301:
            startscript = true;
            newscript="custom_"+customscript[1];
            obj.removetrigger(301);
            state = 0;
            break;
        case 302:
            startscript = true;
            newscript="custom_"+customscript[2];
            obj.removetrigger(302);
            state = 0;
            break;
        case 303:
            startscript = true;
            newscript="custom_"+customscript[3];
            obj.removetrigger(303);
            state = 0;
            break;
        case 304:
            startscript = true;
            newscript="custom_"+customscript[4];
            obj.removetrigger(304);
            state = 0;
            break;
        case 305:
            startscript = true;
            newscript="custom_"+customscript[5];
            obj.removetrigger(305);
            state = 0;
            break;
        case 306:
            startscript = true;
            newscript="custom_"+customscript[6];
            obj.removetrigger(306);
            state = 0;
            break;
        case 307:
            startscript = true;
            newscript="custom_"+customscript[7];
            obj.removetrigger(307);
            state = 0;
            break;
        case 308:
            startscript = true;
            newscript="custom_"+customscript[8];
            obj.removetrigger(308);
            state = 0;
            break;
        case 309:
            startscript = true;
            newscript="custom_"+customscript[9];
            obj.removetrigger(309);
            state = 0;
            break;
        case 310:
            startscript = true;
            newscript="custom_"+customscript[10];
            obj.removetrigger(310);
            state = 0;
            break;
        case 311:
            startscript = true;
            newscript="custom_"+customscript[11];
            obj.removetrigger(311);
            state = 0;
            break;
        case 312:
            startscript = true;
            newscript="custom_"+customscript[12];
            obj.removetrigger(312);
            state = 0;
            break;
        case 313:
            startscript = true;
            newscript="custom_"+customscript[13];
            obj.removetrigger(313);
            state = 0;
            break;
        case 314:
            startscript = true;
            newscript="custom_"+customscript[14];
            obj.removetrigger(314);
            state = 0;
            break;
        case 315:
            startscript = true;
            newscript="custom_"+customscript[15];
            obj.removetrigger(315);
            state = 0;
            break;
        case 316:
            startscript = true;
            newscript="custom_"+customscript[16];
            obj.removetrigger(316);
            state = 0;
            break;
        case 317:
            startscript = true;
            newscript="custom_"+customscript[17];
            obj.removetrigger(317);
            state = 0;
            break;
        case 318:
            startscript = true;
            newscript="custom_"+customscript[18];
            obj.removetrigger(318);
            state = 0;
            break;
        case 319:
            startscript = true;
            newscript="custom_"+customscript[19];
            obj.removetrigger(319);
            state = 0;
            break;
        case 320:
            startscript = true;
            newscript="custom_"+customscript[20];
            obj.removetrigger(320);
            state = 0;
            break;
        case 321:
            startscript = true;
            newscript="custom_"+customscript[21];
            obj.removetrigger(321);
            state = 0;
            break;
        case 322:
            startscript = true;
            newscript="custom_"+customscript[22];
            obj.removetrigger(322);
            state = 0;
            break;
        case 323:
            startscript = true;
            newscript="custom_"+customscript[23];
            obj.removetrigger(323);
            state = 0;
            break;
        case 324:
            startscript = true;
            newscript="custom_"+customscript[24];
            obj.removetrigger(324);
            state = 0;
            break;
        case 325:
            startscript = true;
            newscript="custom_"+customscript[25];
            obj.removetrigger(325);
            state = 0;
            break;
        case 326:
            startscript = true;
            newscript="custom_"+customscript[26];
            obj.removetrigger(326);
            state = 0;
            break;
        case 327:
            startscript = true;
            newscript="custom_"+customscript[27];
            obj.removetrigger(327);
            state = 0;
            break;
        case 328:
            startscript = true;
            newscript="custom_"+customscript[28];
            obj.removetrigger(328);
            state = 0;
            break;
        case 329:
            startscript = true;
            newscript="custom_"+customscript[29];
            obj.removetrigger(329);
            state = 0;
            break;
        case 330:
            startscript = true;
            newscript="custom_"+customscript[30];
            obj.removetrigger(330);
            state = 0;
            break;
        case 331:
            startscript = true;
            newscript="custom_"+customscript[31];
            obj.removetrigger(331);
            state = 0;
            break;
        case 332:
            startscript = true;
            newscript="custom_"+customscript[32];
            obj.removetrigger(332);
            state = 0;
            break;
        case 333:
            startscript = true;
            newscript="custom_"+customscript[33];
            obj.removetrigger(333);
            state = 0;
            break;
        case 334:
            startscript = true;
            newscript="custom_"+customscript[34];
            obj.removetrigger(334);
            state = 0;
            break;
        case 335:
            startscript = true;
            newscript="custom_"+customscript[35];
            obj.removetrigger(335);
            state = 0;
            break;
        case 336:
            startscript = true;
            newscript="custom_"+customscript[36];
            obj.removetrigger(336);
            state = 0;
            break;

        case 1000:
            graphics.showcutscenebars = true;
            hascontrol = false;
            completestop = true;
            state++;
            statedelay = 15;
            break;
        case 1001:
            //Found a trinket!
            advancetext = true;
            state++;
            if (graphics.flipmode)
            {
                graphics.createtextbox("        Congratulations!       ", 50, 105, 174, 174, 174);
                graphics.addline("");
                graphics.addline("You have found a shiny trinket!");
                graphics.textboxcenterx();

#if !defined(NO_CUSTOM_LEVELS)
                if(map.custommode)
                {
                    graphics.createtextbox(" " + help.number(trinkets()) + " out of " + help.number(ed.numtrinkets())+ " ", 50, 65, 174, 174, 174);
                    graphics.textboxcenterx();
                }
                else
#endif
                {
                    graphics.createtextbox(" " + help.number(trinkets()) + " out of Twenty ", 50, 65, 174, 174, 174);
                    graphics.textboxcenterx();
                }
            }
            else
            {
                graphics.createtextbox("        Congratulations!       ", 50, 85, 174, 174, 174);
                graphics.addline("");
                graphics.addline("You have found a shiny trinket!");
                graphics.textboxcenterx();

#if !defined(NO_CUSTOM_LEVELS)
                if(map.custommode)
                {
                    graphics.createtextbox(" " + help.number(trinkets()) + " out of " + help.number(ed.numtrinkets())+ " ", 50, 135, 174, 174, 174);
                    graphics.textboxcenterx();
                }
                else
#endif
                {
                    graphics.createtextbox(" " + help.number(trinkets()) + " out of Twenty ", 50, 135, 174, 174, 174);
                    graphics.textboxcenterx();
                }
            }
            break;
        case 1003:
            graphics.textboxremove();
            hascontrol = true;
            advancetext = false;
            completestop = false;
            state = 0;
            //music.play(music.resumesong);
            if(!muted && music.currentsong>-1) music.fadeMusicVolumeIn(3000);
            graphics.showcutscenebars = false;
            break;

        case 1010:
            graphics.showcutscenebars = true;
            hascontrol = false;
            completestop = true;
            state++;
            statedelay = 15;
            break;
#if !defined(NO_CUSTOM_LEVELS)
        case 1011:
            //Found a crewmate!
            advancetext = true;
            state++;
            if (graphics.flipmode)
            {
                graphics.createtextbox("        Congratulations!       ", 50, 105, 174, 174, 174);
                graphics.addline("");
                graphics.addline("You have found a lost crewmate!");
                graphics.textboxcenterx();

                if(ed.numcrewmates()-crewmates()==0)
                {
                    graphics.createtextbox("     All crewmates rescued!    ", 50, 65, 174, 174, 174);
                }
                else if(ed.numcrewmates()-crewmates()==1)
                {
                    graphics.createtextbox("    " + help.number(ed.numcrewmates()-crewmates())+ " remains    ", 50, 65, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox("     " + help.number(ed.numcrewmates()-crewmates())+ " remain    ", 50, 65, 174, 174, 174);
                }
                graphics.textboxcenterx();

            }
            else
            {
                graphics.createtextbox("        Congratulations!       ", 50, 85, 174, 174, 174);
                graphics.addline("");
                graphics.addline("You have found a lost crewmate!");
                graphics.textboxcenterx();

                if(ed.numcrewmates()-crewmates()==0)
                {
                    graphics.createtextbox("     All crewmates rescued!    ", 50, 135, 174, 174, 174);
                }
                else if(ed.numcrewmates()-crewmates()==1)
                {
                    graphics.createtextbox("    " + help.number(ed.numcrewmates()-crewmates())+ " remains    ", 50, 135, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox("     " + help.number(ed.numcrewmates()-crewmates())+ " remain    ", 50, 135, 174, 174, 174);
                }
                graphics.textboxcenterx();
            }
            break;
        case 1013:
            graphics.textboxremove();
            hascontrol = true;
            advancetext = false;
            completestop = false;
            state = 0;

            if(ed.numcrewmates()-crewmates()==0)
            {
                if(map.custommodeforreal)
                {
                    graphics.fademode = 2;
                    if(!muted && ed.levmusic>0) music.fadeMusicVolumeIn(3000);
                    if(ed.levmusic>0) music.fadeout();
                    state=1014;
                }
                else
                {
                    gamestate = EDITORMODE;
                    graphics.backgrounddrawn=false;
                    if(!muted && ed.levmusic>0) music.fadeMusicVolumeIn(3000);
                    if(ed.levmusic>0) music.fadeout();
                }
            }
            else
            {
                if(!muted && ed.levmusic>0) music.fadeMusicVolumeIn(3000);
            }
            graphics.showcutscenebars = false;
            returntomenu(Menu::levellist);
            break;
#endif
        case 1014:
            frames--;
            if(graphics.fademode == 1)	state++;
            break;
        case 1015:
            graphics.flipmode = false;
            gamestate = TITLEMODE;
            graphics.fademode = 4;
            music.play(6);
            graphics.backgrounddrawn = true;
            map.tdrawback = true;
#if !defined(NO_CUSTOM_LEVELS)
            //Update level stats
            if(ed.numcrewmates()-crewmates()==0)
            {
                //Finished level
                if(ed.numtrinkets()-trinkets()==0)
                {
                    //and got all the trinkets!
                    updatecustomlevelstats(customlevelfilename, 3);
                }
                else
                {
                    updatecustomlevelstats(customlevelfilename, 1);
                }
            }
#endif
            state = 0;
            break;


        case 2000:
            //Game Saved!
            if (intimetrial || nodeathmode || inintermission)
            {
                state = 0;
            }
            else
            {
                savetele();
                if (graphics.flipmode)
                {
                    graphics.createtextbox("    Game Saved    ", -1, 202, 174, 174, 174);
                    graphics.textboxtimer(25);
                }
                else
                {
                    graphics.createtextbox("    Game Saved    ", -1, 12, 174, 174, 174);
                    graphics.textboxtimer(25);
                }
                state = 0;
            }
            break;

        case 2500:

            music.play(5);
            //Activating a teleporter (appear)
            state++;
            statedelay = 15;
            flashlight = 5;
            screenshake = 90;
            music.playef(9);
            break;
        case 2501:
            //Activating a teleporter 2
            state++;
            statedelay = 0;
            flashlight = 5;
            screenshake = 0;
            //we're done here!
            music.playef(10);
            break;
        case 2502:
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            i = obj.getplayer();
            obj.entities[i].colour = 0;
            obj.entities[i].invis = false;

            obj.entities[i].xp = obj.entities[obj.getteleporter()].xp+44;
            obj.entities[i].yp = obj.entities[obj.getteleporter()].yp+44;
            obj.entities[i].ay = -6;
            obj.entities[i].ax = 6;
            obj.entities[i].vy = -6;
            obj.entities[i].vx = 6;

            i = obj.getteleporter();
            obj.entities[i].tile = 1;
            obj.entities[i].colour = 101;
            break;
        case 2503:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 10;
            break;
        case 2504:
            state++;
            i = obj.getplayer();
            //obj.entities[i].xp += 10;
            break;
        case 2505:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 8;
            break;
        case 2506:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 6;
            break;
        case 2507:
            state++;
            i = obj.getplayer();
            //obj.entities[i].xp += 4;
            break;
        case 2508:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 2;
            break;
        case 2509:
            state++;
            statedelay = 15;
            i = obj.getplayer();
            obj.entities[i].xp += 1;
            break;
        case 2510:
            advancetext = true;
            hascontrol = false;
            graphics.createtextbox("Hello?", 125+24, 152-20, 164, 164, 255);
            state++;
            music.playef(11);
            graphics.textboxactive();
            break;
        case 2512:
            advancetext = true;
            hascontrol = false;
            graphics.createtextbox("Is anyone there?", 125+8, 152-24, 164, 164, 255);
            state++;
            music.playef(11);
            graphics.textboxactive();
            break;
        case 2514:
            graphics.textboxremove();
            hascontrol = true;
            advancetext = false;

            state = 0;
            music.play(3);
            break;


        case 3000:
            //Activating a teleporter (long version for level complete)
            state++;
            statedelay = 30;
            flashlight = 5;
            screenshake = 90;
            music.playef(9);
            break;
        case 3001:
            //Activating a teleporter 2
            state++;
            statedelay = 15;
            flashlight = 5;
            music.playef(9);
            break;
        case 3002:
            //Activating a teleporter 2
            state++;
            statedelay = 15;
            flashlight = 5;
            music.playef(9);
            break;
        case 3003:
            //Activating a teleporter 2
            state++;
            statedelay = 15;
            flashlight = 5;
            music.playef(9);
            break;
        case 3004:
            //Activating a teleporter 2
            state++;
            statedelay = 0;
            flashlight = 5;
            screenshake = 0;
            //we're done here!
            music.playef(10);
            break;
        case 3005:
            //Activating a teleporter 2
            state++;
            statedelay = 50;
            //testing!
            //state = 3006; //Warp Zone
            //state = 3020; //Space Station
            switch(companion)
            {
            case 6:
                state = 3006;
                break; //Warp Zone
            case 7:
                state = 3020;
                break; //Space Station
            case 8:
                state = 3040;
                break; //Lab
            case 9:
                state = 3060;
                break; //Tower
            case 10:
                state = 3080;
                break; //Intermission 2
            case 11:
                state = 3085;
                break; //Intermission 1
            }

            i = obj.getplayer();
            obj.entities[i].colour = 0;
            obj.entities[i].invis = true;

            i = obj.getcompanion();
            if(i>-1)
            {
                obj.removeentity(i);
            }

            i = obj.getteleporter();
            obj.entities[i].tile = 1;
            obj.entities[i].colour = 100;
            break;

        case 3006:
            //Level complete! (warp zone)
            unlocknum(4);
            lastsaved = 4;
            music.play(0);
            state++;
            statedelay = 75;

            if (graphics.flipmode)
            {
                graphics.createtextbox("", -1, 180, 165, 165, 255);
            }
            else
            {
                graphics.createtextbox("", -1, 12, 165, 165, 255);
            }
            //graphics.addline("      Level Complete!      ");
            graphics.addline("                                   ");
            graphics.addline("");
            graphics.addline("");
            graphics.textboxcenterx();

            /*												advancetext = true;
            hascontrol = false;
            state = 3;
            graphics.createtextbox("To do: write quick", 50, 80, 164, 164, 255);
            graphics.addline("intro to story!");*/
            break;
        case 3007:
            state++;
            statedelay = 45;

            if (graphics.flipmode)
            {
                graphics.createtextbox("", -1, 104, 175,174,174);
            }
            else
            {
                graphics.createtextbox("", -1, 64+8+16, 175,174,174);
            }
            graphics.addline("     You have rescued  ");
            graphics.addline("      a crew member!   ");
            graphics.addline("");
            graphics.textboxcenterx();
            break;
        case 3008:
        {
            state++;
            statedelay = 45;

            int temp = 6 - crewrescued();
            if (temp == 1)
            {
                std::string tempstring = "  One remains  ";
                if (graphics.flipmode)
                {
                    graphics.createtextbox(tempstring, -1, 72, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox(tempstring, -1, 128+16, 174, 174, 174);
                }
            }
            else if (temp > 0)
            {
                std::string tempstring = "  " + help.number(temp) + " remain  ";
                if (graphics.flipmode)
                {
                    graphics.createtextbox(tempstring, -1, 72, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox(tempstring, -1, 128+16, 174, 174, 174);
                }
            }
            else
            {
                if (graphics.flipmode)
                {
                    graphics.createtextbox("  All Crew Members Rescued!  ", -1, 72, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox("  All Crew Members Rescued!  ", -1, 128+16, 174, 174, 174);
                }
            }
            graphics.textboxcenterx();
            break;
        }
        case 3009:
            state++;
            statedelay = 0;

            if (graphics.flipmode)
            {
                graphics.createtextbox(" Press ACTION to continue ", -1, 20, 164, 164, 255);
            }
            else
            {
                graphics.createtextbox(" Press ACTION to continue ", -1, 196, 164, 164, 255);
            }
            graphics.textboxcenterx();
            break;
        case 3010:
            if (jumppressed)
            {
                state++;
                statedelay = 30;
                graphics.textboxremove();
            }
            break;
        case 3011:
            state = 3070;
            statedelay = 0;
            break;

        case 3020:
            //Level complete! (Space Station 2)
            unlocknum(3);
            lastsaved = 2;
            music.play(0);
            state++;
            statedelay = 75;


            if (graphics.flipmode)
            {
                graphics.createtextbox("", -1, 180, 165, 165, 255);
            }
            else
            {
                graphics.createtextbox("", -1, 12, 165, 165, 255);
            }
            //graphics.addline("      Level Complete!      ");
            graphics.addline("                                   ");
            graphics.addline("");
            graphics.addline("");
            graphics.textboxcenterx();

            /*												advancetext = true;
            hascontrol = false;
            state = 3;
            graphics.createtextbox("To do: write quick", 50, 80, 164, 164, 255);
            graphics.addline("intro to story!");*/
            break;
        case 3021:
            state++;
            statedelay = 45;

            if (graphics.flipmode)
            {
                graphics.createtextbox("", -1, 104, 174,175,174);
            }
            else
            {
                graphics.createtextbox("", -1, 64+8+16, 174,175,174);
            }
            graphics.addline("     You have rescued  ");
            graphics.addline("      a crew member!   ");
            graphics.addline("");
            graphics.textboxcenterx();
            break;
        case 3022:
        {
            state++;
            statedelay = 45;

            int temp = 6 - crewrescued();
            if (temp == 1)
            {
                std::string tempstring = "  One remains  ";
                if (graphics.flipmode)
                {
                    graphics.createtextbox(tempstring, -1, 72, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox(tempstring, -1, 128+16, 174, 174, 174);
                }
            }
            else if (temp > 0)
            {
                std::string tempstring = "  " + help.number(temp) + " remain  ";
                if (graphics.flipmode)
                {
                    graphics.createtextbox(tempstring, -1, 72, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox(tempstring, -1, 128+16, 174, 174, 174);
                }
            }
            else
            {
                if (graphics.flipmode)
                {
                    graphics.createtextbox("  All Crew Members Rescued!  ", -1, 72, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox("  All Crew Members Rescued!  ", -1, 128+16, 174, 174, 174);
                }
            }
            graphics.textboxcenterx();
            break;
        }
        case 3023:
            state++;
            statedelay = 0;

            if (graphics.flipmode)
            {
                graphics.createtextbox(" Press ACTION to continue ", -1, 20, 164, 164, 255);
            }
            else
            {
                graphics.createtextbox(" Press ACTION to continue ", -1, 196, 164, 164, 255);
            }
            graphics.textboxcenterx();
            break;
        case 3024:
            if (jumppressed)
            {
                state++;
                statedelay = 30;
                graphics.textboxremove();
            }
            break;
        case 3025:
            state = 3070;
            statedelay = 0;
            break;

        case 3040:
            //Level complete! (Lab)
            unlocknum(1);
            lastsaved = 5;
            music.play(0);
            state++;
            statedelay = 75;

            if (graphics.flipmode)
            {
                graphics.createtextbox("", -1, 180, 165, 165, 255);
            }
            else
            {
                graphics.createtextbox("", -1, 12, 165, 165, 255);
            }
            //graphics.addline("      Level Complete!      ");
            graphics.addline("                                   ");
            graphics.addline("");
            graphics.addline("");
            graphics.textboxcenterx();

            /*												advancetext = true;
            hascontrol = false;
            state = 3;
            graphics.createtextbox("To do: write quick", 50, 80, 164, 164, 255);
            graphics.addline("intro to story!");*/
            break;
        case 3041:
            state++;
            statedelay = 45;

            if (graphics.flipmode)
            {
                graphics.createtextbox("", -1, 104, 174,174,175);
            }
            else
            {
                graphics.createtextbox("", -1, 64+8+16, 174,174,175);
            }
            graphics.addline("     You have rescued  ");
            graphics.addline("      a crew member!   ");
            graphics.addline("");
            graphics.textboxcenterx();
            break;
        case 3042:
        {
            state++;
            statedelay = 45;

            int temp = 6 - crewrescued();
            if (temp == 1)
            {
                std::string tempstring = "  One remains  ";
                if (graphics.flipmode)
                {
                    graphics.createtextbox(tempstring, -1, 72, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox(tempstring, -1, 128+16, 174, 174, 174);
                }
            }
            else if (temp > 0)
            {
                std::string tempstring = "  " + help.number(temp) + " remain  ";
                if (graphics.flipmode)
                {
                    graphics.createtextbox(tempstring, -1, 72, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox(tempstring, -1, 128+16, 174, 174, 174);
                }
            }
            else
            {
                if (graphics.flipmode)
                {
                    graphics.createtextbox("  All Crew Members Rescued!  ", -1, 72, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox("  All Crew Members Rescued!  ", -1, 128+16, 174, 174, 174);
                }
            }
            graphics.textboxcenterx();
            break;
        }
        case 3043:
            state++;
            statedelay = 0;

            if (graphics.flipmode)
            {
                graphics.createtextbox(" Press ACTION to continue ", -1, 20, 164, 164, 255);
            }
            else
            {
                graphics.createtextbox(" Press ACTION to continue ", -1, 196, 164, 164, 255);
            }
            graphics.textboxcenterx();
            break;
        case 3044:
            if (jumppressed)
            {
                state++;
                statedelay = 30;
                graphics.textboxremove();
            }
            break;
        case 3045:
            state = 3070;
            statedelay = 0;
            break;

        case 3050:
            //Level complete! (Space Station 1)
            unlocknum(0);
            lastsaved = 1;
            music.play(0);
            state++;
            statedelay = 75;


            if (graphics.flipmode)
            {
                graphics.createtextbox("", -1, 180, 165, 165, 255);
            }
            else
            {
                graphics.createtextbox("", -1, 12, 165, 165, 255);
            }
            //graphics.addline("      Level Complete!      ");
            graphics.addline("                                   ");
            graphics.addline("");
            graphics.addline("");
            graphics.textboxcenterx();

            /*												advancetext = true;
            hascontrol = false;
            state = 3;
            graphics.createtextbox("To do: write quick", 50, 80, 164, 164, 255);
            graphics.addline("intro to story!");*/
            break;
        case 3051:
            state++;
            statedelay = 45;

            if (graphics.flipmode)
            {
                graphics.createtextbox("", -1, 104, 175,175,174);
            }
            else
            {
                graphics.createtextbox("", -1, 64+8+16, 175,175,174);
            }
            graphics.addline("     You have rescued  ");
            graphics.addline("      a crew member!   ");
            graphics.addline("");
            graphics.textboxcenterx();
            break;
        case 3052:
        {
            state++;
            statedelay = 45;

            int temp = 6 - crewrescued();
            if (temp == 1)
            {
                std::string tempstring = "  One remains  ";
                if (graphics.flipmode)
                {
                    graphics.createtextbox(tempstring, -1, 72, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox(tempstring, -1, 128+16, 174, 174, 174);
                }
            }
            else if (temp > 0)
            {
                std::string tempstring = "  " + help.number(temp) + " remain  ";
                if (graphics.flipmode)
                {
                    graphics.createtextbox(tempstring, -1, 72, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox(tempstring, -1, 128+16, 174, 174, 174);
                }
            }
            else
            {
                if (graphics.flipmode)
                {
                    graphics.createtextbox("  All Crew Members Rescued!  ", -1, 72, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox("  All Crew Members Rescued!  ", -1, 128+16, 174, 174, 174);
                }
            }
            graphics.textboxcenterx();
            break;
        }
        case 3053:
            state++;
            statedelay = 0;

            if (graphics.flipmode)
            {
                graphics.createtextbox(" Press ACTION to continue ", -1, 20, 164, 164, 255);
            }
            else
            {
                graphics.createtextbox(" Press ACTION to continue ", -1, 196, 164, 164, 255);
            }
            graphics.textboxcenterx();
            break;
        case 3054:
            if (jumppressed)
            {
                state++;
                statedelay = 30;
                graphics.textboxremove();
                crewstats[1] = 0; //Set violet's rescue script to 0 to make the next bit easier
                teleportscript = "";
            }
            break;
        case 3055:
            graphics.fademode = 2;
            state++;
            statedelay = 10;
            break;
        case 3056:
            if(graphics.fademode==1)
            {
                startscript = true;
                if (nocutscenes)
                {
                    newscript="bigopenworldskip";
                }
                else
                {
                    newscript = "bigopenworld";
                }
                state = 0;
            }
            break;


        case 3060:
            //Level complete! (Tower)
            unlocknum(2);
            lastsaved = 3;
            music.play(0);
            state++;
            statedelay = 75;

            if (graphics.flipmode)
            {
                graphics.createtextbox("", -1, 180, 165, 165, 255);
            }
            else
            {
                graphics.createtextbox("", -1, 12, 165, 165, 255);
            }
            //graphics.addline("      Level Complete!      ");
            graphics.addline("                                   ");
            graphics.addline("");
            graphics.addline("");
            graphics.textboxcenterx();

            /*												advancetext = true;
            hascontrol = false;
            state = 3;
            graphics.createtextbox("To do: write quick", 50, 80, 164, 164, 255);
            graphics.addline("intro to story!");*/
            break;
        case 3061:
            state++;
            statedelay = 45;

            if (graphics.flipmode)
            {
                graphics.createtextbox("", -1, 104, 175,174,175);
            }
            else
            {
                graphics.createtextbox("", -1, 64+8+16, 175,174,175);
            }
            graphics.addline("     You have rescued  ");
            graphics.addline("      a crew member!   ");
            graphics.addline("");
            graphics.textboxcenterx();
            break;
        case 3062:
        {
            state++;
            statedelay = 45;

            int temp = 6 - crewrescued();
            if (temp == 1)
            {
                std::string tempstring = "  One remains  ";
                if (graphics.flipmode)
                {
                    graphics.createtextbox(tempstring, -1, 72, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox(tempstring, -1, 128+16, 174, 174, 174);
                }
            }
            else if (temp > 0)
            {
                std::string tempstring = "  " + help.number(temp) + " remain  ";
                if (graphics.flipmode)
                {
                    graphics.createtextbox(tempstring, -1, 72, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox(tempstring, -1, 128+16, 174, 174, 174);
                }
            }
            else
            {
                if (graphics.flipmode)
                {
                    graphics.createtextbox("  All Crew Members Rescued!  ", -1, 72, 174, 174, 174);
                }
                else
                {
                    graphics.createtextbox("  All Crew Members Rescued!  ", -1, 128+16, 174, 174, 174);
                }
            }
            graphics.textboxcenterx();
            break;
        }
        case 3063:
            state++;
            statedelay = 0;

            if (graphics.flipmode)
            {
                graphics.createtextbox(" Press ACTION to continue ", -1, 20, 164, 164, 255);
            }
            else
            {
                graphics.createtextbox(" Press ACTION to continue ", -1, 196, 164, 164, 255);
            }
            graphics.textboxcenterx();
            break;
        case 3064:
            if (jumppressed)
            {
                state++;
                statedelay = 30;
                graphics.textboxremove();
            }
            break;
        case 3065:
            state = 3070;
            statedelay = 0;
            break;


        case 3070:
            graphics.fademode = 2;
            state++;
            break;
        case 3071:
            if (graphics.fademode == 1) state++;
            break;
        case 3072:
            //Ok, we need to adjust some flags based on who've we've rescued. Some of there conversation options
            //change depending on when they get back to the ship.
            if (lastsaved == 2)
            {
                if (crewstats[3]) obj.flags[25] = true;
                if (crewstats[4]) obj.flags[26] = true;
                if (crewstats[5]) obj.flags[24] = true;
            }
            else if (lastsaved == 3)
            {
                if (crewstats[2]) obj.flags[50] = true;
                if (crewstats[4]) obj.flags[49] = true;
                if (crewstats[5]) obj.flags[48] = true;
            }
            else if (lastsaved == 4)
            {
                if (crewstats[2]) obj.flags[54] = true;
                if (crewstats[3]) obj.flags[55] = true;
                if (crewstats[5]) obj.flags[56] = true;
            }
            else if (lastsaved == 5)
            {
                if (crewstats[2]) obj.flags[37] = true;
                if (crewstats[3]) obj.flags[38] = true;
                if (crewstats[4]) obj.flags[39] = true;
            }
            //We're pitch black now, make a decision
            companion = 0;
            if (crewrescued() == 6)
            {
                startscript = true;
                newscript="startlevel_final";
                state = 0;
            }
            else if (crewrescued() == 4)
            {
                companion = 11;
                supercrewmate = true;
                scmprogress = 0;

                startscript = true;
                newscript = "intermission_1";
                obj.flags[19] = true;
                if (lastsaved == 2) obj.flags[32] = true;
                if (lastsaved == 3) obj.flags[35] = true;
                if (lastsaved == 4) obj.flags[34] = true;
                if (lastsaved == 5) obj.flags[33] = true;
                state = 0;
            }
            else if (crewrescued() == 5)
            {
                startscript = true;
                newscript = "intermission_2";
                obj.flags[20] = true;
                if (lastsaved == 2) obj.flags[32] = true;
                if (lastsaved == 3) obj.flags[35] = true;
                if (lastsaved == 4) obj.flags[34] = true;
                if (lastsaved == 5) obj.flags[33] = true;
                state = 0;
            }
            else
            {
                startscript = true;
                newscript="regularreturn";
                state = 0;
            }
            break;

        case 3080:
            //returning from an intermission, very like 3070
            if (inintermission)
            {
                graphics.fademode = 2;
                companion = 0;
                returnmenu();
                state=3100;
            }
            else
            {
                unlocknum(7);
                graphics.fademode = 2;
                companion = 0;
                state++;
            }
            break;
        case 3081:
            if (graphics.fademode == 1) state++;
            break;
        case 3082:
            map.finalmode = false;
            startscript = true;
            newscript="regularreturn";
            state = 0;
            break;

        case 3085:
            //returning from an intermission, very like 3070
            //return to menu from here
            if (inintermission)
            {
                companion = 0;
                supercrewmate = false;
                state++;
                graphics.fademode = 2;
                music.fadeout();
                returnmenu();
                state=3100;
            }
            else
            {
                unlocknum(6);
                graphics.fademode = 2;
                companion = 0;
                supercrewmate = false;
                state++;
            }
            break;
        case 3086:
            if (graphics.fademode == 1) state++;
            break;
        case 3087:
            map.finalmode = false;
            startscript = true;
            newscript="regularreturn";
            state = 0;
            break;

        case 3100:
            if(graphics.fademode == 1)	state++;
            break;
        case 3101:
            graphics.flipmode = false;
            gamestate = TITLEMODE;
            graphics.fademode = 4;
            graphics.backgrounddrawn = true;
            map.tdrawback = true;
            music.play(6);
            state = 0;
            break;

            //startscript = true;	newscript="returntohub";
            //state = 0;

            /*case 3025:
            if (recording == 1) {
            //if recording the input, output it to debug here
            trace(recordstring);
            help.toclipboard(recordstring);
            }
            test = true; teststring = recordstring;
            graphics.createtextbox("   Congratulations!    ", 50, 80, 164, 164, 255);
            graphics.addline("");
            graphics.addline("Your play of this level has");
            graphics.addline("been copied to the clipboard.");
            graphics.addline("");
            graphics.addline("Please consider pasting and");
            graphics.addline("sending it to me! Even if you");
            graphics.addline("made a lot of mistakes - knowing");
            graphics.addline("exactly where people are having");
            graphics.addline("trouble is extremely useful!");
            graphics.textboxcenter();
            state = 0;
            break;*/

        case 3500:
            music.fadeout();
            state++;
            statedelay = 120;
            //state = 3511; //testing
            break;
        case 3501:
            //Game complete!
            NETWORK_unlockAchievement("vvvvvvgamecomplete");
            unlocknum(5);
            crewstats[0] = true;
            state++;
            statedelay = 75;
            music.play(7);

            if (graphics.flipmode)
            {
                graphics.createtextbox("", -1, 180, 164, 165, 255);
            }
            else
            {
                graphics.createtextbox("", -1, 12, 164, 165, 255);
            }
            graphics.addline("                                   ");
            graphics.addline("");
            graphics.addline("");
            graphics.textboxcenterx();
            break;
        case 3502:
            state++;
            statedelay = 45+15;

            if (graphics.flipmode)
            {
                graphics.createtextbox("  All Crew Members Rescued!  ", -1, 175-24, 0, 0, 0);
            }
            else
            {
                graphics.createtextbox("  All Crew Members Rescued!  ", -1, 64, 0, 0, 0);
            }
            savetime = timestring();
            break;
        case 3503:
        {
            state++;
            statedelay = 45;

            std::string tempstring = help.number(trinkets());
            if (graphics.flipmode)
            {
                graphics.createtextbox("Trinkets Found:", 48, 155-24, 0,0,0);
                graphics.createtextbox(tempstring, 180, 155-24, 0, 0, 0);
            }
            else
            {
                graphics.createtextbox("Trinkets Found:", 48, 84, 0,0,0);
                graphics.createtextbox(tempstring, 180, 84, 0, 0, 0);
            }
            break;
        }
        case 3504:
        {
            state++;
            statedelay = 45+15;

            std::string tempstring = savetime;
            if (graphics.flipmode)
            {
                graphics.createtextbox("   Game Time:", 64, 143-24, 0,0,0);
                graphics.createtextbox(tempstring, 180, 143-24, 0, 0, 0);
            }
            else
            {
                graphics.createtextbox("   Game Time:", 64, 96, 0,0,0);
                graphics.createtextbox(tempstring, 180, 96, 0, 0, 0);
            }
            break;
        }
        case 3505:
            state++;
            statedelay = 45;

            if (graphics.flipmode)
            {
                graphics.createtextbox(" Total Flips:", 64, 116-24, 0,0,0);
                graphics.createtextbox(help.String(totalflips), 180, 116-24, 0, 0, 0);
            }
            else
            {
                graphics.createtextbox(" Total Flips:", 64, 123, 0,0,0);
                graphics.createtextbox(help.String(totalflips), 180, 123, 0, 0, 0);
            }
            break;
        case 3506:
            state++;
            statedelay = 45+15;

            if (graphics.flipmode)
            {
                graphics.createtextbox("Total Deaths:", 64, 104-24, 0,0,0);
                graphics.createtextbox(help.String(deathcounts), 180, 104-24, 0, 0, 0);
            }
            else
            {
                graphics.createtextbox("Total Deaths:", 64, 135, 0,0,0);
                graphics.createtextbox(help.String(deathcounts), 180, 135, 0, 0, 0);
            }
            break;
        case 3507:
        {
            state++;
            statedelay = 45+15;

            if (graphics.flipmode)
            {
                std::string tempstring = "Hardest Room (with " + help.String(hardestroomdeaths) + " deaths)";
                graphics.createtextbox(tempstring, -1, 81-24, 0,0,0);
                graphics.createtextbox(hardestroom, -1, 69-24, 0, 0, 0);
            }
            else
            {
                std::string tempstring = "Hardest Room (with " + help.String(hardestroomdeaths) + " deaths)";
                graphics.createtextbox(tempstring, -1, 158, 0,0,0);
                graphics.createtextbox(hardestroom, -1, 170, 0, 0, 0);
            }
            break;
        }
        case 3508:
            state++;
            statedelay = 0;

            if (graphics.flipmode)
            {
                graphics.createtextbox(" Press ACTION to continue ", -1, 20, 164, 164, 255);
            }
            else
            {
                graphics.createtextbox(" Press ACTION to continue ", -1, 196, 164, 164, 255);
            }
            graphics.textboxcenterx();
            break;
        case 3509:
            if (jumppressed)
            {
                state++;
                statedelay = 30;
                graphics.textboxremove();
            }
            break;
        case 3510:
            //Save stats and stuff here
            if (!obj.flags[73])
            {
                //flip mode complete
                NETWORK_unlockAchievement("vvvvvvgamecompleteflip");
                unlock[19] = true;
            }

            if (bestgamedeaths == -1)
            {
                bestgamedeaths = deathcounts;
            }
            else
            {
                if (deathcounts < bestgamedeaths)
                {
                    bestgamedeaths = deathcounts;
                }
            }

            if (bestgamedeaths > -1) {
                if (bestgamedeaths <= 500) {
                    NETWORK_unlockAchievement("vvvvvvcomplete500");
                }
            if (bestgamedeaths <= 250) {
                NETWORK_unlockAchievement("vvvvvvcomplete250");
            }
            if (bestgamedeaths <= 100) {
                NETWORK_unlockAchievement("vvvvvvcomplete100");
            }
            if (bestgamedeaths <= 50) {
                NETWORK_unlockAchievement("vvvvvvcomplete50");
            }
        }


            savestats();
            if (nodeathmode)
            {
                NETWORK_unlockAchievement("vvvvvvmaster"); //bloody hell
                unlock[20] = true;
                state = 3520;
                statedelay = 0;
            }
            else
            {
                statedelay = 120;
                state++;
            }
            break;
        case 3511:
            //Activating a teleporter (long version for level complete)
            i = obj.getplayer();
            obj.entities[i].colour = 102;

            obj.flags[67] = true;

            state++;
            statedelay = 30;
            flashlight = 5;
            screenshake = 90;
            music.playef(9);
            break;
        case 3512:
            //Activating a teleporter 2
            state++;
            statedelay = 15;
            flashlight = 5;
            music.playef(9);
            break;
        case 3513:
            //Activating a teleporter 2
            state++;
            statedelay = 15;
            flashlight = 5;
            music.playef(9);
            break;
        case 3514:
            //Activating a teleporter 2
            state++;
            statedelay = 15;
            flashlight = 5;
            music.playef(9);
            break;
        case 3515:
            //Activating a teleporter 2
            state++;
            statedelay = 0;
            flashlight = 5;
            screenshake = 0;

            i = obj.getplayer();
            obj.entities[i].colour = 0;
            obj.entities[i].invis = true;

            //we're done here!
            music.playef(10);
            statedelay = 60;
            break;
        case 3516:
            graphics.fademode = 2;
            state++;
            break;
        case 3517:
            if (graphics.fademode == 1)
            {
                state++;
                statedelay = 30;
            }
            break;
        case 3518:
            graphics.fademode = 4;
            state = 0;
            statedelay = 30;
            //music.play(5);
            //music.play(10);

            map.finalmode = false;
            map.final_colormode = false;
            map.final_mapcol = 0;
            map.final_colorframe = 0;
            map.finalstretch = false;
            map.finalx = 100;
            map.finaly = 100;

            graphics.cutscenebarspos = 320;

            teleport_to_new_area = true;
            teleportscript = "gamecomplete";
            break;

        case 3520:
            //NO DEATH MODE COMPLETE JESUS
            hascontrol = false;
            crewstats[0] = true;

            graphics.fademode = 2;
            state++;
            break;
        case 3521:
            if(graphics.fademode == 1)	state++;
            break;
        case 3522:
            graphics.flipmode = false;
            gamestate = TITLEMODE;
            graphics.fademode = 4;
            graphics.backgrounddrawn = true;
            map.tdrawback = true;
            createmenu(Menu::nodeathmodecomplete);
            state = 0;
            break;

        case 4000:
            //Activating a teleporter (short version)
            state++;
            statedelay = 10;
            flashlight = 5;
            screenshake = 10;
            music.playef(9);
            break;
        case 4001:
            //Activating a teleporter 2
            state++;
            statedelay = 0;
            flashlight = 5;
            screenshake = 0;
            //we're done here!
            music.playef(10);
            break;
        case 4002:
            //Activating a teleporter 2
            state++;
            statedelay = 10;
            //testing!
            //state = 3006; //Warp Zone
            //state = 3020; //Space Station
            //state = 3040; //Lab

            i = obj.getplayer();
            obj.entities[i].colour = 0;
            obj.entities[i].invis = true;

            i = obj.getteleporter();
            if(i>-1)
            {
                obj.entities[i].tile = 1;
                obj.entities[i].colour = 100;
            }
            break;
        case 4003:
            state = 0;
            statedelay = 0;
            teleport_to_new_area = true;
            break;

        case 4010:
            //Activating a teleporter (default appear)
            state++;
            statedelay = 15;
            flashlight = 5;
            screenshake = 90;
            music.playef(9);
            break;
        case 4011:
            //Activating a teleporter 2
            state++;
            statedelay = 0;
            flashlight = 5;
            screenshake = 0;
            music.playef(10);
            break;
        case 4012:
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            i = obj.getplayer();
            j = obj.getteleporter();
            if (j != -1)
            {
                obj.entities[i].xp = obj.entities[j].xp+44;
                obj.entities[i].yp = obj.entities[j].yp+44;
                obj.entities[j].tile = 2;
                obj.entities[j].colour = 101;
            }
            obj.entities[i].colour = 0;
            obj.entities[i].invis = false;
            obj.entities[i].dir = 1;

            obj.entities[i].ay = -6;
            obj.entities[i].ax = 6;
            obj.entities[i].vy = -6;
            obj.entities[i].vx = 6;
            break;
        case 4013:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 10;
            break;
        case 4014:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 10;
            break;
        case 4015:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 8;
            break;
        case 4016:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 6;
            break;
        case 4017:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 3;
            break;
        case 4018:
            state++;
            statedelay = 15;
            i = obj.getplayer();
            obj.entities[i].xp += 1;
            break;
        case 4019:
            if (intimetrial || nodeathmode || inintermission)
            {
            }
            else
            {
                savetele();
            }
            i = obj.getteleporter();
            activetele = true;
            teleblock.x = obj.entities[i].xp - 32;
            teleblock.y = obj.entities[i].yp - 32;
            teleblock.w = 160;
            teleblock.h = 160;
            hascontrol = true;
            advancetext = false;
            state = 0;
            break;

        case 4020:
            //Activating a teleporter (default appear)
            state++;
            statedelay = 15;
            flashlight = 5;
            screenshake = 90;
            music.playef(9);
            break;
        case 4021:
            //Activating a teleporter 2
            state++;
            statedelay = 0;
            flashlight = 5;
            screenshake = 0;
            music.playef(10);
            break;
        case 4022:
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            i = obj.getplayer();
            j = obj.getteleporter();
            if (j != -1)
            {
                obj.entities[i].xp = obj.entities[j].xp+44;
                obj.entities[i].yp = obj.entities[j].yp+44;
                obj.entities[j].tile = 2;
                obj.entities[j].colour = 101;
            }
            obj.entities[i].colour = 0;
            obj.entities[i].invis = false;
            obj.entities[i].dir = 1;

            obj.entities[i].ay = -6;
            obj.entities[i].ax = 6;
            obj.entities[i].vy = -6;
            obj.entities[i].vx = 6;
            break;
        case 4023:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 12;
            break;
        case 4024:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 12;
            break;
        case 4025:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 10;
            break;
        case 4026:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 8;
            break;
        case 4027:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 5;
            break;
        case 4028:
            state++;
            statedelay = 15;
            i = obj.getplayer();
            obj.entities[i].xp += 2;
            break;
        case 4029:
            hascontrol = true;
            advancetext = false;
            state = 0;
            break;

        case 4030:
            //Activating a teleporter (default appear)
            state++;
            statedelay = 15;
            flashlight = 5;
            screenshake = 90;
            music.playef(9);
            break;
        case 4031:
            //Activating a teleporter 2
            state++;
            statedelay = 0;
            flashlight = 5;
            screenshake = 0;
            music.playef(10);
            break;
        case 4032:
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            i = obj.getplayer();
            j = obj.getteleporter();
            if (j != -1)
            {
                obj.entities[i].xp = obj.entities[j].xp+44;
                obj.entities[i].yp = obj.entities[j].yp+44;
                obj.entities[j].tile = 2;
                obj.entities[j].colour = 101;
            }
            obj.entities[i].colour = 0;
            obj.entities[i].invis = false;
            obj.entities[i].dir = 0;

            obj.entities[i].ay = -6;
            obj.entities[i].ax = -6;
            obj.entities[i].vy = -6;
            obj.entities[i].vx = -6;
            break;
        case 4033:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp -= 12;
            break;
        case 4034:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp -= 12;
            break;
        case 4035:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp -= 10;
            break;
        case 4036:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp -= 8;
            break;
        case 4037:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp -= 5;
            break;
        case 4038:
            state++;
            statedelay = 15;
            i = obj.getplayer();
            obj.entities[i].xp -= 2;
            break;
        case 4039:
            hascontrol = true;
            advancetext = false;
            state = 0;
            break;

        case 4040:
            //Activating a teleporter (default appear)
            state++;
            statedelay = 15;
            flashlight = 5;
            screenshake = 90;
            music.playef(9);
            break;
        case 4041:
            //Activating a teleporter 2
            state++;
            statedelay = 0;
            flashlight = 5;
            screenshake = 0;
            music.playef(10);
            break;
        case 4042:
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            i = obj.getplayer();
            j = obj.getteleporter();
            if (j != -1)
            {
                obj.entities[i].xp = obj.entities[j].xp+44;
                obj.entities[i].yp = obj.entities[j].yp+44;
                obj.entities[j].tile = 2;
                obj.entities[j].colour = 101;
            }
            obj.entities[i].colour = 0;
            obj.entities[i].invis = false;
            obj.entities[i].dir = 1;

            obj.entities[i].ay = -6;
            obj.entities[i].ax = 6;
            obj.entities[i].vy = -6;
            obj.entities[i].vx = 6;
            break;
        case 4043:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 12;
            obj.entities[i].yp -= 15;
            break;
        case 4044:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 12;
            obj.entities[i].yp -= 10;
            break;
        case 4045:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 12;
            obj.entities[i].yp -= 10;
            break;
        case 4046:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 8;
            obj.entities[i].yp -= 8;
            break;
        case 4047:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 6;
            obj.entities[i].yp -= 8;
            break;
        case 4048:
            state++;
            statedelay = 15;
            i = obj.getplayer();
            obj.entities[i].xp += 3;
            break;
        case 4049:
            hascontrol = true;
            advancetext = false;
            state = 0;
            break;

        case 4050:
            //Activating a teleporter (default appear)
            state++;
            statedelay = 15;
            flashlight = 5;
            screenshake = 90;
            music.playef(9);
            break;
        case 4051:
            //Activating a teleporter 2
            state++;
            statedelay = 0;
            flashlight = 5;
            screenshake = 0;
            music.playef(10);
            break;
        case 4052:
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            i = obj.getplayer();
            j = obj.getteleporter();
            if (j != -1)
            {
                obj.entities[i].xp = obj.entities[j].xp+44;
                obj.entities[i].yp = obj.entities[j].yp+44;
                obj.entities[j].tile = 2;
                obj.entities[j].colour = 101;
            }
            obj.entities[i].colour = 0;
            obj.entities[i].invis = false;
            obj.entities[i].dir = 1;

            obj.entities[i].ay = -6;
            obj.entities[i].ax = 6;
            obj.entities[i].vy = -6;
            obj.entities[i].vx = 6;
            break;
        case 4053:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 4;
            obj.entities[i].yp -= 15;
            break;
        case 4054:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 4;
            obj.entities[i].yp -= 10;
            break;
        case 4055:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 4;
            obj.entities[i].yp -= 10;
            break;
        case 4056:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 4;
            obj.entities[i].yp -= 8;
            break;
        case 4057:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 2;
            obj.entities[i].yp -= 8;
            break;
        case 4058:
            state++;
            statedelay = 15;
            i = obj.getplayer();
            obj.entities[i].xp += 1;
            break;
        case 4059:
            hascontrol = true;
            advancetext = false;
            state = 0;
            break;

        case 4060:
            //Activating a teleporter (default appear)
            state++;
            statedelay = 15;
            flashlight = 5;
            screenshake = 90;
            music.playef(9);
            break;
        case 4061:
            //Activating a teleporter 2
            state++;
            statedelay = 0;
            flashlight = 5;
            screenshake = 0;
            music.playef(10);
            break;
        case 4062:
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            i = obj.getplayer();
            j = obj.getteleporter();
            if (j != -1)
            {
                obj.entities[i].xp = obj.entities[j].xp+44;
                obj.entities[i].yp = obj.entities[j].yp+44;
                obj.entities[j].tile = 2;
                obj.entities[j].colour = 101;
            }
            obj.entities[i].colour = 0;
            obj.entities[i].invis = false;
            obj.entities[i].dir = 0;

            obj.entities[i].ay = -6;
            obj.entities[i].ax = -6;
            obj.entities[i].vy = -6;
            obj.entities[i].vx = -6;
            break;
        case 4063:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp -= 28;
            obj.entities[i].yp -= 8;
            break;
        case 4064:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp -= 28;
            obj.entities[i].yp -= 8;
            break;
        case 4065:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp -= 25;
            break;
        case 4066:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp -= 25;
            break;
        case 4067:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp -= 20;
            break;
        case 4068:
            state++;
            statedelay = 15;
            i = obj.getplayer();
            obj.entities[i].xp -= 16;
            break;
        case 4069:
            hascontrol = true;
            advancetext = false;
            state = 0;
            break;


        case 4070:
            //Activating a teleporter (special for final script, player has colour changed to match rescued crewmate)
            state++;
            statedelay = 15;
            flashlight = 5;
            screenshake = 90;
            music.playef(9);
            break;
        case 4071:
            //Activating a teleporter 2
            state++;
            statedelay = 0;
            flashlight = 5;
            screenshake = 0;
            music.playef(10);
            break;
        case 4072:
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            i = obj.getplayer();
            j = obj.getteleporter();
            if (j != -1)
            {
                obj.entities[i].xp = obj.entities[j].xp+44;
                obj.entities[i].yp = obj.entities[j].yp+44;
                obj.entities[j].tile = 2;
                obj.entities[j].colour = 101;
            }
            obj.entities[i].invis = false;
            obj.entities[i].dir = 1;
            obj.entities[i].colour = obj.crewcolour(lastsaved);

            obj.entities[i].ay = -6;
            obj.entities[i].ax = 6;
            obj.entities[i].vy = -6;
            obj.entities[i].vx = 6;
            break;
        case 4073:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 10;
            break;
        case 4074:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 10;
            break;
        case 4075:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 8;
            break;
        case 4076:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 6;
            break;
        case 4077:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 3;
            break;
        case 4078:
            state++;
            statedelay = 15;
            i = obj.getplayer();
            obj.entities[i].xp += 1;
            break;
        case 4079:
            state = 0;
            startscript = true;
            newscript = "finallevel_teleporter";
            break;

        case 4080:
            //Activating a teleporter (default appear)
            state++;
            statedelay = 15;
            flashlight = 5;
            screenshake = 90;
            music.playef(9);
            break;
        case 4081:
            //Activating a teleporter 2
            state++;
            statedelay = 0;
            flashlight = 5;
            screenshake = 0;
            music.playef(10);
            break;
        case 4082:
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            i = obj.getplayer();
            j = obj.getteleporter();
            if (j != -1)
            {
                obj.entities[i].xp = obj.entities[j].xp+44;
                obj.entities[i].yp = obj.entities[j].yp+44;
                obj.entities[j].tile = 2;
                obj.entities[j].colour = 101;
            }
            obj.entities[i].colour = 0;
            obj.entities[i].invis = false;
            obj.entities[i].dir = 1;

            obj.entities[i].ay = -6;
            obj.entities[i].ax = 6;
            obj.entities[i].vy = -6;
            obj.entities[i].vx = 6;
            break;
        case 4083:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 10;
            break;
        case 4084:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 10;
            break;
        case 4085:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 8;
            break;
        case 4086:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 6;
            break;
        case 4087:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 3;
            break;
        case 4088:
            state++;
            statedelay = 15;
            i = obj.getplayer();
            obj.entities[i].xp += 1;
            break;
        case 4089:
            startscript = true;
            newscript = "gamecomplete_ending";
            state = 0;
            break;

        case 4090:
            //Activating a teleporter (default appear)
            state++;
            statedelay = 15;
            flashlight = 5;
            screenshake = 90;
            music.playef(9);
            break;
        case 4091:
            //Activating a teleporter 2
            state++;
            statedelay = 0;
            flashlight = 5;
            screenshake = 0;
            music.playef(10);
            break;
        case 4092:
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            i = obj.getplayer();
            j = obj.getteleporter();
            if (j != -1)
            {
                obj.entities[i].xp = obj.entities[j].xp+44;
                obj.entities[i].yp = obj.entities[j].yp+44;
                obj.entities[j].tile = 2;
                obj.entities[j].colour = 101;
            }
            obj.entities[i].colour = 0;
            obj.entities[i].invis = false;
            obj.entities[i].dir = 1;

            obj.entities[i].ay = -6;
            obj.entities[i].ax = 6;
            obj.entities[i].vy = -6;
            obj.entities[i].vx = 6;
            break;
        case 4093:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 10;
            break;
        case 4094:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 10;
            break;
        case 4095:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 8;
            break;
        case 4096:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 6;
            break;
        case 4097:
            state++;
            i = obj.getplayer();
            obj.entities[i].xp += 3;
            break;
        case 4098:
            state++;
            statedelay = 15;
            i = obj.getplayer();
            obj.entities[i].xp += 1;
            break;
        case 4099:
            if (nocutscenes)
            {
                startscript = true;
                newscript = "levelonecompleteskip";
            }
            else
            {
                startscript = true;
                newscript = "levelonecomplete_ending";
            }
            state = 0;
            break;
        }
    }
}

void Game::gethardestroom()
{
    if (currentroomdeaths > hardestroomdeaths)
    {
        hardestroomdeaths = currentroomdeaths;
        hardestroom = map.roomname;
        if (map.roomname == "glitch")
        {
            if (roomx == 42 && roomy == 51)
            {
                hardestroom = "Rear Vindow";
            }
            else if (roomx == 48 && roomy == 51)
            {
                hardestroom = "On the Vaterfront";
            }
            else if (roomx == 49 && roomy == 51)
            {
                hardestroom = "The Untouchavles";
            }
        }
        else if (map.roomname == "change")
        {
            if (roomx == 45 && roomy == 51) hardestroom =map.specialnames[3];
            if (roomx == 46 && roomy == 51) hardestroom =map.specialnames[4];
            if (roomx == 47 && roomy == 51) hardestroom =map.specialnames[5];
            if (roomx == 50 && roomy == 53) hardestroom =map.specialnames[6];
            if (roomx == 50 && roomy == 54) hardestroom = map.specialnames[7];
        }
        else if (map.roomname == "")
        {
            hardestroom = "Dimension VVVVVV";
        }
    }
}

void Game::deletestats()
{
    if (!FILESYSTEM_delete("saves/unlock.vvv"))
    {
        puts("Error deleting saves/unlock.vvv");
    }
    else
    {
        for (int i = 0; i < 25; i++)
        {
            unlock[i] = false;
            unlocknotify[i] = false;
        }
        for (int i = 0; i < 6; i++)
        {
            besttimes[i] = -1;
            besttrinkets[i] = -1;
            bestlives[i] = -1;
            bestrank[i] = -1;
        }
        graphics.setflipmode = false;
        stat_trinkets = 0;
    }
}

void Game::unlocknum( int t )
{
    if (map.custommode)
    {
        //Don't let custom levels unlock things!
        return;
    }

    unlock[t] = true;
    savestats();
}

void Game::loadstats()
{
    TiXmlDocument doc;
    if (!FILESYSTEM_loadTiXmlDocument("saves/unlock.vvv", &doc))
    {
        savestats();
        printf("No Stats found. Assuming a new player\n");
    }

    TiXmlHandle hDoc(&doc);
    TiXmlElement* pElem;
    TiXmlHandle hRoot(0);


    {
        pElem=hDoc.FirstChildElement().Element();
        // should always have a valid root but handle gracefully if it does
        if (!pElem)
        {

        }
        ;

        // save this for later
        hRoot=TiXmlHandle(pElem);
    }

    // WINDOW DIMS, ADDED AT PATCH 22
    int width = 320;
    int height = 240;

    for( pElem = hRoot.FirstChild( "Data" ).FirstChild().Element(); pElem; pElem=pElem->NextSiblingElement())
    {
        std::string pKey(pElem->Value());
        const char* pText = pElem->GetText() ;

        if (pKey == "unlock")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                unlock.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    unlock.push_back(atoi(values[i].c_str()));
                }
            }
        }

        if (pKey == "unlocknotify")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                unlocknotify.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    unlocknotify.push_back(atoi(values[i].c_str()));
                }
            }
        }

        if (pKey == "besttimes")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                besttimes.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    besttimes.push_back(atoi(values[i].c_str()));
                }
            }
        }

        if (pKey == "besttrinkets")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                besttrinkets.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    besttrinkets.push_back(atoi(values[i].c_str()));
                }
            }
        }


        if (pKey == "bestlives")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                bestlives.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    bestlives.push_back(atoi(values[i].c_str()));
                }
            }
        }


        if (pKey == "bestrank")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                bestrank.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    bestrank.push_back(atoi(values[i].c_str()));
                }
            }
        }



        if (pKey == "bestgamedeaths")
        {
            bestgamedeaths = atoi(pText);
        }

        if (pKey == "stat_trinkets")
        {
            stat_trinkets = atoi(pText);
        }

        if (pKey == "fullscreen")
        {
            fullscreen = atoi(pText);
        }

        if (pKey == "stretch")
        {
            stretchMode = atoi(pText);
        }

        if (pKey == "useLinearFilter")
        {
            useLinearFilter = atoi(pText);
        }

        if (pKey == "window_width")
        {
            width = atoi(pText);
        }
        if (pKey == "window_height")
        {
            height = atoi(pText);
        }


        if (pKey == "noflashingmode")
        {
            noflashingmode = atoi(pText);
        }

        if (pKey == "colourblindmode")
        {
            colourblindmode = atoi(pText);
        }

        if (pKey == "setflipmode")
        {
            graphics.setflipmode = atoi(pText);
        }

        if (pKey == "invincibility")
        {
            map.invincibility = atoi(pText);
        }

        if (pKey == "slowdown")
        {
            slowdown = atoi(pText);
            switch(slowdown)
            {
            case 30:
                gameframerate=34;
                break;
            case 24:
                gameframerate=41;
                break;
            case 18:
                gameframerate=55;
                break;
            case 12:
                gameframerate=83;
                break;
            default:
                gameframerate=34;
                break;
            }

        }

        if (pKey == "swnbestrank")
        {
            swnbestrank = atoi(pText);
        }

        if (pKey == "swnrecord")
        {
            swnrecord = atoi(pText);
        }

        if (pKey == "advanced_mode")
        {
            advanced_mode = atoi(pText);
        }

        if (pKey == "advanced_smoothing")
        {
            fullScreenEffect_badSignal = atoi(pText);
            graphics.screenbuffer->badSignalEffect = fullScreenEffect_badSignal;
        }

        if (pKey == "usingmmmmmm")
        {
            if(atoi(pText)>0){
                usingmmmmmm = 1;
            }else{
                usingmmmmmm = 0;
            }
        }

        if (pKey == "skipfakeload")
        {
            skipfakeload = atoi(pText);
        }

        if (pKey == "notextoutline")
        {
            graphics.notextoutline = atoi(pText);
        }

        if (pKey == "translucentroomname")
        {
            graphics.translucentroomname = atoi(pText);
        }

        if (pKey == "showmousecursor")
        {
            graphics.showmousecursor = atoi(pText);
        }

        if (pKey == "flipButton")
        {
            SDL_GameControllerButton newButton;
            if (GetButtonFromString(pText, &newButton))
            {
                controllerButton_flip.push_back(newButton);
            }
        }

        if (pKey == "enterButton")
        {
            SDL_GameControllerButton newButton;
            if (GetButtonFromString(pText, &newButton))
            {
                controllerButton_map.push_back(newButton);
            }
        }

        if (pKey == "escButton")
        {
            SDL_GameControllerButton newButton;
            if (GetButtonFromString(pText, &newButton))
            {
                controllerButton_esc.push_back(newButton);
            }
        }

        if (pKey == "controllerSensitivity")
        {
            controllerSensitivity = atoi(pText);
        }

    }

    if(fullscreen)
    {
        graphics.screenbuffer->toggleFullScreen();
    }
    for (int i = 0; i < stretchMode; i += 1)
    {
        graphics.screenbuffer->toggleStretchMode();
    }
    if (useLinearFilter)
    {
        graphics.screenbuffer->toggleLinearFilter();
    }
    graphics.screenbuffer->ResizeScreen(width, height);

    if (graphics.showmousecursor == true)
    {
        SDL_ShowCursor(SDL_ENABLE);
    }
    else {
        SDL_ShowCursor(SDL_DISABLE);
    }

    if (controllerButton_flip.size() < 1)
    {
        controllerButton_flip.push_back(SDL_CONTROLLER_BUTTON_A);
    }
    if (controllerButton_map.size() < 1)
    {
        controllerButton_map.push_back(SDL_CONTROLLER_BUTTON_Y);
    }
    if (controllerButton_esc.size() < 1)
    {
        controllerButton_esc.push_back(SDL_CONTROLLER_BUTTON_B);
    }
}

void Game::savestats()
{
    TiXmlDocument doc;
    TiXmlElement* msg;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );

    TiXmlElement * root = new TiXmlElement( "Save" );
    doc.LinkEndChild( root );

    TiXmlComment * comment = new TiXmlComment();
    comment->SetValue(" Save file " );
    root->LinkEndChild( comment );

    TiXmlElement * dataNode = new TiXmlElement( "Data" );
    root->LinkEndChild( dataNode );

    std::string s_unlock;
    for(size_t i = 0; i < unlock.size(); i++ )
    {
        s_unlock += help.String(unlock[i]) + ",";
    }
    msg = new TiXmlElement( "unlock" );
    msg->LinkEndChild( new TiXmlText( s_unlock.c_str() ));
    dataNode->LinkEndChild( msg );

    std::string s_unlocknotify;
    for(size_t i = 0; i < unlocknotify.size(); i++ )
    {
        s_unlocknotify += help.String(unlocknotify[i]) + ",";
    }
    msg = new TiXmlElement( "unlocknotify" );
    msg->LinkEndChild( new TiXmlText( s_unlocknotify.c_str() ));
    dataNode->LinkEndChild( msg );

    std::string s_besttimes;
    for(size_t i = 0; i < besttrinkets.size(); i++ )
    {
        s_besttimes += help.String(besttimes[i]) + ",";
    }
    msg = new TiXmlElement( "besttimes" );
    msg->LinkEndChild( new TiXmlText( s_besttimes.c_str() ));
    dataNode->LinkEndChild( msg );

    std::string s_besttrinkets;
    for(size_t i = 0; i < besttrinkets.size(); i++ )
    {
        s_besttrinkets += help.String(besttrinkets[i]) + ",";
    }
    msg = new TiXmlElement( "besttrinkets" );
    msg->LinkEndChild( new TiXmlText( s_besttrinkets.c_str() ));
    dataNode->LinkEndChild( msg );

    std::string s_bestlives;
    for(size_t i = 0; i < bestlives.size(); i++ )
    {
        s_bestlives += help.String(bestlives[i]) + ",";
    }
    msg = new TiXmlElement( "bestlives" );
    msg->LinkEndChild( new TiXmlText( s_bestlives.c_str() ));
    dataNode->LinkEndChild( msg );

    std::string s_bestrank;
    for(size_t i = 0; i < bestrank.size(); i++ )
    {
        s_bestrank += help.String(bestrank[i]) + ",";
    }
    msg = new TiXmlElement( "bestrank" );
    msg->LinkEndChild( new TiXmlText( s_bestrank.c_str() ));
    dataNode->LinkEndChild( msg );

    msg = new TiXmlElement( "bestgamedeaths" );
    msg->LinkEndChild( new TiXmlText( help.String(bestgamedeaths).c_str() ));
    dataNode->LinkEndChild( msg );
    msg = new TiXmlElement( "stat_trinkets" );
    msg->LinkEndChild( new TiXmlText( help.String(stat_trinkets).c_str()));
    dataNode->LinkEndChild( msg );

    msg = new TiXmlElement( "fullscreen" );
    msg->LinkEndChild( new TiXmlText( help.String(fullscreen).c_str()));
    dataNode->LinkEndChild( msg );

    msg = new TiXmlElement( "stretch" );
    msg->LinkEndChild( new TiXmlText( help.String(stretchMode).c_str()));
    dataNode->LinkEndChild( msg );

    msg = new TiXmlElement( "useLinearFilter" );
    msg->LinkEndChild( new TiXmlText( help.String(useLinearFilter).c_str()));
    dataNode->LinkEndChild( msg );

    int width, height;
    graphics.screenbuffer->GetWindowSize(&width, &height);
    msg = new TiXmlElement( "window_width" );
    msg->LinkEndChild( new TiXmlText( help.String(width).c_str()));
    dataNode->LinkEndChild( msg );
    msg = new TiXmlElement( "window_height" );
    msg->LinkEndChild( new TiXmlText( help.String(height).c_str()));
    dataNode->LinkEndChild( msg );

    msg = new TiXmlElement( "noflashingmode" );
    msg->LinkEndChild( new TiXmlText( help.String(noflashingmode).c_str()));
    dataNode->LinkEndChild( msg );

    msg = new TiXmlElement( "colourblindmode" );
    msg->LinkEndChild( new TiXmlText( help.String(colourblindmode).c_str()));
    dataNode->LinkEndChild( msg );

    msg = new TiXmlElement( "setflipmode" );
    msg->LinkEndChild( new TiXmlText( help.String(graphics.setflipmode).c_str()));
    dataNode->LinkEndChild( msg );

    msg = new TiXmlElement( "invincibility" );
    msg->LinkEndChild( new TiXmlText( help.String(map.invincibility).c_str()));
    dataNode->LinkEndChild( msg );

    msg = new TiXmlElement( "slowdown" );
    msg->LinkEndChild( new TiXmlText( help.String(slowdown).c_str()));
    dataNode->LinkEndChild( msg );

    msg = new TiXmlElement( "swnbestrank" );
    msg->LinkEndChild( new TiXmlText( help.String(swnbestrank).c_str()));
    dataNode->LinkEndChild( msg );

    msg = new TiXmlElement( "swnrecord" );
    msg->LinkEndChild( new TiXmlText( help.String(swnrecord).c_str()));
    dataNode->LinkEndChild( msg );


    msg = new TiXmlElement( "advanced_mode" );
    msg->LinkEndChild( new TiXmlText( help.String(advanced_mode).c_str()));
    dataNode->LinkEndChild( msg );

    msg = new TiXmlElement( "advanced_smoothing" );
    msg->LinkEndChild( new TiXmlText( help.String(fullScreenEffect_badSignal).c_str()));
    dataNode->LinkEndChild( msg );


    msg = new TiXmlElement( "usingmmmmmm" );
    msg->LinkEndChild( new TiXmlText( help.String(usingmmmmmm).c_str()));
    dataNode->LinkEndChild( msg );

    msg = new TiXmlElement("skipfakeload");
    msg->LinkEndChild(new TiXmlText(help.String((int) skipfakeload).c_str()));
    dataNode->LinkEndChild(msg);

    msg = new TiXmlElement("notextoutline");
    msg->LinkEndChild(new TiXmlText(help.String((int) graphics.notextoutline).c_str()));
    dataNode->LinkEndChild(msg);

    msg = new TiXmlElement("translucentroomname");
    msg->LinkEndChild(new TiXmlText(help.String((int) graphics.translucentroomname).c_str()));
    dataNode->LinkEndChild(msg);

    msg = new TiXmlElement("showmousecursor");
    msg->LinkEndChild(new TiXmlText(help.String((int)graphics.showmousecursor).c_str()));
    dataNode->LinkEndChild(msg);

    for (size_t i = 0; i < controllerButton_flip.size(); i += 1)
    {
        msg = new TiXmlElement("flipButton");
        msg->LinkEndChild(new TiXmlText(help.String((int) controllerButton_flip[i]).c_str()));
        dataNode->LinkEndChild(msg);
    }
    for (size_t i = 0; i < controllerButton_map.size(); i += 1)
    {
        msg = new TiXmlElement("enterButton");
        msg->LinkEndChild(new TiXmlText(help.String((int) controllerButton_map[i]).c_str()));
        dataNode->LinkEndChild(msg);
    }
    for (size_t i = 0; i < controllerButton_esc.size(); i += 1)
    {
        msg = new TiXmlElement("escButton");
        msg->LinkEndChild(new TiXmlText(help.String((int) controllerButton_esc[i]).c_str()));
        dataNode->LinkEndChild(msg);
    }

    msg = new TiXmlElement( "controllerSensitivity" );
    msg->LinkEndChild( new TiXmlText( help.String(controllerSensitivity).c_str()));
    dataNode->LinkEndChild( msg );

    FILESYSTEM_saveTiXmlDocument("saves/unlock.vvv", &doc);
}

void Game::customstart()
{
    jumpheld = true;

    savex = edsavex;
    savey = edsavey;
    saverx = edsaverx;
    savery = edsavery;

    savegc = edsavegc;
    savedir = edsavedir; //Worldmap Start
    //savex = 6 * 8; savey = 15 * 8; saverx = 46; savery = 54; savegc = 0; savedir = 1; //Final Level Current
    savepoint = 0;
    gravitycontrol = savegc;

    //state = 2; deathseq = -1; lifeseq = 10; //Not dead, in game initilisation state
    state = 0;
    deathseq = -1;
    lifeseq = 0;

    //let's teleport in!
    //state = 2500;
    //if (!nocutscenes) music.play(5);
}

void Game::start()
{
    jumpheld = true;

    savex = 232;
    savey = 113;
    saverx = 104;
    savery = 110;
    savegc = 0;
    savedir = 1; //Worldmap Start
    //savex = 6 * 8; savey = 15 * 8; saverx = 46; savery = 54; savegc = 0; savedir = 1; //Final Level Current
    savepoint = 0;
    gravitycontrol = savegc;

    //state = 2; deathseq = -1; lifeseq = 10; //Not dead, in game initilisation state
    state = 0;
    deathseq = -1;
    lifeseq = 0;

    //let's teleport in!
    //state = 2500;
    if (!nocutscenes) music.play(5);
}

void Game::deathsequence()
{
    int i;
    if (supercrewmate && scmhurt)
    {
        i = obj.getscm();
    }
    else
    {
        i = obj.getplayer();
    }
    obj.entities[i].colour = 1;

    obj.entities[i].invis = false;
    if (deathseq == 30)
    {
        if (nodeathmode)
        {
            music.fadeout();
            gameoverdelay = 60;
        }
        deathcounts++;
        music.playef(2);
        obj.entities[i].invis = true;
        if (map.finalmode)
        {
            map.roomdeathsfinal[roomx - 41 + (20 * (roomy - 48))]++;
            currentroomdeaths = map.roomdeathsfinal[roomx - 41 + (20 * (roomy - 48))];
        }
        else
        {
            map.roomdeaths[roomx - 100 + (20*(roomy - 100))]++;
            currentroomdeaths = map.roomdeaths[roomx - 100 + (20 * (roomy - 100))];
        }
    }
    if (deathseq == 25) obj.entities[i].invis = true;
    if (deathseq == 20) obj.entities[i].invis = true;
    if (deathseq == 16) obj.entities[i].invis = true;
    if (deathseq == 14) obj.entities[i].invis = true;
    if (deathseq == 12) obj.entities[i].invis = true;
    if (deathseq < 10) obj.entities[i].invis = true;
    if (!nodeathmode)
    {
        if (deathseq <= 1) obj.entities[i].invis = false;
    }
    else
    {
        gameoverdelay--;
    }
}

void Game::startspecial( int t )
{
    jumpheld = true;

    switch(t)
    {
    case 0: //Secret Lab
        savex = 104;
        savey = 169;
        saverx = 118;
        savery = 106;
        savegc = 0;
        savedir = 1;
        break;
    case 1: //Intermission 1 (any)
        savex = 80;
        savey = 57;
        saverx = 41;
        savery = 56;
        savegc = 0;
        savedir = 0;
        break;
    default:
        savex = 232;
        savey = 113;
        saverx = 104;
        savery = 110;
        savegc = 0;
        savedir = 1; //Worldmap Start
        break;
    }

    savepoint = 0;
    gravitycontrol = savegc;
    state = 0;
    deathseq = -1;
    lifeseq = 0;
}

void Game::starttrial( int t )
{
    jumpheld = true;

    switch(t)
    {
    case 0: //Space Station 1
        savex = 200;
        savey = 161;
        saverx = 113;
        savery = 105;
        savegc = 0;
        savedir = 1;
        break;
    case 1: //Lab
        savex = 191;
        savey = 33;
        saverx = 102;
        savery = 116;
        savegc = 0;
        savedir = 1;
        break;
    case 2: //Tower
        savex = 84;
        savey = 193, saverx = 108;
        savery = 109;
        savegc = 0;
        savedir = 1;
        break;
    case 3: //Space Station 2
        savex = 148;
        savey = 38;
        saverx = 112;
        savery = 114;
        savegc = 1;
        savedir = 0;
        break;
    case 4: //Warp
        savex = 52;
        savey = 73;
        saverx = 114;
        savery = 101;
        savegc = 0;
        savedir = 1;
        break;
    case 5: //Final
        savex = 101;
        savey = 113;
        saverx = 46;
        savery = 54;
        savegc = 0;
        savedir = 1;
        break;
    default:
        savex = 232;
        savey = 113;
        saverx = 104;
        savery = 110;
        savegc = 0;
        savedir = 1; //Worldmap Start
        break;
    }

    savepoint = 0;
    gravitycontrol = savegc;

    //state = 2; deathseq = -1; lifeseq = 10; //Not dead, in game initilisation state
    state = 0;
    deathseq = -1;
    lifeseq = 0;
}

void Game::loadquick()
{
    TiXmlDocument doc;
    if (!FILESYSTEM_loadTiXmlDocument("saves/qsave.vvv", &doc)) return;

    TiXmlHandle hDoc(&doc);
    TiXmlElement* pElem;
    TiXmlHandle hRoot(0);


    {
        pElem=hDoc.FirstChildElement().Element();
        // should always have a valid root but handle gracefully if it does
        if (!pElem)
        {
            printf("Save Not Found\n");
        }

        // save this for later
        hRoot=TiXmlHandle(pElem);
    }

    for( pElem = hRoot.FirstChild( "Data" ).FirstChild().Element(); pElem; pElem=pElem->NextSiblingElement())
    {
        std::string pKey(pElem->Value());
        const char* pText = pElem->GetText() ;
        if(pText == NULL)
        {
            pText = "";
        }

        if (pKey == "worldmap")
        {
            std::string TextString = (pText);
            if(TextString.length()>1)
            {
                std::vector<std::string> values = split(TextString,',');
                map.explored.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    map.explored.push_back(atoi(values[i].c_str()));
                }
            }
        }

        if (pKey == "flags")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                obj.flags.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    obj.flags.push_back((bool) atoi(values[i].c_str()));
                }
            }
        }

        if (pKey == "crewstats")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                crewstats.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    crewstats.push_back(atoi(values[i].c_str()));
                }
            }
        }

        if (pKey == "collect")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                obj.collect.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    obj.collect.push_back((bool) atoi(values[i].c_str()));
                }
            }
        }

        if (pKey == "finalmode")
        {
            map.finalmode = atoi(pText);
        }
        if (pKey == "finalstretch")
        {
            map.finalstretch = atoi(pText);
        }

        if (pKey == "finalx")
        {
            map.finalx = atoi(pText);
        }
        else if (pKey == "finaly")
        {
            map.finaly = atoi(pText);
        }
        else if (pKey == "savex")
        {
            savex = atoi(pText);
        }
        else if (pKey == "savey")
        {
            savey = atoi(pText);
        }
        else if (pKey == "saverx")
        {
            saverx = atoi(pText);
        }
        else if (pKey == "savery")
        {
            savery = atoi(pText);
        }
        else if (pKey == "savegc")
        {
            savegc = atoi(pText);
        }
        else if (pKey == "savedir")
        {
            savedir= atoi(pText);
        }
        else if (pKey == "savepoint")
        {
            savepoint = atoi(pText);
        }
        else if (pKey == "companion")
        {
            companion = atoi(pText);
        }
        else if (pKey == "lastsaved")
        {
            lastsaved = atoi(pText);
        }
        else if (pKey == "teleportscript")
        {
            teleportscript = pText;
        }
        else if (pKey == "supercrewmate")
        {
            supercrewmate = atoi(pText);
        }
        else if (pKey == "scmprogress")
        {
            scmprogress = atoi(pText);
        }
        else if (pKey == "scmmoveme")
        {
            scmmoveme = atoi(pText);
        }
        else if (pKey == "frames")
        {
            frames = atoi(pText);
            frames = 0;
        }
        else if (pKey == "seconds")
        {
            seconds = atoi(pText);
        }
        else if (pKey == "minutes")
        {
            minutes = atoi(pText);
        }
        else if (pKey == "hours")
        {
            hours = atoi(pText);
        }
        else if (pKey == "deathcounts")
        {
            deathcounts = atoi(pText);
        }
        else if (pKey == "totalflips")
        {
            totalflips = atoi(pText);
        }
        else if (pKey == "hardestroom")
        {
            hardestroom = atoi(pText);
        }
        else if (pKey == "hardestroomdeaths")
        {
            hardestroomdeaths = atoi(pText);
        }
        else if (pKey == "currentsong")
        {
            music.play(atoi(pText));
        }

    }

    if (map.finalmode)
    {
        map.final_colormode = false;
        map.final_mapcol = 0;
        map.final_colorframe = 0;
    }
    if (map.finalstretch)
    {
        map.finalstretch = true;
        map.final_colormode = true;
        map.final_mapcol = 0;
        map.final_colorframe = 1;
    }

    map.showteleporters = true;
    if(obj.flags[12]) map.showtargets = true;
    if (obj.flags[42]) map.showtrinkets = true;


}

void Game::customloadquick(std::string savfile)
{
    if (cliplaytest) {
        savex = playx;
        savey = playy;
        saverx = playrx;
        savery = playry;
        savegc = playgc;
        return;
    }

    std::string levelfile = savfile.substr(7);
    TiXmlDocument doc;
    if (!FILESYSTEM_loadTiXmlDocument(("saves/"+levelfile+".vvv").c_str(), &doc)) return;

    TiXmlHandle hDoc(&doc);
    TiXmlElement* pElem;
    TiXmlHandle hRoot(0);


    {
        pElem=hDoc.FirstChildElement().Element();
        // should always have a valid root but handle gracefully if it does
        if (!pElem)
        {
            printf("Save Not Found\n");
        }

        // save this for later
        hRoot=TiXmlHandle(pElem);
    }

    for( pElem = hRoot.FirstChild( "Data" ).FirstChild().Element(); pElem; pElem=pElem->NextSiblingElement())
    {
        std::string pKey(pElem->Value());
        const char* pText = pElem->GetText() ;
        if(pText == NULL)
        {
            pText = "";
        }

        if (pKey == "worldmap")
        {
            std::string TextString = (pText);
            if(TextString.length()>1)
            {
                std::vector<std::string> values = split(TextString,',');
                map.explored.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    map.explored.push_back(atoi(values[i].c_str()));
                }
            }
        }

        if (pKey == "flags")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                obj.flags.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    obj.flags.push_back((bool) atoi(values[i].c_str()));
                }
            }
        }

        if (pKey == "moods")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                for(size_t i = 0; i < 6; i++)
                {
                    obj.customcrewmoods[i]=atoi(values[i].c_str());
                }
            }
        }

        if (pKey == "crewstats")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                crewstats.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    crewstats.push_back(atoi(values[i].c_str()));
                }
            }
        }

        if (pKey == "collect")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                obj.collect.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    obj.collect.push_back((bool) atoi(values[i].c_str()));
                }
            }
        }

        if (pKey == "customcollect")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                obj.customcollect.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    obj.customcollect.push_back((bool) atoi(values[i].c_str()));
                }
            }
        }

        if (pKey == "finalmode")
        {
            map.finalmode = atoi(pText);
        }
        if (pKey == "finalstretch")
        {
            map.finalstretch = atoi(pText);
        }

        if (map.finalmode)
        {
            map.final_colormode = false;
            map.final_mapcol = 0;
            map.final_colorframe = 0;
        }
        if (map.finalstretch)
        {
            map.finalstretch = true;
            map.final_colormode = true;
            map.final_mapcol = 0;
            map.final_colorframe = 1;
        }


        if (pKey == "finalx")
        {
            map.finalx = atoi(pText);
        }
        else if (pKey == "finaly")
        {
            map.finaly = atoi(pText);
        }
        else if (pKey == "savex")
        {
            savex = atoi(pText);
        }
        else if (pKey == "savey")
        {
            savey = atoi(pText);
        }
        else if (pKey == "saverx")
        {
            saverx = atoi(pText);
        }
        else if (pKey == "savery")
        {
            savery = atoi(pText);
        }
        else if (pKey == "savegc")
        {
            savegc = atoi(pText);
        }
        else if (pKey == "savedir")
        {
            savedir= atoi(pText);
        }
        else if (pKey == "savepoint")
        {
            savepoint = atoi(pText);
        }
        else if (pKey == "companion")
        {
            companion = atoi(pText);
        }
        else if (pKey == "lastsaved")
        {
            lastsaved = atoi(pText);
        }
        else if (pKey == "teleportscript")
        {
            teleportscript = pText;
        }
        else if (pKey == "supercrewmate")
        {
            supercrewmate = atoi(pText);
        }
        else if (pKey == "scmprogress")
        {
            scmprogress = atoi(pText);
        }
        else if (pKey == "scmmoveme")
        {
            scmmoveme = atoi(pText);
        }
        else if (pKey == "frames")
        {
            frames = atoi(pText);
            frames = 0;
        }
        else if (pKey == "seconds")
        {
            seconds = atoi(pText);
        }
        else if (pKey == "minutes")
        {
            minutes = atoi(pText);
        }
        else if (pKey == "hours")
        {
            hours = atoi(pText);
        }
        else if (pKey == "deathcounts")
        {
            deathcounts = atoi(pText);
        }
        else if (pKey == "totalflips")
        {
            totalflips = atoi(pText);
        }
        else if (pKey == "hardestroom")
        {
            hardestroom = atoi(pText);
        }
        else if (pKey == "hardestroomdeaths")
        {
            hardestroomdeaths = atoi(pText);
        }
        else if (pKey == "currentsong")
        {
            music.play(atoi(pText));
        }
        else if (pKey == "showminimap")
        {
            map.customshowmm = atoi(pText);
        }

    }

    map.showteleporters = true;
    if(obj.flags[12]) map.showtargets = true;
    if (obj.flags[42]) map.showtrinkets = true;

}

void Game::loadsummary()
{
    TiXmlDocument docTele;
    if (!FILESYSTEM_loadTiXmlDocument("saves/tsave.vvv", &docTele))
    {
        telesummary = "";
    }
    else
    {
        TiXmlHandle hDoc(&docTele);
        TiXmlElement* pElem;
        TiXmlHandle hRoot(0);


        {
            pElem=hDoc.FirstChildElement().Element();
            // should always have a valid root but handle gracefully if it does
            if (!pElem)
            {
                printf("Save Not Found\n");
            }

            // save this for later
            hRoot=TiXmlHandle(pElem);
        }
        int l_minute, l_second, l_hours;
        l_minute = l_second= l_hours = 0;
        int l_saveX = 0;
        int l_saveY = 0;
        for( pElem = hRoot.FirstChild( "Data" ).FirstChild().Element(); pElem; pElem=pElem->NextSiblingElement())
        {
            std::string pKey(pElem->Value());
            const char* pText = pElem->GetText() ;

            if (pKey == "summary")
            {
                telesummary = pText;
            }

            else if (pKey == "seconds")
            {
                l_second = atoi(pText);
            }
            else if (pKey == "minutes")
            {
                l_minute = atoi(pText);
            }
            else if (pKey == "hours")
            {
                l_hours = atoi(pText);
            }
            else if (pKey == "savery")
            {
                l_saveY = atoi(pText);
            }
            else if (pKey == "saverx")
            {
                l_saveX = atoi(pText);
            }
            else if (pKey == "trinkets")
            {
                tele_trinkets = atoi(pText);
            }
            else if (pKey == "finalmode")
            {
                map.finalmode = atoi(pText);
            }
            else if (pKey == "finalstretch")
            {
                map.finalstretch = atoi(pText);
            }

            if (pKey == "crewstats")
            {
                std::string TextString = (pText);
                if(TextString.length())
                {
                    std::vector<std::string> values = split(TextString,',');
                    tele_crewstats.clear();
                    for(size_t i = 0; i < values.size(); i++)
                    {
                        tele_crewstats.push_back(atoi(values[i].c_str()));
                    }
                }
            }

        }
        tele_gametime = giventimestring(l_hours,l_minute, l_second);
        tele_currentarea = map.currentarea(map.area(l_saveX, l_saveY));
    }

    TiXmlDocument doc;
    if (!FILESYSTEM_loadTiXmlDocument("saves/qsave.vvv", &doc))
    {
        quicksummary = "";
    }
    else
    {
        TiXmlHandle hDoc(&doc);
        TiXmlElement* pElem;
        TiXmlHandle hRoot(0);


        {
            pElem=hDoc.FirstChildElement().Element();
            // should always have a valid root but handle gracefully if it does
            if (!pElem)
            {
                printf("Save Not Found\n");
            }

            // save this for later
            hRoot=TiXmlHandle(pElem);
        }
        int l_minute, l_second, l_hours;
        l_minute = l_second= l_hours = 0;
        int l_saveX = 0;
        int l_saveY = 0;
        for( pElem = hRoot.FirstChild( "Data" ).FirstChild().Element(); pElem; pElem=pElem->NextSiblingElement())
        {
            std::string pKey(pElem->Value());
            const char* pText = pElem->GetText() ;

            if (pKey == "summary")
            {
                quicksummary = pText;
            }

            else if (pKey == "seconds")
            {
                l_second = atoi(pText);
            }
            else if (pKey == "minutes")
            {
                l_minute = atoi(pText);
            }
            else if (pKey == "hours")
            {
                l_hours = atoi(pText);
            }
            else if (pKey == "savery")
            {
                l_saveY = atoi(pText);
            }
            else if (pKey == "saverx")
            {
                l_saveX = atoi(pText);
            }
            else if (pKey == "trinkets")
            {
                quick_trinkets = atoi(pText);
            }
            else if (pKey == "finalmode")
            {
                map.finalmode = atoi(pText);
            }
            else if (pKey == "finalstretch")
            {
                map.finalstretch = atoi(pText);
            }

            if (pKey == "crewstats")
            {
                std::string TextString = (pText);
                if(TextString.length())
                {
                    std::vector<std::string> values = split(TextString,',');
                    quick_crewstats.clear();
                    for(size_t i = 0; i < values.size(); i++)
                    {
                        quick_crewstats.push_back(atoi(values[i].c_str()));
                    }
                }
            }

        }

        quick_gametime = giventimestring(l_hours,l_minute, l_second);
        quick_currentarea = map.currentarea(map.area(l_saveX, l_saveY));
    }



}

void Game::initteleportermode()
{
    //Set the teleporter variable to the right position!
    teleport_to_teleporter = 0;

    for (size_t i = 0; i < map.teleporters.size(); i++)
    {
        if (roomx == map.teleporters[i].x + 100 && roomy == map.teleporters[i].y + 100)
        {
            teleport_to_teleporter = i;
        }
    }
}

void Game::savetele()
{
    //TODO make this code a bit cleaner.

    if (map.custommode)
    {
        //Don't trash save data!
        return;
    }

    TiXmlDocument doc;
    TiXmlElement* msg;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );

    TiXmlElement * root = new TiXmlElement( "Save" );
    doc.LinkEndChild( root );

    TiXmlComment * comment = new TiXmlComment();
    comment->SetValue(" Save file " );
    root->LinkEndChild( comment );

    TiXmlElement * msgs = new TiXmlElement( "Data" );
    root->LinkEndChild( msgs );


    //Flags, map and stats

    std::string mapExplored;
    for(size_t i = 0; i < map.explored.size(); i++ )
    {
        mapExplored += help.String(map.explored[i]) + ",";
    }
    msg = new TiXmlElement( "worldmap" );
    msg->LinkEndChild( new TiXmlText( mapExplored.c_str() ));
    msgs->LinkEndChild( msg );

    std::string flags;
    for(size_t i = 0; i < obj.flags.size(); i++ )
    {
        flags += help.String((int) obj.flags[i]) + ",";
    }
    msg = new TiXmlElement( "flags" );
    msg->LinkEndChild( new TiXmlText( flags.c_str() ));
    msgs->LinkEndChild( msg );

    std::string crewstatsString;
    for(size_t i = 0; i < crewstats.size(); i++ )
    {
        crewstatsString += help.String(crewstats[i]) + ",";
    }
    msg = new TiXmlElement( "crewstats" );
    msg->LinkEndChild( new TiXmlText( crewstatsString.c_str() ));
    msgs->LinkEndChild( msg );

    std::string collect;
    for(size_t i = 0; i < obj.collect.size(); i++ )
    {
        collect += help.String((int) obj.collect[i]) + ",";
    }
    msg = new TiXmlElement( "collect" );
    msg->LinkEndChild( new TiXmlText( collect.c_str() ));
    msgs->LinkEndChild( msg );

    //Position

    msg = new TiXmlElement( "finalx" );
    msg->LinkEndChild( new TiXmlText( help.String(map.finalx).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "finaly" );
    msg->LinkEndChild( new TiXmlText( help.String(map.finaly).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savex" );
    msg->LinkEndChild( new TiXmlText( help.String(savex).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savey" );
    msg->LinkEndChild( new TiXmlText( help.String(savey).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "saverx" );
    msg->LinkEndChild( new TiXmlText( help.String(saverx).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savery" );
    msg->LinkEndChild( new TiXmlText( help.String(savery).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savegc" );
    msg->LinkEndChild( new TiXmlText( help.String(savegc).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savedir" );
    msg->LinkEndChild( new TiXmlText( help.String(savedir).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savepoint" );
    msg->LinkEndChild( new TiXmlText( help.String(savepoint).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "trinkets" );
    msg->LinkEndChild( new TiXmlText( help.String(trinkets()).c_str() ));
    msgs->LinkEndChild( msg );


    //Special stats

    if(music.nicefade==1)
    {
        msg = new TiXmlElement( "currentsong" );
        msg->LinkEndChild( new TiXmlText( help.String(music.nicechange).c_str() ));
        msgs->LinkEndChild( msg );
    }
    else
    {
        msg = new TiXmlElement( "currentsong" );
        msg->LinkEndChild( new TiXmlText( help.String(music.currentsong).c_str() ));
        msgs->LinkEndChild( msg );
    }

    msg = new TiXmlElement( "teleportscript" );
    msg->LinkEndChild( new TiXmlText( teleportscript.c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "companion" );
    msg->LinkEndChild( new TiXmlText( help.String(companion).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "lastsaved" );
    msg->LinkEndChild( new TiXmlText( help.String(lastsaved).c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "supercrewmate" );
    msg->LinkEndChild( new TiXmlText( BoolToString(supercrewmate) ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "scmprogress" );
    msg->LinkEndChild( new TiXmlText( help.String(scmprogress).c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "scmmoveme" );
    msg->LinkEndChild( new TiXmlText( BoolToString(scmmoveme) ));
    msgs->LinkEndChild( msg );


    msg = new TiXmlElement( "frames" );
    msg->LinkEndChild( new TiXmlText( help.String(frames).c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "seconds" );
    msg->LinkEndChild( new TiXmlText( help.String(seconds).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "minutes" );
    msg->LinkEndChild( new TiXmlText( help.String(minutes).c_str()) );
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "hours" );
    msg->LinkEndChild( new TiXmlText( help.String(hours).c_str()) );
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "deathcounts" );
    msg->LinkEndChild( new TiXmlText( help.String(deathcounts).c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "totalflips" );
    msg->LinkEndChild( new TiXmlText( help.String(totalflips).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "hardestroom" );
    msg->LinkEndChild( new TiXmlText( hardestroom.c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "hardestroomdeaths" );
    msg->LinkEndChild( new TiXmlText( help.String(hardestroomdeaths).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "finalmode" );
    msg->LinkEndChild( new TiXmlText( BoolToString(map.finalmode)));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "finalstretch" );
    msg->LinkEndChild( new TiXmlText( BoolToString(map.finalstretch) ));
    msgs->LinkEndChild( msg );


    msg = new TiXmlElement( "summary" );
    std::string summary = savearea + ", " + timestring();
    msg->LinkEndChild( new TiXmlText( summary.c_str() ));
    msgs->LinkEndChild( msg );

    telesummary = summary;

    if(FILESYSTEM_saveTiXmlDocument("saves/tsave.vvv", &doc))
    {
        printf("Game saved\n");
    }
    else
    {
        printf("Could Not Save game!\n");
        printf("Failed: %s%s\n", saveFilePath.c_str(), "tsave.vvv");
    }
}


void Game::savequick()
{
    if (map.custommode)
    {
        //Don't trash save data!
        return;
    }

    TiXmlDocument doc;
    TiXmlElement* msg;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );

    TiXmlElement * root = new TiXmlElement( "Save" );
    doc.LinkEndChild( root );

    TiXmlComment * comment = new TiXmlComment();
    comment->SetValue(" Save file " );
    root->LinkEndChild( comment );

    TiXmlElement * msgs = new TiXmlElement( "Data" );
    root->LinkEndChild( msgs );


    //Flags, map and stats

    std::string mapExplored;
    for(size_t i = 0; i < map.explored.size(); i++ )
    {
        mapExplored += help.String(map.explored[i]) + ",";
    }
    msg = new TiXmlElement( "worldmap" );
    msg->LinkEndChild( new TiXmlText( mapExplored.c_str() ));
    msgs->LinkEndChild( msg );

    std::string flags;
    for(size_t i = 0; i < obj.flags.size(); i++ )
    {
        flags += help.String((int) obj.flags[i]) + ",";
    }
    msg = new TiXmlElement( "flags" );
    msg->LinkEndChild( new TiXmlText( flags.c_str() ));
    msgs->LinkEndChild( msg );

    std::string crewstatsString;
    for(size_t i = 0; i < crewstats.size(); i++ )
    {
        crewstatsString += help.String(crewstats[i]) + ",";
    }
    msg = new TiXmlElement( "crewstats" );
    msg->LinkEndChild( new TiXmlText( crewstatsString.c_str() ));
    msgs->LinkEndChild( msg );

    std::string collect;
    for(size_t i = 0; i < obj.collect.size(); i++ )
    {
        collect += help.String((int) obj.collect[i]) + ",";
    }
    msg = new TiXmlElement( "collect" );
    msg->LinkEndChild( new TiXmlText( collect.c_str() ));
    msgs->LinkEndChild( msg );

    //Position

    msg = new TiXmlElement( "finalx" );
    msg->LinkEndChild( new TiXmlText( help.String(map.finalx).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "finaly" );
    msg->LinkEndChild( new TiXmlText( help.String(map.finaly).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savex" );
    msg->LinkEndChild( new TiXmlText( help.String(savex).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savey" );
    msg->LinkEndChild( new TiXmlText( help.String(savey).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "saverx" );
    msg->LinkEndChild( new TiXmlText( help.String(saverx).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savery" );
    msg->LinkEndChild( new TiXmlText( help.String(savery).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savegc" );
    msg->LinkEndChild( new TiXmlText( help.String(savegc).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savedir" );
    msg->LinkEndChild( new TiXmlText( help.String(savedir).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savepoint" );
    msg->LinkEndChild( new TiXmlText( help.String(savepoint).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "trinkets" );
    msg->LinkEndChild( new TiXmlText( help.String(trinkets()).c_str() ));
    msgs->LinkEndChild( msg );


    //Special stats

    if(music.nicefade==1)
    {
        msg = new TiXmlElement( "currentsong" );
        msg->LinkEndChild( new TiXmlText( help.String(music.nicechange).c_str() ));
        msgs->LinkEndChild( msg );
    }
    else
    {
        msg = new TiXmlElement( "currentsong" );
        msg->LinkEndChild( new TiXmlText( help.String(music.currentsong).c_str() ));
        msgs->LinkEndChild( msg );
    }

    msg = new TiXmlElement( "teleportscript" );
    msg->LinkEndChild( new TiXmlText( teleportscript.c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "companion" );
    msg->LinkEndChild( new TiXmlText( help.String(companion).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "lastsaved" );
    msg->LinkEndChild( new TiXmlText( help.String(lastsaved).c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "supercrewmate" );
    msg->LinkEndChild( new TiXmlText( BoolToString(supercrewmate) ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "scmprogress" );
    msg->LinkEndChild( new TiXmlText( help.String(scmprogress).c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "scmmoveme" );
    msg->LinkEndChild( new TiXmlText( BoolToString(scmmoveme) ));
    msgs->LinkEndChild( msg );


    msg = new TiXmlElement( "finalmode" );
    msg->LinkEndChild( new TiXmlText( BoolToString(map.finalmode) ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "finalstretch" );
    msg->LinkEndChild( new TiXmlText( BoolToString(map.finalstretch) ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "frames" );
    msg->LinkEndChild( new TiXmlText( help.String(frames).c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "seconds" );
    msg->LinkEndChild( new TiXmlText( help.String(seconds).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "minutes" );
    msg->LinkEndChild( new TiXmlText( help.String(minutes).c_str()) );
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "hours" );
    msg->LinkEndChild( new TiXmlText( help.String(hours).c_str()) );
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "deathcounts" );
    msg->LinkEndChild( new TiXmlText( help.String(deathcounts).c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "totalflips" );
    msg->LinkEndChild( new TiXmlText( help.String(totalflips).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "hardestroom" );
    msg->LinkEndChild( new TiXmlText( hardestroom.c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "hardestroomdeaths" );
    msg->LinkEndChild( new TiXmlText( help.String(hardestroomdeaths).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "summary" );
    std::string summary = savearea + ", " + timestring();
    msg->LinkEndChild( new TiXmlText( summary.c_str() ));
    msgs->LinkEndChild( msg );

    quicksummary = summary;

    if(FILESYSTEM_saveTiXmlDocument("saves/qsave.vvv", &doc))
    {
        printf("Game saved\n");
    }
    else
    {
        printf("Could Not Save game!\n");
        printf("Failed: %s%s\n", saveFilePath.c_str(), "qsave.vvv");
    }

}

void Game::customsavequick(std::string savfile)
{
    TiXmlDocument doc;
    TiXmlElement* msg;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );

    TiXmlElement * root = new TiXmlElement( "Save" );
    doc.LinkEndChild( root );

    TiXmlComment * comment = new TiXmlComment();
    comment->SetValue(" Save file " );
    root->LinkEndChild( comment );

    TiXmlElement * msgs = new TiXmlElement( "Data" );
    root->LinkEndChild( msgs );


    //Flags, map and stats

    std::string mapExplored;
    for(size_t i = 0; i < map.explored.size(); i++ )
    {
        mapExplored += help.String(map.explored[i]) + ",";
    }
    msg = new TiXmlElement( "worldmap" );
    msg->LinkEndChild( new TiXmlText( mapExplored.c_str() ));
    msgs->LinkEndChild( msg );

    std::string flags;
    for(size_t i = 0; i < obj.flags.size(); i++ )
    {
        flags += help.String((int) obj.flags[i]) + ",";
    }
    msg = new TiXmlElement( "flags" );
    msg->LinkEndChild( new TiXmlText( flags.c_str() ));
    msgs->LinkEndChild( msg );

    std::string moods;
    for(int i = 0; i < 6; i++ )
    {
        moods += help.String(obj.customcrewmoods[i]) + ",";
    }
    msg = new TiXmlElement( "moods" );
    msg->LinkEndChild( new TiXmlText( moods.c_str() ));
    msgs->LinkEndChild( msg );

    std::string crewstatsString;
    for(size_t i = 0; i < crewstats.size(); i++ )
    {
        crewstatsString += help.String(crewstats[i]) + ",";
    }
    msg = new TiXmlElement( "crewstats" );
    msg->LinkEndChild( new TiXmlText( crewstatsString.c_str() ));
    msgs->LinkEndChild( msg );

    std::string collect;
    for(size_t i = 0; i < obj.collect.size(); i++ )
    {
        collect += help.String((int) obj.collect[i]) + ",";
    }
    msg = new TiXmlElement( "collect" );
    msg->LinkEndChild( new TiXmlText( collect.c_str() ));
    msgs->LinkEndChild( msg );

    std::string customcollect;
    for(size_t i = 0; i < obj.customcollect.size(); i++ )
    {
        customcollect += help.String((int) obj.customcollect[i]) + ",";
    }
    msg = new TiXmlElement( "customcollect" );
    msg->LinkEndChild( new TiXmlText( customcollect.c_str() ));
    msgs->LinkEndChild( msg );

    //Position

    msg = new TiXmlElement( "finalx" );
    msg->LinkEndChild( new TiXmlText( help.String(map.finalx).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "finaly" );
    msg->LinkEndChild( new TiXmlText( help.String(map.finaly).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savex" );
    msg->LinkEndChild( new TiXmlText( help.String(savex).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savey" );
    msg->LinkEndChild( new TiXmlText( help.String(savey).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "saverx" );
    msg->LinkEndChild( new TiXmlText( help.String(saverx).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savery" );
    msg->LinkEndChild( new TiXmlText( help.String(savery).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savegc" );
    msg->LinkEndChild( new TiXmlText( help.String(savegc).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savedir" );
    msg->LinkEndChild( new TiXmlText( help.String(savedir).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "savepoint" );
    msg->LinkEndChild( new TiXmlText( help.String(savepoint).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "trinkets" );
    msg->LinkEndChild( new TiXmlText( help.String(trinkets()).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "crewmates" );
    msg->LinkEndChild( new TiXmlText( help.String(crewmates()).c_str() ));
    msgs->LinkEndChild( msg );


    //Special stats

    if(music.nicefade==1)
    {
        msg = new TiXmlElement( "currentsong" );
        msg->LinkEndChild( new TiXmlText( help.String(music.nicechange).c_str() ));
        msgs->LinkEndChild( msg );
    }
    else
    {
        msg = new TiXmlElement( "currentsong" );
        msg->LinkEndChild( new TiXmlText( help.String(music.currentsong).c_str() ));
        msgs->LinkEndChild( msg );
    }

    msg = new TiXmlElement( "teleportscript" );
    msg->LinkEndChild( new TiXmlText( teleportscript.c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "companion" );
    msg->LinkEndChild( new TiXmlText( help.String(companion).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "lastsaved" );
    msg->LinkEndChild( new TiXmlText( help.String(lastsaved).c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "supercrewmate" );
    msg->LinkEndChild( new TiXmlText( BoolToString(supercrewmate) ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "scmprogress" );
    msg->LinkEndChild( new TiXmlText( help.String(scmprogress).c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "scmmoveme" );
    msg->LinkEndChild( new TiXmlText( BoolToString(scmmoveme) ));
    msgs->LinkEndChild( msg );


    msg = new TiXmlElement( "frames" );
    msg->LinkEndChild( new TiXmlText( help.String(frames).c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "seconds" );
    msg->LinkEndChild( new TiXmlText( help.String(seconds).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "minutes" );
    msg->LinkEndChild( new TiXmlText( help.String(minutes).c_str()) );
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "hours" );
    msg->LinkEndChild( new TiXmlText( help.String(hours).c_str()) );
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "deathcounts" );
    msg->LinkEndChild( new TiXmlText( help.String(deathcounts).c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "totalflips" );
    msg->LinkEndChild( new TiXmlText( help.String(totalflips).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "hardestroom" );
    msg->LinkEndChild( new TiXmlText( hardestroom.c_str() ));
    msgs->LinkEndChild( msg );
    msg = new TiXmlElement( "hardestroomdeaths" );
    msg->LinkEndChild( new TiXmlText( help.String(hardestroomdeaths).c_str() ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "showminimap" );
    msg->LinkEndChild( new TiXmlText( BoolToString(map.customshowmm) ));
    msgs->LinkEndChild( msg );

    msg = new TiXmlElement( "summary" );
    std::string summary = savearea + ", " + timestring();
    msg->LinkEndChild( new TiXmlText( summary.c_str() ));
    msgs->LinkEndChild( msg );

    customquicksummary = summary;

    std::string levelfile = savfile.substr(7);
    if(FILESYSTEM_saveTiXmlDocument(("saves/"+levelfile+".vvv").c_str(), &doc))
    {
        printf("Game saved\n");
    }
    else
    {
        printf("Could Not Save game!\n");
        printf("Failed: %s%s%s", saveFilePath.c_str(), levelfile.c_str(), ".vvv");
    }
}


void Game::loadtele()
{
    TiXmlDocument doc;
    if (!FILESYSTEM_loadTiXmlDocument("saves/tsave.vvv", &doc)) return;

    TiXmlHandle hDoc(&doc);
    TiXmlElement* pElem;
    TiXmlHandle hRoot(0);


    {
        pElem=hDoc.FirstChildElement().Element();
        // should always have a valid root but handle gracefully if it does
        if (!pElem)
        {
            printf("Save Not Found\n");
        }

        // save this for later
        hRoot=TiXmlHandle(pElem);
    }


    for( pElem = hRoot.FirstChild( "Data" ).FirstChild().Element(); pElem; pElem=pElem->NextSiblingElement())
    {
        std::string pKey(pElem->Value());
        const char* pText = pElem->GetText() ;
        if(pText == NULL)
        {
            pText = "";
        }

        if (pKey == "worldmap")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                map.explored.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    map.explored.push_back(atoi(values[i].c_str()));
                }
            }
        }

        if (pKey == "flags")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                obj.flags.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    obj.flags.push_back((bool) atoi(values[i].c_str()));
                }
            }
        }

        if (pKey == "crewstats")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                crewstats.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    crewstats.push_back(atoi(values[i].c_str()));
                }
            }
        }

        if (pKey == "collect")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                obj.collect.clear();
                for(size_t i = 0; i < values.size(); i++)
                {
                    obj.collect.push_back((bool) atoi(values[i].c_str()));
                }
            }
        }

        if (pKey == "finalmode")
        {
            map.finalmode = atoi(pText);
        }
        if (pKey == "finalstretch")
        {
            map.finalstretch = atoi(pText);
        }

        if (map.finalmode)
        {
            map.final_colormode = false;
            map.final_mapcol = 0;
            map.final_colorframe = 0;
        }
        if (map.finalstretch)
        {
            map.finalstretch = true;
            map.final_colormode = true;
            map.final_mapcol = 0;
            map.final_colorframe = 1;
        }


        if (pKey == "finalx")
        {
            map.finalx = atoi(pText);
        }
        else if (pKey == "finaly")
        {
            map.finaly = atoi(pText);
        }
        else if (pKey == "savex")
        {
            savex = atoi(pText);
        }
        else if (pKey == "savey")
        {
            savey = atoi(pText);
        }
        else if (pKey == "saverx")
        {
            saverx = atoi(pText);
        }
        else if (pKey == "savery")
        {
            savery = atoi(pText);
        }
        else if (pKey == "savegc")
        {
            savegc = atoi(pText);
        }
        else if (pKey == "savedir")
        {
            savedir= atoi(pText);
        }
        else if (pKey == "savepoint")
        {
            savepoint = atoi(pText);
        }
        else if (pKey == "companion")
        {
            companion = atoi(pText);
        }
        else if (pKey == "lastsaved")
        {
            lastsaved = atoi(pText);
        }
        else if (pKey == "teleportscript")
        {
            teleportscript = pText;
        }
        else if (pKey == "supercrewmate")
        {
            supercrewmate = atoi(pText);
        }
        else if (pKey == "scmprogress")
        {
            scmprogress = atoi(pText);
        }
        else if (pKey == "scmmoveme")
        {
            scmmoveme = atoi(pText);
        }
        else if (pKey == "frames")
        {
            frames = atoi(pText);
            frames = 0;
        }
        else if (pKey == "seconds")
        {
            seconds = atoi(pText);
        }
        else if (pKey == "minutes")
        {
            minutes = atoi(pText);
        }
        else if (pKey == "hours")
        {
            hours = atoi(pText);
        }
        else if (pKey == "deathcounts")
        {
            deathcounts = atoi(pText);
        }
        else if (pKey == "totalflips")
        {
            totalflips = atoi(pText);
        }
        else if (pKey == "hardestroom")
        {
            hardestroom = pText;
        }
        else if (pKey == "hardestroomdeaths")
        {
            hardestroomdeaths = atoi(pText);
        }
        else if (pKey == "currentsong")
        {
            music.play(atoi(pText));
        }

    }

    map.showteleporters = true;
    if(obj.flags[12]) map.showtargets = true;
    if (obj.flags[42]) map.showtrinkets = true;
}

std::string Game::unrescued()
{
    //Randomly return the name of an unrescued crewmate
    if (fRandom() * 100 > 50)
    {
        if (!crewstats[5]) return "Victoria";
        if (!crewstats[2]) return "Vitellary";
        if (!crewstats[4]) return "Verdigris";
        if (!crewstats[3]) return "Vermilion";
    }
    else
    {
        if (fRandom() * 100 > 50)
        {
            if (!crewstats[2]) return "Vitellary";
            if (!crewstats[4]) return "Verdigris";
            if (!crewstats[3]) return "Vermilion";
            if (!crewstats[5]) return "Victoria";
        }
        else
        {
            if (!crewstats[4]) return "Verdigris";
            if (!crewstats[3]) return "Vermilion";
            if (!crewstats[5]) return "Victoria";
            if (!crewstats[2]) return "Vitellary";
        }
    }
    return "you";
}

void Game::gameclock()
{
/*
test = true;
std::ostringstream os;
    os << hours << ":" << minutes << ":" << seconds << ", " << frames;
teststring = os.str();
*/
    frames++;
    if (frames >= 30)
    {
        frames -= 30;
        seconds++;
        if (seconds >= 60)
        {
            seconds -= 60;
            minutes++;
            if (minutes >= 60)
            {
                minutes -= 60;
                hours++;
            }
        }
    }
}

std::string Game::giventimestring( int hrs, int min, int sec )
{
    std::string tempstring = "";
    if (hrs > 0)
    {
        tempstring += help.String(hrs) + ":";
    }
    tempstring += help.twodigits(min) + ":" + help.twodigits(sec);
    return tempstring;
}

std::string Game::timestring()
{
    std::string tempstring = "";
    if (hours > 0)
    {
        tempstring += help.String(hours) + ":";
    }
    tempstring += help.twodigits(minutes) + ":" + help.twodigits(seconds);
    return tempstring;
}

std::string Game::partimestring()
{
    //given par time in seconds:
    std::string tempstring = "";
    if (timetrialpar >= 60)
    {
        tempstring = help.twodigits(int((timetrialpar - (timetrialpar % 60)) / 60)) + ":" + help.twodigits(timetrialpar % 60);
    }
    else
    {
        tempstring = "00:" + help.twodigits(timetrialpar);
    }
    return tempstring;
}

std::string Game::resulttimestring()
{
    //given result time in seconds:
    std::string tempstring = "";
    if (timetrialresulttime > 60)
    {
        tempstring = help.twodigits(int((timetrialresulttime - (timetrialresulttime % 60)) / 60)) + ":"
                     + help.twodigits(timetrialresulttime % 60);
    }
    else
    {
        tempstring = "00:" + help.twodigits(timetrialresulttime);
    }
    return tempstring;
}

std::string Game::timetstring( int t )
{
    //given par time in seconds:
    std::string tempstring = "";
    if (t >= 60)
    {
        tempstring = help.twodigits(int((t - (t % 60)) / 60)) + ":" + help.twodigits(t % 60);
    }
    else
    {
        tempstring = "00:" + help.twodigits(t);
    }
    return tempstring;
}

void Game::returnmenu()
{
    if (menustack.empty())
    {
        puts("Error: returning to previous menu frame on empty stack!");
        return;
    }

    MenuStackFrame& frame = menustack[menustack.size()-1];

    //Store this in case createmenu() removes the stack frame
    int previousoption = frame.option;

    createmenu(frame.name, true);
    currentmenuoption = previousoption;

    //Remove the stackframe now, but createmenu() might have already gotten to it
    //if we were returning to the main menu
    if (!menustack.empty())
    {
        menustack.pop_back();
    }
}

void Game::returntomenu(enum Menu::MenuName t)
{
    if (currentmenuname == t)
    {
        //Why are you calling this function then?
        return;
    }

    //Unwind the menu stack until we reach our desired menu
    int i = menustack.size() - 1;
    while (i >= 0)
    {
        //If we pop it off we can't reference it anymore, so check for it now
        bool is_the_menu_we_want = menustack[i].name == t;

        returnmenu();

        if (is_the_menu_we_want)
        {
            break;
        }

        i--;
    }
}

void Game::createmenu( enum Menu::MenuName t, bool samemenu/*= false*/ )
{
    if (t == Menu::mainmenu)
    {
        //Either we've just booted up the game or returned from gamemode
        //Whichever it is, we shouldn't have a stack,
        //and most likely don't have a current stackframe
        menustack.clear();
    }
    else if (!samemenu)
    {
        MenuStackFrame frame;
        frame.option = currentmenuoption;
        frame.name = currentmenuname;
        menustack.push_back(frame);
    }

    currentmenuoption = 0;
    currentmenuname = t;
    menuxoff = 0;
    menuyoff = 0;
    menucountdown = 0;
    menuoptions.clear();

    switch (t)
    {
    case Menu::mainmenu:
#if !defined(MAKEANDPLAY)
        option("start game");
#endif
#if !defined(NO_CUSTOM_LEVELS)
        option("player levels");
#endif
        option("graphic options");
        option("game options");
#if !defined(MAKEANDPLAY)
        option("view credits");
#endif
        option("quit game");
        menuxoff = -16;
        menuyoff = -10;
        break;
#if !defined(NO_CUSTOM_LEVELS)
    case Menu::playerworlds:
        option("play a level");
 #if !defined(NO_EDITOR)
        option("level editor");
 #endif
        option("open level folder", FILESYSTEM_openDirectoryEnabled());
        option("back to menu");
        menuxoff = -30;
        menuyoff = -40;
        break;
    case Menu::levellist:
        if(ed.ListOfMetaData.size()==0)
        {
            option("ok");
            menuxoff = 0;
            menuyoff = -20;
        }
        else
        {
            for(int i=0; i<(int) ed.ListOfMetaData.size(); i++) // FIXME: int/size_t! -flibit
            {
                if(i>=levelpage*8 && i< (levelpage*8)+8)
                {
                    //This is, er, suboptimal. Whatever, life optimisation and all that
                    int tvar=-1;
                    for(int j=0; j<numcustomlevelstats; j++)
                    {
                        if(ed.ListOfMetaData[i].filename.substr(7) == customlevelstats[j])
                        {
                            tvar=j;
                            j=numcustomlevelstats+1;
                        }
                    }
                    std::string text;
                    if(tvar>=0)
                    {
                        if(customlevelscore[tvar]==0)
                        {
                            text = "   " + ed.ListOfMetaData[i].title;
                        }
                        else if(customlevelscore[tvar]==1)
                        {
                            text = " * " + ed.ListOfMetaData[i].title;
                        }
                        else if(customlevelscore[tvar]==3)
                        {
                            text = "** " + ed.ListOfMetaData[i].title;
                        }
                    }
                    else
                    {
                        text = "   " + ed.ListOfMetaData[i].title;
                    }
                    std::transform(text.begin(), text.end(), text.begin(), ::tolower);
                    option(text);
                }
            }
            if((size_t) ((levelpage*8)+8) <ed.ListOfMetaData.size())
            {
                option("next page");
            }
            else
            {
                option("first page");
            }
            if (levelpage == 0)
            {
                option("last page");
            }
            else
            {
                option("previous page");
            }
            option("return to menu");

            menuxoff = -90;
            menuyoff = 70-(menuoptions.size()*10);
        }
        break;
#endif
    case Menu::quickloadlevel:
        option("continue from save");
        option("start from beginning");
        option("back to levels");
        menuxoff = -40;
        menuyoff = -30;
        break;
    case Menu::youwannaquit:
        option("yes, quit");
        option("no, return");
        menuxoff = 0;
        menuyoff = -20;
        break;
    case Menu::errornostart:
        option("ok");
        menuxoff = 0;
        menuyoff = -20;
        break;
    case Menu::graphicoptions:
        option("toggle fullscreen");
        option("toggle letterbox");
        option("toggle filter");
        option("toggle analogue");
        option("toggle mouse");
        option("return");
        menuxoff = -50;
        menuyoff = 8;
        break;
    case Menu::ed_settings:
        option("change description");
        option("edit scripts");
        option("change music");
        option("load level");
        option("save level");
        option("quit to main menu");

        menuxoff = -50;
        menuyoff = -20;
        break;
    case Menu::ed_desc:
        option("change name");
        option("change author");
        option("change description");
        option("change website");
        option("back to settings");

        menuxoff = -40;
        menuyoff = 6;
        break;
    case Menu::ed_music:
        option("next song");
        option("back");
        menuxoff = -10;
        menuyoff = 16;
        break;
    case Menu::ed_quit:
        option("yes, save and quit");
        option("no, quit without saving");
        option("return to editor");
        menuxoff = -50;
        menuyoff = 8;
        break;
    case Menu::options:
        option("accessibility options");
#if !defined(MAKEANDPLAY)
        option("unlock play modes");
#endif
        option("game pad options");
        option("clear data");
        //Add extra menu for mmmmmm mod
        if(music.mmmmmm){
            option("soundtrack");
        }

        option("return");
        menuxoff = -40;
        menuyoff = 0;
        break;
    case Menu::accessibility:
        option("animated backgrounds");
        option("screen effects");
        option("text outline");
        option("invincibility");
        option("slowdown");
        option("load screen");
        option("room name bg");
        option("return");
        menuxoff = -85;
        menuyoff = -10;
        break;
    case Menu::controller:
        option("analog stick sensitivity");
        option("bind flip");
        option("bind enter");
        option("bind menu");
        option("return");
        menuxoff = -40;
        menuyoff = 10;
        break;
    case Menu::cleardatamenu:
        option("no! don't delete");
        option("yes, delete everything");
        menuxoff = -30;
        menuyoff = 64;
        break;
    case Menu::setinvincibility:
        option("no, return to options");
        option("yes, enable");
        menuxoff = -30;
        menuyoff = 64;
        break;
    case Menu::setslowdown:
        option("normal speed");
        option("80% speed");
        option("60% speed");
        option("40% speed");
        menuxoff = -40;
        menuyoff = 16;
        break;
    case Menu::unlockmenu:
        option("unlock time trials");
        option("unlock intermissions", !unlock[16]);
        option("unlock no death mode", !unlock[17]);
        option("unlock flip mode", !unlock[18]);
        option("unlock ship jukebox", (stat_trinkets<20));
        option("unlock secret lab", !unlock[8]);
        option("return");
        menuxoff = -70;
        menuyoff = -20;
        break;
    case Menu::credits:
        option("next page");
        option("last page");
        option("return");
        menuxoff = 20;
        menuyoff = 64;
        break;
    case Menu::credits2:
        option("next page");
        option("previous page");
        option("return");
        menuxoff = 20;
        menuyoff = 64;
        break;
    case Menu::credits25:
        option("next page");
        option("previous page");
        option("return");
        menuxoff = 20;
        menuyoff = 64;
        break;
    case Menu::credits3:
        option("next page");
        option("previous page");
        option("return");
        menuxoff = 20;
        menuyoff = 64;
        break;
    case Menu::credits4:
        option("next page");
        option("previous page");
        option("return");
        menuxoff = 20;
        menuyoff = 64;
        break;
    case Menu::credits5:
        option("next page");
        option("previous page");
        option("return");
        menuxoff = 20;
        menuyoff = 64;
        break;
    case Menu::credits6:
        option("first page");
        option("previous page");
        option("return");
        menuxoff = 20;
        menuyoff = 64;
        break;
    case Menu::play:
    {
        //Ok, here's where the unlock stuff comes into it:
        //First up, time trials:
        int temp = 0;
        if (unlock[0] && stat_trinkets >= 3 && !unlocknotify[9]) temp++;
        if (unlock[1] && stat_trinkets >= 6 && !unlocknotify[10]) temp++;
        if (unlock[2] && stat_trinkets >= 9 && !unlocknotify[11]) temp++;
        if (unlock[3] && stat_trinkets >= 12 && !unlocknotify[12]) temp++;
        if (unlock[4] && stat_trinkets >= 15 && !unlocknotify[13]) temp++;
        if (unlock[5] && stat_trinkets >= 18 && !unlocknotify[14]) temp++;
        if (temp > 0)
        {
            //you've unlocked a time trial!
            if (unlock[0] && stat_trinkets >= 3)
            {
                unlocknotify[9] = true;
                unlock[9] = true;
            }
            if (unlock[1] && stat_trinkets >= 6)
            {
                unlocknotify[10] = true;
                unlock[10] = true;
            }
            if (unlock[2] && stat_trinkets >= 9)
            {
                unlocknotify[11] = true;
                unlock[11] = true;
            }
            if (unlock[3] && stat_trinkets >= 12)
            {
                unlocknotify[12] = true;
                unlock[12] = true;
            }
            if (unlock[4] && stat_trinkets >= 15)
            {
                unlocknotify[13] = true;
                unlock[13] = true;
            }
            if (unlock[5] && stat_trinkets >= 18)
            {
                unlocknotify[14] = true;
                unlock[14] = true;
            }

            if (temp == 1)
            {
                createmenu(Menu::unlocktimetrial);
                savemystats = true;
            }
            else if (temp > 1)
            {
                createmenu(Menu::unlocktimetrials);
                savemystats = true;
            }
        }
        else
        {
            //Alright, we haven't unlocked any time trials. How about no death mode?
            temp = 0;
            if (bestrank[0] >= 2) temp++;
            if (bestrank[1] >= 2) temp++;
            if (bestrank[2] >= 2) temp++;
            if (bestrank[3] >= 2) temp++;
            if (bestrank[4] >= 2) temp++;
            if (bestrank[5] >= 2) temp++;
            if (temp >= 4 && !unlocknotify[17])
            {
                //Unlock No Death Mode
                unlocknotify[17] = true;
                unlock[17] = true;
                createmenu(Menu::unlocknodeathmode);
                savemystats = true;
            }
            //Alright then! Flip mode?
            else if (unlock[5] && !unlocknotify[18])
            {
                unlock[18] = true;
                unlocknotify[18] = true;
                createmenu(Menu::unlockflipmode);
                savemystats = true;
            }
            //What about the intermission levels?
            else if (unlock[7] && !unlocknotify[16])
            {
                unlock[16] = true;
                unlocknotify[16] = true;
                createmenu(Menu::unlockintermission);
                savemystats = true;
            }
            else
            {
                if (save_exists())
                {
                    option("continue");
                }
                else
                {
                    option("new game");
                }
                //ok, secret lab! no notification, but test:
                if (unlock[8])
                {
                    option("secret lab", !map.invincibility && slowdown == 30);
                }
                option("play modes");
                if (save_exists())
                {
                    option("new game");
                }
                option("return");
                if (unlock[8])
                {
                    menuxoff = -40;
                    menuyoff = -30;
                }
                else
                {
                    menuxoff = -20;
                    menuyoff = -40;
                }
            }
        }
        break;
    }
    case Menu::unlocktimetrial:
    case Menu::unlocktimetrials:
    case Menu::unlocknodeathmode:
    case Menu::unlockintermission:
    case Menu::unlockflipmode:
        option("continue");
        menuxoff = 20;
        menuyoff = 70;
        break;
    case Menu::newgamewarning:
        option("start new game");
        option("return to menu");
        menuxoff = -30;
        menuyoff = 64;
        break;
    case Menu::playmodes:
        option("time trials", !map.invincibility && slowdown == 30);
        option("intermissions", unlock[16]);
        option("no death mode", unlock[17] && !map.invincibility && slowdown == 30);
        option("flip mode", unlock[18]);
        option("return to play menu");
        menuxoff = -70;
        menuyoff = 8;
        break;
    case Menu::intermissionmenu:
        option("play intermission 1");
        option("play intermission 2");
        option("return to play menu");
        menuxoff = -50;
        menuyoff = -35;
        break;
    case Menu::playint1:
        option("Vitellary");
        option("Vermilion");
        option("Verdigris");
        option("Victoria");
        option("return");
        menuxoff = -60;
        menuyoff = 10;
        break;
    case Menu::playint2:
        option("Vitellary");
        option("Vermilion");
        option("Verdigris");
        option("Victoria");
        option("return");
        menuxoff = -60;
        menuyoff = 10;
        break;
    case Menu::continuemenu:
        map.settowercolour(3);
        option("continue from teleporter");
        option("continue from quicksave");
        option("return to play menu");
        menuxoff = -60;
        menuyoff = 20;
        break;
    case Menu::startnodeathmode:
        option("disable cutscenes");
        option("enable cutscenes");
        option("return to play menu");
        menuxoff = -60;
        menuyoff = 40;
        break;
    case Menu::gameover:
        menucountdown = 120;
        menudest=Menu::gameover2;
        break;
    case Menu::gameover2:
        option("return to play menu");
        menuxoff = -25;
        menuyoff = 80;
        break;
    case Menu::unlockmenutrials:
        option("space station 1", !unlock[9]);
        option("the laboratory", !unlock[10]);
        option("the tower", !unlock[11]);
        option("space station 2", !unlock[12]);
        option("the warp zone", !unlock[13]);
        option("the final level", !unlock[14]);

        option("return to unlock menu");
        menuxoff = -80;
        menuyoff = 0;
        break;
    case Menu::timetrials:
        option(unlock[9] ? "space station 1" : "???", unlock[9]);
        option(unlock[10] ? "the laboratory" : "???", unlock[10]);
        option(unlock[11] ? "the tower" : "???", unlock[11]);
        option(unlock[12] ? "space station 2" : "???", unlock[12]);
        option(unlock[13] ? "the warp zone" : "???", unlock[13]);
        option(unlock[14] ? "the final level" : "???", unlock[14]);

        option("return to play menu");
        menuxoff = -80;
        menuyoff = 0;
        break;
    case Menu::nodeathmodecomplete:
        menucountdown = 90;
        menudest = Menu::nodeathmodecomplete2;
        break;
    case Menu::nodeathmodecomplete2:
        option("return to play menu");
        menuxoff = -25;
        menuyoff = 70;
        break;
    case Menu::timetrialcomplete:
        menucountdown = 90;
        menudest=Menu::timetrialcomplete2;
        break;
    case Menu::timetrialcomplete2:
        menucountdown = 60;
        menudest=Menu::timetrialcomplete3;
        break;
    case Menu::timetrialcomplete3:
        option("return to play menu");
        option("try again");
        menuxoff = -25;
        menuyoff = 70;
        break;
    case Menu::gamecompletecontinue:
        option("return to play menu");
        menuxoff = -25;
        menuyoff = 70;
        break;
    }
}

void Game::deletequick()
{
    if( !FILESYSTEM_delete( "saves/qsave.vvv" ) )
        puts("Error deleting saves/qsave.vvv");
    else
        quicksummary = "";
}

void Game::deletetele()
{
    if( !FILESYSTEM_delete( "saves/tsave.vvv" ) )
        puts("Error deleting saves/tsave.vvv");
    else
        telesummary = "";
}

void Game::swnpenalty()
{
    //set the SWN clock back to the closest 5 second interval
    if (swntimer <= 150)
    {
        swntimer += 8;
        if (swntimer > 150) swntimer = 150;
    }
    else if (swntimer <= 300)
    {
        swntimer += 8;
        if (swntimer > 300) swntimer = 300;
    }
    else if (swntimer <= 450)
    {
        swntimer += 8;
        if (swntimer > 450) swntimer = 450;
    }
    else if (swntimer <= 600)
    {
        swntimer += 8;
        if (swntimer > 600) swntimer = 600;
    }
    else if (swntimer <= 750)
    {
        swntimer += 8;
        if (swntimer > 750) swntimer = 750;
    }
    else if (swntimer <= 900)
    {
        swntimer += 8;
        if (swntimer > 900) swntimer = 900;
    }
    else if (swntimer <= 1050)
    {
        swntimer += 8;
        if (swntimer > 1050) swntimer = 1050;
    }
    else if (swntimer <= 1200)
    {
        swntimer += 8;
        if (swntimer > 1200) swntimer = 1200;
    }
    else if (swntimer <= 1350)
    {
        swntimer += 8;
        if (swntimer > 1350) swntimer = 1350;
    }
    else if (swntimer <= 1500)
    {
        swntimer += 8;
        if (swntimer > 1500) swntimer = 1500;
    }
    else if (swntimer <= 1650)
    {
        swntimer += 8;
        if (swntimer > 1650) swntimer = 1650;
    }
    else if (swntimer <= 1800)
    {
        swntimer += 8;
        if (swntimer > 1800) swntimer = 1800;
    }
    else if (swntimer <= 2100)
    {
        swntimer += 8;
        if (swntimer > 2100) swntimer = 2100;
    }
    else if (swntimer <= 2400)
    {
        swntimer += 8;
        if (swntimer > 2400) swntimer = 2400;
    }
}

int Game::crewrescued()
{
    int temp = 0;
    for (size_t i = 0; i < crewstats.size(); i++)
    {
        if (crewstats[i])
        {
            temp++;
        }
    }
    return temp;
}

void Game::resetgameclock()
{
    frames = 0;
    seconds = 0;
    minutes = 0;
    hours = 0;
}

int Game::trinkets()
{
    int temp = 0;
    for (size_t i = 0; i < obj.collect.size(); i++)
    {
        if (obj.collect[i])
        {
            temp++;
        }
    }
    return temp;
}

int Game::crewmates()
{
    int temp = 0;
    for (size_t i = 0; i < obj.customcollect.size(); i++)
    {
        if (obj.customcollect[i])
        {
            temp++;
        }
    }
    return temp;
}

bool Game::anything_unlocked()
{
    for (size_t i = 0; i < unlock.size(); i++)
    {
        if (unlock[i] &&
        (i == 8 // Secret Lab
        || i >= 9 || i <= 14 // any Time Trial
        || i == 16 // Intermission replays
        || i == 17 // No Death Mode
        || i == 18)) // Flip Mode
        {
            return true;
        }
    }
    return false;
}

bool Game::save_exists()
{
    return telesummary != "" || quicksummary != "";
}

void Game::quittomenu()
{
    gamestate = TITLEMODE;
    graphics.fademode = 4;
    music.play(6);
    graphics.backgrounddrawn = false;
    map.tdrawback = true;
    graphics.flipmode = false;
    //Don't be stuck on the summary screen,
    //or "who do you want to play the level with?"
    //or "do you want cutscenes?"
    //or the confirm-load-quicksave menu
    if (intimetrial)
    {
        returntomenu(Menu::timetrials);
    }
    else if (inintermission)
    {
        returntomenu(Menu::intermissionmenu);
    }
    else if (nodeathmode)
    {
        returntomenu(Menu::playmodes);
    }
    else if (map.custommode)
    {
        returntomenu(Menu::levellist);
    }
    else if (save_exists() || anything_unlocked())
    {
        returntomenu(Menu::play);
    }
    else
    {
        createmenu(Menu::mainmenu);
    }
    script.hardreset();
}

void Game::returntolab()
{
    gamestate = GAMEMODE;
    graphics.fademode = 4;
    startscript = true;
    newscript="returntolab";
}
