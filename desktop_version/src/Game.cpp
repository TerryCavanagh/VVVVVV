#define GAME_DEFINITION
#include "Game.h"

#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tinyxml2.h>

#include "editor.h"
#include "Entity.h"
#include "Enums.h"
#include "FileSystemUtils.h"
#include "Graphics.h"
#include "KeyPoll.h"
#include "MakeAndPlay.h"
#include "Map.h"
#include "Music.h"
#include "Network.h"
#include "Script.h"
#include "UtilityClass.h"
#include "XMLUtils.h"

static bool GetButtonFromString(const char *pText, SDL_GameControllerButton *button)
{
    if (*pText == '0' ||
        *pText == 'a' ||
        *pText == 'A')
    {
        *button = SDL_CONTROLLER_BUTTON_A;
        return true;
    }
    if (SDL_strcmp(pText, "1") == 0 ||
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
        SDL_strcasecmp(pText, "BACK") == 0)
    {
        *button = SDL_CONTROLLER_BUTTON_BACK;
        return true;
    }
    if (*pText == '5' ||
        SDL_strcasecmp(pText, "GUIDE") == 0)
    {
        *button = SDL_CONTROLLER_BUTTON_GUIDE;
        return true;
    }
    if (*pText == '6' ||
        SDL_strcasecmp(pText, "START") == 0)
    {
        *button = SDL_CONTROLLER_BUTTON_START;
        return true;
    }
    if (*pText == '7' ||
        SDL_strcasecmp(pText, "LS") == 0)
    {
        *button = SDL_CONTROLLER_BUTTON_LEFTSTICK;
        return true;
    }
    if (*pText == '8' ||
        SDL_strcasecmp(pText, "RS") == 0)
    {
        *button = SDL_CONTROLLER_BUTTON_RIGHTSTICK;
        return true;
    }
    if (*pText == '9' ||
        SDL_strcasecmp(pText, "LB") == 0)
    {
        *button = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
        return true;
    }
    if (SDL_strcmp(pText, "10") == 0 ||
        SDL_strcasecmp(pText, "RB") == 0)
    {
        *button = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
        return true;
    }
    return false;
}


void Game::init(void)
{
    roomx = 0;
    roomy = 0;
    prevroomx = 0;
    prevroomy = 0;
    saverx = 0;
    savery = 0;

    mutebutton = 0;
    muted = false;
    musicmuted = false;
    musicmutebutton = 0;

    glitchrunkludge = false;
    gamestate = TITLEMODE;
    prevgamestate = TITLEMODE;
    hascontrol = true;
    jumpheld = false;
    advancetext = false;
    jumppressed = 0;
    gravitycontrol = 0;
    teleport = false;
    edteleportent = 0; //Added in the port!
    companion = 0;
    roomchange = false;


    quickrestartkludge = false;

    tapleft = 0;
    tapright = 0;

    press_right = 0;
    press_left = 0;


    pausescript = false;
    completestop = false;
    activeactivity = -1;
    act_fade = 0;
    prev_act_fade = 0;
    backgroundtext = false;
    startscript = false;
    inintermission = false;

    alarmon = false;
    alarmdelay = 0;
    blackout = false;
    creditposx = 0;
    creditposy = 0;
    creditposdelay = 0;
    oldcreditposx = 0;

    useteleporter = false;
    teleport_to_teleporter = 0;

    activetele = false;
    readytotele = 0;
    oldreadytotele = 0;
    activity_r = 0;
    activity_g = 0;
    activity_b = 0;
    creditposition = 0;
    oldcreditposition = 0;
    bestgamedeaths = -1;

    //Accessibility Options
    colourblindmode = false;
    noflashingmode = false;
    slowdown = 30;

    nodeathmode = false;
    nocutscenes = false;
    ndmresultcrewrescued = 0;
    ndmresulttrinkets = 0;

    customcol=0;

    SDL_memset(crewstats, false, sizeof(crewstats));
    SDL_memset(ndmresultcrewstats, false, sizeof(ndmresultcrewstats));
    SDL_memset(tele_crewstats, false, sizeof(tele_crewstats));
    SDL_memset(quick_crewstats, false, sizeof(quick_crewstats));
    SDL_memset(besttimes, -1, sizeof(besttimes));
    SDL_memset(bestframes, -1, sizeof(bestframes));
    SDL_memset(besttrinkets, -1, sizeof(besttrinkets));
    SDL_memset(bestlives, -1, sizeof(bestlives));
    SDL_memset(bestrank, -1, sizeof(bestrank));

    crewstats[0] = true;
    lastsaved = 0;

    tele_gametime = "00:00";
    tele_trinkets = 0;
    tele_currentarea = "Error! Error!";
    quick_gametime = "00:00";
    quick_trinkets = 0;
    quick_currentarea = "Error! Error!";

    //Menu stuff initiliased here:
    SDL_memset(unlock, false, sizeof(unlock));
    SDL_memset(unlocknotify, false, sizeof(unlock));

    currentmenuoption = 0;
    current_credits_list_index = 0;
    menuxoff = 0;
    menuyoff = 0;
    menucountdown = 0;
    levelpage=0;
    playcustomlevel=0;
    createmenu(Menu::mainmenu);

    silence_settings_error = false;

    deathcounts = 0;
    gameoverdelay = 0;
    frames = 0;
    seconds = 0;
    minutes = 0;
    hours = 0;
    gamesaved = false;
    gamesavefailed = false;
    savetime = "00:00";
    savearea = "nowhere";
    savetrinkets = 0;

    intimetrial = false;
    timetrialcountdown = 0;
    timetrialshinytarget = 0;
    timetrialparlost = false;
    timetrialpar = 0;
    timetrialresulttime = 0;
    timetrialresultframes = 0;
    timetrialresultshinytarget = 0;
    timetrialresulttrinkets = 0;
    timetrialresultpar = 0;
    timetrialresultdeaths = 0;

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

    tinyxml2::XMLDocument doc;
    if (!FILESYSTEM_loadTiXml2Document("saves/qsave.vvv", doc))
    {
        quicksummary = "";
        printf("Quick Save Not Found\n");
    }
    else
    {
        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;
        tinyxml2::XMLHandle hRoot(NULL);

        pElem=hDoc.FirstChildElement().ToElement();
        if (!pElem)
        {
            printf("Quick Save Appears Corrupted: No XML Root\n");
        }

        // save this for later
        hRoot=tinyxml2::XMLHandle(pElem);

        for( pElem = hRoot.FirstChildElement( "Data" ).FirstChild().ToElement(); pElem; pElem=pElem->NextSiblingElement())
        {
            const char* pKey = pElem->Value();
            const char* pText = pElem->GetText() ;

            if (SDL_strcmp(pKey, "summary") == 0)
            {
                quicksummary = pText;
            }


        }
    }


    tinyxml2::XMLDocument docTele;
    if (!FILESYSTEM_loadTiXml2Document("saves/tsave.vvv", docTele))
    {
        telesummary = "";
        printf("Teleporter Save Not Found\n");
    }
    else
    {
        tinyxml2::XMLHandle hDoc(&docTele);
        tinyxml2::XMLElement* pElem;
        tinyxml2::XMLHandle hRoot(NULL);


        {
            pElem=hDoc.FirstChildElement().ToElement();
            // should always have a valid root but handle gracefully if it does
            if (!pElem)
            {
                printf("Teleporter Save Appears Corrupted: No XML Root\n");
            }

            // save this for later
            hRoot=tinyxml2::XMLHandle(pElem);
        }

        for( pElem = hRoot.FirstChildElement( "Data" ).FirstChild().ToElement(); pElem; pElem=pElem->NextSiblingElement())
        {
            const char* pKey = pElem->Value();
            const char* pText = pElem->GetText() ;

            if (SDL_strcmp(pKey, "summary") == 0)
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

    ghostsenabled = false;
    gametimer = 0;

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

#if !defined(NO_CUSTOM_LEVELS)
    shouldreturntoeditor = false;
#endif

    over30mode = false;
    glitchrunnermode = false;

    ingame_titlemode = false;
#if !defined(NO_CUSTOM_LEVELS) && !defined(NO_EDITOR)
    ingame_editormode = false;
#endif
    kludge_ingametemp = Menu::mainmenu;

    disablepause = false;
}

void Game::lifesequence(void)
{
    if (lifeseq > 0)
    {
        int i = obj.getplayer();
        if (INBOUNDS_VEC(i, obj.entities))
        {
            obj.entities[i].invis = false;
            if (lifeseq == 2) obj.entities[i].invis = true;
            if (lifeseq == 6) obj.entities[i].invis = true;
            if (lifeseq >= 8) obj.entities[i].invis = true;
        }
        if (lifeseq > 5) gravitycontrol = savegc;

        lifeseq--;
        if (INBOUNDS_VEC(i, obj.entities) && lifeseq <= 0)
        {
            obj.entities[i].invis = false;
        }
    }
}

void Game::clearcustomlevelstats(void)
{
    //just clearing the array
    customlevelstats.clear();

    customlevelstatsloaded=false; //To ensure we don't load it where it isn't needed
}


void Game::updatecustomlevelstats(std::string clevel, int cscore)
{
    if (clevel.find("levels/") != std::string::npos)
    {
        clevel = clevel.substr(7);
    }
    int tvar=-1;
    for(size_t j=0; j<customlevelstats.size(); j++)
    {
        if(clevel==customlevelstats[j].name)
        {
            tvar=j;
            break;
        }
    }
    if(tvar>=0)
    {
        // We have an existing entry
        // Don't update it unless it's a higher score
        if (cscore > customlevelstats[tvar].score)
        {
            customlevelstats[tvar].score=cscore;
        }
    }
    else
    {
        //add a new entry
        CustomLevelStat levelstat = {clevel, cscore};
        customlevelstats.push_back(levelstat);
    }
    savecustomlevelstats();
}

#define LOAD_ARRAY_RENAME(ARRAY_NAME, DEST) \
    if (SDL_strcmp(pKey, #ARRAY_NAME) == 0 && pText[0] != '\0') \
    { \
        /* We're loading in 32-bit integers. If we need more than 16 chars,
         * something is seriously wrong */ \
        char buffer[16]; \
        size_t start = 0; \
        size_t i = 0; \
        \
        while (next_split_s(buffer, sizeof(buffer), &start, pText, ',')) \
        { \
            if (i >= SDL_arraysize(DEST)) \
            { \
                break; \
            } \
            \
            DEST[i] = help.Int(buffer); \
            ++i; \
        } \
    }

#define LOAD_ARRAY(ARRAY_NAME) LOAD_ARRAY_RENAME(ARRAY_NAME, ARRAY_NAME)

void Game::loadcustomlevelstats(void)
{
    if(customlevelstatsloaded)
    {
        return;
    }

    tinyxml2::XMLDocument doc;
    if (!FILESYSTEM_loadTiXml2Document("saves/levelstats.vvv", doc))
    {
        //No levelstats file exists; start new
        customlevelstats.clear();
        savecustomlevelstats();
        return;
    }

    // Old system
    std::vector<std::string> customlevelnames;
    std::vector<int> customlevelscores;

    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;
    tinyxml2::XMLHandle hRoot(NULL);

    {
        pElem=hDoc.FirstChildElement().ToElement();
        // should always have a valid root but handle gracefully if it does
        if (!pElem)
        {
            printf("Error: Levelstats file corrupted\n");
        }

        // save this for later
        hRoot=tinyxml2::XMLHandle(pElem);
    }

    // First pass, look for the new system of storing stats
    // If they don't exist, then fall back to the old system
    for (pElem = hRoot.FirstChildElement("Data").FirstChild().ToElement(); pElem; pElem = pElem->NextSiblingElement())
    {
        const char* pKey = pElem->Value();
        const char* pText = pElem->GetText();
        if (pText == NULL)
        {
            pText = "";
        }

        if (SDL_strcmp(pKey, "stats") == 0)
        {
            for (tinyxml2::XMLElement* stat_el = pElem->FirstChildElement(); stat_el; stat_el = stat_el->NextSiblingElement())
            {
                CustomLevelStat stat = {};

                if (stat_el->GetText() != NULL)
                {
                    stat.score = help.Int(stat_el->GetText());
                }

                if (stat_el->Attribute("name"))
                {
                    stat.name = stat_el->Attribute("name");
                }

                customlevelstats.push_back(stat);
            }

            return;
        }
    }


    // Since we're still here, we must be on the old system
    for( pElem = hRoot.FirstChildElement( "Data" ).FirstChild().ToElement(); pElem; pElem=pElem->NextSiblingElement())
    {
        const char* pKey = pElem->Value();
        const char* pText = pElem->GetText() ;
        if(pText == NULL)
        {
            pText = "";
        }

        LOAD_ARRAY_RENAME(customlevelscore, customlevelscores)

        if (SDL_strcmp(pKey, "customlevelstats") == 0 && pText[0] != '\0')
        {
            size_t start = 0;
            size_t len = 0;
            size_t prev_start = 0;

            while (next_split(&start, &len, &pText[start], '|'))
            {
                customlevelnames.push_back(std::string(&pText[prev_start], len));

                prev_start = start;
            }
        }
    }

    // If the two arrays happen to differ in length, just go with the smallest one
    for (int i = 0; i < VVV_min(customlevelnames.size(), customlevelscores.size()); i++)
    {
        CustomLevelStat stat = {customlevelnames[i], customlevelscores[i]};
        customlevelstats.push_back(stat);
    }
}

void Game::savecustomlevelstats(void)
{
    tinyxml2::XMLDocument doc;
    bool already_exists = FILESYSTEM_loadTiXml2Document("saves/levelstats.vvv", doc);
    if (!already_exists)
    {
        puts("No levelstats.vvv found. Creating new file");
    }

    xml::update_declaration(doc);

    tinyxml2::XMLElement * root = xml::update_element(doc, "Levelstats");

    xml::update_comment(root, " Levelstats Save file ");

    tinyxml2::XMLElement * msgs = xml::update_element(root, "Data");

    int numcustomlevelstats = customlevelstats.size();
    if(numcustomlevelstats>=200)numcustomlevelstats=199;
    xml::update_tag(msgs, "numcustomlevelstats", numcustomlevelstats);

    std::string customlevelscorestr;
    for(int i = 0; i < numcustomlevelstats; i++ )
    {
        customlevelscorestr += help.String(customlevelstats[i].score) + ",";
    }
    xml::update_tag(msgs, "customlevelscore", customlevelscorestr.c_str());

    std::string customlevelstatsstr;
    for(int i = 0; i < numcustomlevelstats; i++ )
    {
        customlevelstatsstr += customlevelstats[i].name + "|";
    }
    xml::update_tag(msgs, "customlevelstats", customlevelstatsstr.c_str());

    // New system
    tinyxml2::XMLElement* msg = xml::update_element_delete_contents(msgs, "stats");
    tinyxml2::XMLElement* stat_el;
    for (size_t i = 0; i < customlevelstats.size(); i++)
    {
        stat_el = doc.NewElement("stat");
        CustomLevelStat& stat = customlevelstats[i];

        stat_el->SetAttribute("name", stat.name.c_str());
        stat_el->LinkEndChild(doc.NewText(help.String(stat.score).c_str()));

        msg->LinkEndChild(stat_el);
    }

    if(FILESYSTEM_saveTiXml2Document("saves/levelstats.vvv", doc))
    {
        printf("Level stats saved\n");
    }
    else
    {
        printf("Could Not Save level stats!\n");
        printf("Failed: %s%s\n", saveFilePath, "levelstats.vvv");
    }
}

void Game::levelcomplete_textbox(void)
{
    if (graphics.flipmode)
    {
        graphics.createtextbox("", -1, 180, 165, 165, 255);
    }
    else
    {
        graphics.createtextbox("", -1, 12, 165, 165, 255);
    }
    graphics.addline("                                   ");
    graphics.addline("");
    graphics.addline("");
    graphics.textboxcenterx();
}

void Game::crewmate_textbox(const int r, const int g, const int b)
{
    if (graphics.flipmode)
    {
        graphics.createtextbox("", -1, 104, r, g, b);
    }
    else
    {
        graphics.createtextbox("", -1, 64 + 8 + 16, r, g, b);
    }
    graphics.addline("     You have rescued  ");
    graphics.addline("      a crew member!   ");
    graphics.addline("");
    graphics.textboxcenterx();
}

void Game::remaining_textbox(void)
{
    const int remaining = 6 - crewrescued();
    const char* string;
    char buffer[161]; /* 4 bytes per 40 chars (whole screen) plus 1 for null terminator */
    if (remaining == 1)
    {
        string = "  One remains  ";
    }
    else if (remaining > 0)
    {
        SDL_snprintf(
            buffer,
            sizeof(buffer),
            "  %s remain  ",
            help.number(remaining).c_str()
        );
        string = buffer;
    }
    else
    {
        string = "  All Crew Members Rescued!  ";
    }

    if (graphics.flipmode)
    {
        graphics.createtextbox(string, -1, 72, 174, 174, 174);
    }
    else
    {
        graphics.createtextbox(string, -1, 128 + 16, 174, 174, 174);
    }
    graphics.textboxcenterx();
}

void Game::actionprompt_textbox(void)
{
    if (graphics.flipmode)
    {
        graphics.createtextbox(" Press ACTION to continue ", -1, 20, 164, 164, 255);
    }
    else
    {
        graphics.createtextbox(" Press ACTION to continue ", -1, 196, 164, 164, 255);
    }
    graphics.textboxcenterx();
}

void Game::updatestate(void)
{
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

            //Prevent softlocks if there's no cutscene running right now
            if (!script.running)
            {
                hascontrol = true;
            }
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
        {
            //leaving the naughty corner
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[obj.getplayer()].tile = 0;
            }
            state = 0;
            break;
        }
        case 16:
        {
            //entering the naughty corner
            int i = obj.getplayer();
            if(INBOUNDS_VEC(i, obj.entities) && obj.entities[i].tile == 0)
            {
                obj.entities[i].tile = 144;
                music.playef(2);
            }
            state = 0;
            break;
        }

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
            music.play(6); //should be after quittomenu()
            state = 0;
            break;

        case 82:
            //Time Trial Complete!
            obj.removetrigger(82);
            hascontrol = false;

            timetrialresulttime = seconds + (minutes * 60) + (hours * 60 * 60);
            timetrialresultframes = frames;
            timetrialresulttrinkets = trinkets();
            timetrialresultshinytarget = timetrialshinytarget;
            timetrialresultpar = timetrialpar;
            timetrialresultdeaths = deathcounts;

            timetrialrank = 0;
            if (timetrialresulttime <= timetrialpar) timetrialrank++;
            if (trinkets() >= timetrialshinytarget) timetrialrank++;
            if (deathcounts == 0) timetrialrank++;

            if (timetrialresulttime < besttimes[timetriallevel]
            || (timetrialresulttime == besttimes[timetriallevel] && timetrialresultframes < bestframes[timetriallevel])
            || besttimes[timetriallevel]==-1)
            {
                besttimes[timetriallevel] = timetrialresulttime;
                bestframes[timetriallevel] = timetrialresultframes;
            }
            if (timetrialresulttrinkets > besttrinkets[timetriallevel] || besttrinkets[timetriallevel]==-1)
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
                    if(timetriallevel==0) unlockAchievement("vvvvvvtimetrial_station1_fixed");
                    if(timetriallevel==1) unlockAchievement("vvvvvvtimetrial_lab_fixed");
                    if(timetriallevel==2) unlockAchievement("vvvvvvtimetrial_tower_fixed");
                    if(timetriallevel==3) unlockAchievement("vvvvvvtimetrial_station2_fixed");
                    if(timetriallevel==4) unlockAchievement("vvvvvvtimetrial_warp_fixed");
                    if(timetriallevel==5) unlockAchievement("vvvvvvtimetrial_final_fixed");
                }
            }

            savestatsandsettings();

            graphics.fademode = 2;
            music.fadeout();
            state++;
            break;
        case 83:
            frames--;
            if(graphics.fademode == 1)	state++;
            break;
        case 84:
            quittomenu();
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


            int i = obj.getplayer();
            hascontrol = false;
            if (INBOUNDS_VEC(i, obj.entities) && obj.entities[i].onroof > 0 && gravitycontrol == 1)
            {
                gravitycontrol = 0;
                music.playef(1);
            }
            if (INBOUNDS_VEC(i, obj.entities) && obj.entities[i].onground > 0)
            {
                state++;
            }
        }
        break;
        case 102:
        {


            companion = 6;
            int i = obj.getcompanion();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].tile = 0;
                obj.entities[i].state = 1;
            }

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
            int i = obj.getcompanion();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].tile = 54;
                obj.entities[i].state = 0;
            }
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

            int i = obj.getcompanion();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].tile = 0;
                obj.entities[i].state = 1;
            }
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
            hascontrol = false;
            state++;
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

            int i = obj.getplayer();
            hascontrol = false;
            if (INBOUNDS_VEC(i, obj.entities) && obj.entities[i].onground > 0 && gravitycontrol == 0)
            {
                gravitycontrol = 1;
                music.playef(1);
            }
            if (INBOUNDS_VEC(i, obj.entities) && obj.entities[i].onroof > 0)
            {
                state++;
            }

        }
        break;
        case 122:
        {
            companion = 7;
            int i = obj.getcompanion();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].tile = 6;
                obj.entities[i].state = 1;
            }

            advancetext = true;
            hascontrol = false;

            graphics.createtextbox("Captain! You're ok!", 60-10, 90-40, 255, 255, 134);
            state++;
            music.playef(14);
            break;
        }
        case 124:
        {
            graphics.createtextbox("I've found a teleporter, but", 60-20, 90 - 40, 255, 255, 134);
            graphics.addline("I can't get it to go anywhere...");
            state++;
            music.playef(2);
            graphics.textboxactive();
            break;
        }
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
        {
            graphics.createtextbox("Yey! Let's go home!", 60-30, 90-35, 255, 255, 134);
            state++;
            music.playef(14);
            graphics.textboxactive();
            int i = obj.getcompanion();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].tile = 6;
                obj.entities[i].state = 1;
            }
            break;
        }
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


        // WARNING: If updating this code, make sure to update Map.cpp mapclass::twoframedelayfix()
        case 300:
        case 301:
        case 302:
        case 303:
        case 304:
        case 305:
        case 306:
        case 307:
        case 308:
        case 309:
        case 310:
        case 311:
        case 312:
        case 313:
        case 314:
        case 315:
        case 316:
        case 317:
        case 318:
        case 319:
        case 320:
        case 321:
        case 322:
        case 323:
        case 324:
        case 325:
        case 326:
        case 327:
        case 328:
        case 329:
        case 330:
        case 331:
        case 332:
        case 333:
        case 334:
        case 335:
        case 336:
            startscript = true;
            newscript="custom_"+customscript[state - 300];
            obj.removetrigger(state);
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
        case 1002:
            if (!advancetext)
            {
                // Prevent softlocks if we somehow don't have advancetext
                state++;
            }
            break;
        case 1003:
            graphics.textboxremove();
            hascontrol = true;
            advancetext = false;
            completestop = false;
            state = 0;
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
        case 1012:
            if (!advancetext)
            {
                // Prevent softlocks if we somehow don't have advancetext
                state++;
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
                    shouldreturntoeditor = true;
                    if(!muted && ed.levmusic>0) music.fadeMusicVolumeIn(3000);
                    if(ed.levmusic>0) music.fadeout();
                }
            }
            else
            {
                if(!muted && ed.levmusic>0) music.fadeMusicVolumeIn(3000);
            }
            graphics.showcutscenebars = false;
            break;
#endif
        case 1014:
            frames--;
            if(graphics.fademode == 1)	state++;
            break;
        case 1015:
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
            quittomenu();
            music.play(6); //should be after quittomenu()
            state = 0;
            break;


        case 2000:
            //Game Saved!
            if (inspecial() || map.custommode)
            {
                state = 0;
            }
            else
            {
                if (savetele())
                {
                    graphics.createtextbox("    Game Saved    ", -1, graphics.flipmode ? 202 : 12, 174, 174, 174);
                    graphics.textboxtimer(25);
                }
                else
                {
                    graphics.createtextbox("  ERROR: Could not save game!  ", -1, graphics.flipmode ? 202 : 12, 255, 60, 60);
                    graphics.textboxtimer(50);
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
        {
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].colour = 0;
                obj.entities[i].invis = false;

                int j = obj.getteleporter();
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[i].xp = obj.entities[j].xp+44;
                    obj.entities[i].yp = obj.entities[j].yp+44;
                    obj.entities[i].lerpoldxp = obj.entities[i].xp;
                    obj.entities[i].lerpoldyp = obj.entities[i].yp;
                }
                obj.entities[i].ay = -6;
                obj.entities[i].ax = 6;
                obj.entities[i].vy = -6;
                obj.entities[i].vx = 6;
            }

            i = obj.getteleporter();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].tile = 1;
                obj.entities[i].colour = 101;
            }
            break;
        }
        case 2503:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 2504:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                //obj.entities[i].xp += 10;
            }
            break;
        }
        case 2505:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 8;
            }
            break;
        }
        case 2506:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 6;
            }
            break;
        }
        case 2507:
        {
            state++;
            break;
        }
        case 2508:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 2;
            }
            break;
        }
        case 2509:
        {
            state++;
            statedelay = 15;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 1;
            }
            break;
        }
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
        {
            //Activating a teleporter 2
            state++;
            statedelay = 50;
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

            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].colour = 0;
                obj.entities[i].invis = true;
            }

            i = obj.getcompanion();
            if(INBOUNDS_VEC(i, obj.entities))
            {
                obj.disableentity(i);
            }

            i = obj.getteleporter();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].tile = 1;
                obj.entities[i].colour = 100;
            }
            break;
        }

        case 3006:
            //Level complete! (warp zone)
            unlocknum(4);
            lastsaved = 4;
            music.play(0);
            state++;
            statedelay = 75;

            levelcomplete_textbox();
            break;
        case 3007:
            state++;
            statedelay = 45;

            crewmate_textbox(175, 174, 174);
            break;
        case 3008:
            state++;
            statedelay = 45;

            remaining_textbox();
            break;
        case 3009:
            state++;
            statedelay = 0;

            actionprompt_textbox();
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


            levelcomplete_textbox();
            break;
        case 3021:
            state++;
            statedelay = 45;

            crewmate_textbox(174, 175, 174);
            break;
        case 3022:
            state++;
            statedelay = 45;

            remaining_textbox();
            break;
        case 3023:
            state++;
            statedelay = 0;

            actionprompt_textbox();
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

            levelcomplete_textbox();
            break;
        case 3041:
            state++;
            statedelay = 45;

            crewmate_textbox(174, 174, 175);
            break;
        case 3042:
            state++;
            statedelay = 45;

            remaining_textbox();
            break;
        case 3043:
            state++;
            statedelay = 0;

            actionprompt_textbox();
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


            levelcomplete_textbox();
            break;
        case 3051:
            state++;
            statedelay = 45;

            crewmate_textbox(175, 175, 174);
            break;
        case 3052:
            state++;
            statedelay = 45;

            remaining_textbox();
            break;
        case 3053:
            state++;
            statedelay = 0;

            actionprompt_textbox();
            break;
        case 3054:
            if (jumppressed)
            {
                state++;
                statedelay = 30;
                graphics.textboxremove();
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
                if (crewrescued() == 6)
                {
                    newscript = "startlevel_final";
                }
                else
                {
                    if (nocutscenes)
                    {
                        newscript="bigopenworldskip";
                    }
                    else
                    {
                        newscript = "bigopenworld";
                    }
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

            levelcomplete_textbox();
            break;
        case 3061:
            state++;
            statedelay = 45;

            crewmate_textbox(175, 174, 175);
            break;
        case 3062:
            state++;
            statedelay = 45;

            remaining_textbox();
            break;
        case 3063:
            state++;
            statedelay = 0;

            actionprompt_textbox();
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
            quittomenu();
            music.play(6); //should be after quittomenu();
            state = 0;
            break;

        case 3500:
            music.fadeout();
            state++;
            statedelay = 120;
            break;
        case 3501:
            //Game complete!
            unlockAchievement("vvvvvvgamecomplete");
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
            savetime += "." + help.twodigits(frames*100 / 30);
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

            actionprompt_textbox();
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
                unlockAchievement("vvvvvvgamecompleteflip");
                unlocknum(19);
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
                    unlockAchievement("vvvvvvcomplete500");
                }
            if (bestgamedeaths <= 250) {
                unlockAchievement("vvvvvvcomplete250");
            }
            if (bestgamedeaths <= 100) {
                unlockAchievement("vvvvvvcomplete100");
            }
            if (bestgamedeaths <= 50) {
                unlockAchievement("vvvvvvcomplete50");
            }
        }


            savestatsandsettings();
            if (nodeathmode)
            {
                unlockAchievement("vvvvvvmaster"); //bloody hell
                unlocknum(20);
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
        {
            //Activating a teleporter (long version for level complete)
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].colour = 102;
            }

            state++;
            statedelay = 30;
            flashlight = 5;
            screenshake = 90;
            music.playef(9);
            break;
        }
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
        {
            //Activating a teleporter 2
            state++;
            statedelay = 0;
            flashlight = 5;
            screenshake = 0;

            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].colour = 0;
                obj.entities[i].invis = true;
            }

            //we're done here!
            music.playef(10);
            statedelay = 60;
            break;
        }
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

            map.finalmode = false;
            map.final_colormode = false;
            map.final_mapcol = 0;
            map.final_colorframe = 0;
            map.finalstretch = false;

            graphics.setbars(320);

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
            copyndmresults();
            quittomenu();
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
        {
            //Activating a teleporter 2
            state++;
            statedelay = 10;

            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].colour = 0;
                obj.entities[i].invis = true;
            }

            i = obj.getteleporter();
            if(INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].tile = 1;
                obj.entities[i].colour = 100;
            }
            break;
        }
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
        {
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            int i = obj.getplayer();
            int j = obj.getteleporter();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[i].xp = obj.entities[j].xp+44;
                    obj.entities[i].yp = obj.entities[j].yp+44;
                    obj.entities[i].lerpoldxp = obj.entities[i].xp;
                    obj.entities[i].lerpoldyp = obj.entities[i].yp;
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
            }
            break;
        }
        case 4013:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4014:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4015:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 8;
            }
            break;
        }
        case 4016:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 6;
            }
            break;
        }
        case 4017:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 3;
            }
            break;
        }
        case 4018:
        {
            state++;
            statedelay = 15;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 1;
            }
            break;
        }
        case 4019:
        {
            if (intimetrial || nodeathmode || inintermission)
            {
            }
            else
            {
                savetele();
            }
            int i = obj.getteleporter();
            activetele = true;
            if (INBOUNDS_VEC(i, obj.entities))
            {
                teleblock.x = obj.entities[i].xp - 32;
                teleblock.y = obj.entities[i].yp - 32;
            }
            teleblock.w = 160;
            teleblock.h = 160;
            hascontrol = true;
            advancetext = false;
            state = 0;
            break;
        }

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
        {
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            int i = obj.getplayer();
            int j = obj.getteleporter();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[i].xp = obj.entities[j].xp+44;
                    obj.entities[i].yp = obj.entities[j].yp+44;
                    obj.entities[i].lerpoldxp = obj.entities[i].xp;
                    obj.entities[i].lerpoldyp = obj.entities[i].yp;
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
            }
            break;
        }
        case 4023:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 12;
            }
            break;
        }
        case 4024:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 12;
            }
            break;
        }
        case 4025:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4026:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 8;
            }
            break;
        }
        case 4027:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 5;
            }
            break;
        }
        case 4028:
        {
            state++;
            statedelay = 15;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 2;
            }
            break;
        }
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
        {
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            int i = obj.getplayer();
            int j = obj.getteleporter();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[i].xp = obj.entities[j].xp+44;
                    obj.entities[i].yp = obj.entities[j].yp+44;
                    obj.entities[i].lerpoldxp = obj.entities[i].xp;
                    obj.entities[i].lerpoldyp = obj.entities[i].yp;
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
            }
            break;
        }
        case 4033:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 12;
            }
            break;
        }
        case 4034:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 12;
            }
            break;
        }
        case 4035:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 10;
            }
            break;
        }
        case 4036:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 8;
            }
            break;
        }
        case 4037:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 5;
            }
            break;
        }
        case 4038:
        {
            state++;
            statedelay = 15;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 2;
            }
            break;
        }
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
        {
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            int i = obj.getplayer();
            int j = obj.getteleporter();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[i].xp = obj.entities[j].xp+44;
                    obj.entities[i].yp = obj.entities[j].yp+44;
                    obj.entities[i].lerpoldxp = obj.entities[i].xp;
                    obj.entities[i].lerpoldyp = obj.entities[i].yp;
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
            }
            break;
        }
        case 4043:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 12;
                obj.entities[i].yp -= 15;
            }
            break;
        }
        case 4044:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 12;
                obj.entities[i].yp -= 10;
            }
            break;
        }
        case 4045:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 12;
                obj.entities[i].yp -= 10;
            }
            break;
        }
        case 4046:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 8;
                obj.entities[i].yp -= 8;
            }
            break;
        }
        case 4047:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 6;
                obj.entities[i].yp -= 8;
            }
            break;
        }
        case 4048:
        {
            state++;
            statedelay = 15;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 3;
            }
            break;
        }
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
        {
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            int i = obj.getplayer();
            int j = obj.getteleporter();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[i].xp = obj.entities[j].xp+44;
                    obj.entities[i].yp = obj.entities[j].yp+44;
                    obj.entities[i].lerpoldxp = obj.entities[i].xp;
                    obj.entities[i].lerpoldyp = obj.entities[i].yp;
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
            }
            break;
        }
        case 4053:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 4;
                obj.entities[i].yp -= 15;
            }
            break;
        }
        case 4054:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 4;
                obj.entities[i].yp -= 10;
            }
            break;
        }
        case 4055:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 4;
                obj.entities[i].yp -= 10;
            }
            break;
        }
        case 4056:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 4;
                obj.entities[i].yp -= 8;
            }
            break;
        }
        case 4057:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 2;
                obj.entities[i].yp -= 8;
            }
            break;
        }
        case 4058:
        {
            state++;
            statedelay = 15;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 1;
            }
            break;
        }
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
        {
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            int i = obj.getplayer();
            int j = obj.getteleporter();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[i].xp = obj.entities[j].xp+44;
                    obj.entities[i].yp = obj.entities[j].yp+44;
                    obj.entities[i].lerpoldxp = obj.entities[i].xp;
                    obj.entities[i].lerpoldyp = obj.entities[i].yp;
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
            }
            break;
        }
        case 4063:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 28;
                obj.entities[i].yp -= 8;
            }
            break;
        }
        case 4064:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 28;
                obj.entities[i].yp -= 8;
            }
            break;
        }
        case 4065:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 25;
            }
            break;
        }
        case 4066:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 25;
            }
            break;
        }
        case 4067:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 20;
            }
            break;
        }
        case 4068:
        {
            state++;
            statedelay = 15;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 16;
            }
            break;
        }
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
        {
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            int i = obj.getplayer();
            int j = obj.getteleporter();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[i].xp = obj.entities[j].xp+44;
                    obj.entities[i].yp = obj.entities[j].yp+44;
                    obj.entities[i].lerpoldxp = obj.entities[i].xp;
                    obj.entities[i].lerpoldyp = obj.entities[i].yp;
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
            }
            break;
        }
        case 4073:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4074:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4075:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 8;
            }
            break;
        }
        case 4076:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 6;
            }
            break;
        }
        case 4077:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 3;
            }
            break;
        }
        case 4078:
        {
            state++;
            statedelay = 15;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 1;
            }
            break;
        }
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
        {
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            int i = obj.getplayer();
            int j = obj.getteleporter();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[i].xp = obj.entities[j].xp+44;
                    obj.entities[i].yp = obj.entities[j].yp+44;
                    obj.entities[i].lerpoldxp = obj.entities[i].xp;
                    obj.entities[i].lerpoldyp = obj.entities[i].yp;
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
            }
            break;
        }
        case 4083:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4084:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4085:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 8;
            }
            break;
        }
        case 4086:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 6;
            }
            break;
        }
        case 4087:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 3;
            }
            break;
        }
        case 4088:
        {
            state++;
            statedelay = 15;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 1;
            }
            break;
        }
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
        {
            //Activating a teleporter 2
            state++;
            statedelay = 5;

            int i = obj.getplayer();
            int j = obj.getteleporter();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[i].xp = obj.entities[j].xp+44;
                    obj.entities[i].yp = obj.entities[j].yp+44;
                    obj.entities[i].lerpoldxp = obj.entities[i].xp;
                    obj.entities[i].lerpoldyp = obj.entities[i].yp;
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
            }
            break;
        }
        case 4093:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4094:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4095:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 8;
            }
            break;
        }
        case 4096:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 6;
            }
            break;
        }
        case 4097:
        {
            state++;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 3;
            }
            break;
        }
        case 4098:
        {
            state++;
            statedelay = 15;
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 1;
            }
            break;
        }
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

void Game::gethardestroom(void)
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

void Game::deletestats(void)
{
    if (!FILESYSTEM_delete("saves/unlock.vvv"))
    {
        puts("Error deleting saves/unlock.vvv");
    }
    else
    {
        for (int i = 0; i < numunlock; i++)
        {
            unlock[i] = false;
            unlocknotify[i] = false;
        }
        for (int i = 0; i < numtrials; i++)
        {
            besttimes[i] = -1;
            bestframes[i] = -1;
            besttrinkets[i] = -1;
            bestlives[i] = -1;
            bestrank[i] = -1;
        }
#ifndef MAKEANDPLAY
        graphics.setflipmode = false;
#endif
        stat_trinkets = 0;
    }
}

void Game::deletesettings(void)
{
    if (!FILESYSTEM_delete("saves/settings.vvv"))
    {
        puts("Error deleting saves/settings.vvv");
    }
}

void Game::unlocknum( int t )
{
#if !defined(MAKEANDPLAY)
    if (map.custommode)
    {
        //Don't let custom levels unlock things!
        return;
    }

    unlock[t] = true;
    savestatsandsettings();
#endif
}

void Game::loadstats(ScreenSettings* screen_settings)
{
    tinyxml2::XMLDocument doc;
    if (!FILESYSTEM_loadTiXml2Document("saves/unlock.vvv", doc))
    {
        // Save unlock.vvv only. Maybe we have a settings.vvv laying around too,
        // and we don't want to overwrite that!
        savestats(screen_settings);

        printf("No Stats found. Assuming a new player\n");
    }

    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;
    tinyxml2::XMLHandle hRoot(NULL);


    {
        pElem=hDoc.FirstChildElement().ToElement();
        // should always have a valid root but handle gracefully if it does
        if (!pElem)
        {

        }
        ;

        // save this for later
        hRoot=tinyxml2::XMLHandle(pElem);
    }

    tinyxml2::XMLElement* dataNode = hRoot.FirstChildElement("Data").FirstChild().ToElement();

    for( pElem = dataNode; pElem; pElem=pElem->NextSiblingElement())
    {
        const char* pKey = pElem->Value();
        const char* pText = pElem->GetText() ;

        LOAD_ARRAY(unlock)

        LOAD_ARRAY(unlocknotify)

        LOAD_ARRAY(besttimes)

        LOAD_ARRAY(bestframes)

        LOAD_ARRAY(besttrinkets)


        LOAD_ARRAY(bestlives)


        LOAD_ARRAY(bestrank)



        if (SDL_strcmp(pKey, "bestgamedeaths") == 0)
        {
            bestgamedeaths = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "stat_trinkets") == 0)
        {
            stat_trinkets = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "swnbestrank") == 0)
        {
            swnbestrank = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "swnrecord") == 0)
        {
            swnrecord = help.Int(pText);
        }
    }

    deserializesettings(dataNode, screen_settings);
}

void Game::deserializesettings(tinyxml2::XMLElement* dataNode, ScreenSettings* screen_settings)
{
    // Don't duplicate controller buttons!
    controllerButton_flip.clear();
    controllerButton_map.clear();
    controllerButton_esc.clear();
    controllerButton_restart.clear();

    for (tinyxml2::XMLElement* pElem = dataNode;
    pElem != NULL;
    pElem = pElem->NextSiblingElement())
    {
        const char* pKey = pElem->Value();
        const char* pText = pElem->GetText();

        if (SDL_strcmp(pKey, "fullscreen") == 0)
        {
            screen_settings->fullscreen = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "stretch") == 0)
        {
            screen_settings->stretch = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "useLinearFilter") == 0)
        {
            screen_settings->linearFilter = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "window_width") == 0)
        {
            screen_settings->windowWidth = help.Int(pText);
        }
        if (SDL_strcmp(pKey, "window_height") == 0)
        {
            screen_settings->windowHeight = help.Int(pText);
        }


        if (SDL_strcmp(pKey, "noflashingmode") == 0)
        {
            noflashingmode = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "colourblindmode") == 0)
        {
            colourblindmode = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "setflipmode") == 0)
        {
            graphics.setflipmode = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "invincibility") == 0)
        {
            map.invincibility = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "slowdown") == 0)
        {
            slowdown = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "advanced_smoothing") == 0)
        {
            screen_settings->badSignal = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "usingmmmmmm") == 0)
        {
            music.usingmmmmmm = (bool) help.Int(pText);
        }

        if (SDL_strcmp(pKey, "ghostsenabled") == 0)
        {
            ghostsenabled = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "skipfakeload") == 0)
        {
            skipfakeload = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "disablepause") == 0)
        {
            disablepause = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "over30mode") == 0)
        {
            over30mode = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "glitchrunnermode") == 0)
        {
            glitchrunnermode = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "vsync") == 0)
        {
            screen_settings->useVsync = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "notextoutline") == 0)
        {
            graphics.notextoutline = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "translucentroomname") == 0)
        {
            graphics.translucentroomname = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "showmousecursor") == 0)
        {
            graphics.showmousecursor = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "flipButton") == 0)
        {
            SDL_GameControllerButton newButton;
            if (GetButtonFromString(pText, &newButton))
            {
                controllerButton_flip.push_back(newButton);
            }
        }

        if (SDL_strcmp(pKey, "enterButton") == 0)
        {
            SDL_GameControllerButton newButton;
            if (GetButtonFromString(pText, &newButton))
            {
                controllerButton_map.push_back(newButton);
            }
        }

        if (SDL_strcmp(pKey, "escButton") == 0)
        {
            SDL_GameControllerButton newButton;
            if (GetButtonFromString(pText, &newButton))
            {
                controllerButton_esc.push_back(newButton);
            }
        }

        if (SDL_strcmp(pKey, "restartButton") == 0)
        {
            SDL_GameControllerButton newButton;
            if (GetButtonFromString(pText, &newButton))
            {
                controllerButton_restart.push_back(newButton);
            }
        }

        if (SDL_strcmp(pKey, "controllerSensitivity") == 0)
        {
            key.sensitivity = help.Int(pText);
        }

    }

    if (graphics.showmousecursor)
    {
        SDL_ShowCursor(SDL_ENABLE);
    }
    else
    {
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
    if (controllerButton_restart.size() < 1)
    {
        controllerButton_restart.push_back(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
    }
}

bool Game::savestats(void)
{
    if (graphics.screenbuffer == NULL)
    {
        return false;
    }

    ScreenSettings screen_settings;
    graphics.screenbuffer->GetSettings(&screen_settings);

    return savestats(&screen_settings);
}

bool Game::savestats(const ScreenSettings* screen_settings)
{
    tinyxml2::XMLDocument doc;
    bool already_exists = FILESYSTEM_loadTiXml2Document("saves/unlock.vvv", doc);
    if (!already_exists)
    {
        puts("No unlock.vvv found. Creating new file");
    }

    xml::update_declaration(doc);

    tinyxml2::XMLElement * root = xml::update_element(doc, "Save");

    xml::update_comment(root, " Save file " );

    tinyxml2::XMLElement * dataNode = xml::update_element(root, "Data");

    std::string s_unlock;
    for(size_t i = 0; i < SDL_arraysize(unlock); i++ )
    {
        s_unlock += help.String(unlock[i]) + ",";
    }
    xml::update_tag(dataNode, "unlock", s_unlock.c_str());

    std::string s_unlocknotify;
    for(size_t i = 0; i < SDL_arraysize(unlocknotify); i++ )
    {
        s_unlocknotify += help.String(unlocknotify[i]) + ",";
    }
    xml::update_tag(dataNode, "unlocknotify", s_unlocknotify.c_str());

    std::string s_besttimes;
    for(size_t i = 0; i < SDL_arraysize(besttimes); i++ )
    {
        s_besttimes += help.String(besttimes[i]) + ",";
    }
    xml::update_tag(dataNode, "besttimes", s_besttimes.c_str());

    std::string s_bestframes;
    for (size_t i = 0; i < SDL_arraysize(bestframes); i++)
    {
        s_bestframes += help.String(bestframes[i]) + ",";
    }
    xml::update_tag(dataNode, "bestframes", s_bestframes.c_str());

    std::string s_besttrinkets;
    for(size_t i = 0; i < SDL_arraysize(besttrinkets); i++ )
    {
        s_besttrinkets += help.String(besttrinkets[i]) + ",";
    }
    xml::update_tag(dataNode, "besttrinkets", s_besttrinkets.c_str());

    std::string s_bestlives;
    for(size_t i = 0; i < SDL_arraysize(bestlives); i++ )
    {
        s_bestlives += help.String(bestlives[i]) + ",";
    }
    xml::update_tag(dataNode, "bestlives", s_bestlives.c_str());

    std::string s_bestrank;
    for(size_t i = 0; i < SDL_arraysize(bestrank); i++ )
    {
        s_bestrank += help.String(bestrank[i]) + ",";
    }
    xml::update_tag(dataNode, "bestrank", s_bestrank.c_str());

    xml::update_tag(dataNode, "bestgamedeaths", bestgamedeaths);

    xml::update_tag(dataNode, "stat_trinkets", stat_trinkets);

    xml::update_tag(dataNode, "swnbestrank", swnbestrank);

    xml::update_tag(dataNode, "swnrecord", swnrecord);

    serializesettings(dataNode, screen_settings);

    return FILESYSTEM_saveTiXml2Document("saves/unlock.vvv", doc);
}

bool Game::savestatsandsettings(void)
{
    const bool stats_saved = savestats();

    const bool settings_saved = savesettings();

    return stats_saved && settings_saved; // Not the same as `savestats() && savesettings()`!
}

void Game::savestatsandsettings_menu(void)
{
    // Call Game::savestatsandsettings(), but upon failure, go to the save error screen
    if (!savestatsandsettings() && !silence_settings_error)
    {
        createmenu(Menu::errorsavingsettings);
        map.nexttowercolour();
    }
}

void Game::serializesettings(tinyxml2::XMLElement* dataNode, const ScreenSettings* screen_settings)
{
    tinyxml2::XMLDocument& doc = xml::get_document(dataNode);

    xml::update_tag(dataNode, "fullscreen", (int) screen_settings->fullscreen);

    xml::update_tag(dataNode, "stretch", screen_settings->stretch);

    xml::update_tag(dataNode, "useLinearFilter", (int) screen_settings->linearFilter);

    xml::update_tag(dataNode, "window_width", screen_settings->windowWidth);

    xml::update_tag(dataNode, "window_height", screen_settings->windowHeight);

    xml::update_tag(dataNode, "noflashingmode", noflashingmode);

    xml::update_tag(dataNode, "colourblindmode", colourblindmode);

    xml::update_tag(dataNode, "setflipmode", graphics.setflipmode);

    xml::update_tag(dataNode, "invincibility", map.invincibility);

    xml::update_tag(dataNode, "slowdown", slowdown);


    xml::update_tag(dataNode, "advanced_smoothing", (int) screen_settings->badSignal);


    xml::update_tag(dataNode, "usingmmmmmm", music.usingmmmmmm);

    xml::update_tag(dataNode, "ghostsenabled", (int) ghostsenabled);

    xml::update_tag(dataNode, "skipfakeload", (int) skipfakeload);

    xml::update_tag(dataNode, "disablepause", (int) disablepause);

    xml::update_tag(dataNode, "notextoutline", (int) graphics.notextoutline);

    xml::update_tag(dataNode, "translucentroomname", (int) graphics.translucentroomname);

    xml::update_tag(dataNode, "showmousecursor", (int) graphics.showmousecursor);

    xml::update_tag(dataNode, "over30mode", (int) over30mode);

    xml::update_tag(dataNode, "glitchrunnermode", (int) glitchrunnermode);

    xml::update_tag(dataNode, "vsync", (int) screen_settings->useVsync);

    // Delete all controller buttons we had previously.
    // dataNode->FirstChildElement() shouldn't be NULL at this point...
    // we've already added a bunch of elements
    for (tinyxml2::XMLElement* element = dataNode->FirstChildElement();
    element != NULL;
    /* Increment code handled separately */)
    {
        const char* name = element->Name();

        if (SDL_strcmp(name, "flipButton") == 0
        || SDL_strcmp(name, "enterButton") == 0
        || SDL_strcmp(name, "escButton") == 0
        || SDL_strcmp(name, "restartButton") == 0)
        {
            // Can't just doc.DeleteNode(element) and then go to next,
            // element->NextSiblingElement() will be NULL.
            // Instead, store pointer of element we want to delete. Then
            // increment `element`. And THEN delete the element.
            tinyxml2::XMLElement* delete_this = element;

            element = element->NextSiblingElement();

            doc.DeleteNode(delete_this);
            continue;
        }

        element = element->NextSiblingElement();
    }

    // Now add them
    for (size_t i = 0; i < controllerButton_flip.size(); i += 1)
    {
        tinyxml2::XMLElement* msg = doc.NewElement("flipButton");
        msg->LinkEndChild(doc.NewText(help.String((int) controllerButton_flip[i]).c_str()));
        dataNode->LinkEndChild(msg);
    }
    for (size_t i = 0; i < controllerButton_map.size(); i += 1)
    {
        tinyxml2::XMLElement* msg = doc.NewElement("enterButton");
        msg->LinkEndChild(doc.NewText(help.String((int) controllerButton_map[i]).c_str()));
        dataNode->LinkEndChild(msg);
    }
    for (size_t i = 0; i < controllerButton_esc.size(); i += 1)
    {
        tinyxml2::XMLElement* msg = doc.NewElement("escButton");
        msg->LinkEndChild(doc.NewText(help.String((int) controllerButton_esc[i]).c_str()));
        dataNode->LinkEndChild(msg);
    }
    for (size_t i = 0; i < controllerButton_restart.size(); i += 1)
    {
        tinyxml2::XMLElement* msg = doc.NewElement("restartButton");
        msg->LinkEndChild(doc.NewText(help.String((int) controllerButton_restart[i]).c_str()));
        dataNode->LinkEndChild(msg);
    }

    xml::update_tag(dataNode, "controllerSensitivity", key.sensitivity);
}

void Game::loadsettings(ScreenSettings* screen_settings)
{
    tinyxml2::XMLDocument doc;
    if (!FILESYSTEM_loadTiXml2Document("saves/settings.vvv", doc))
    {
        savesettings(screen_settings);
        puts("No settings.vvv found");
    }

    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;
    tinyxml2::XMLHandle hRoot(NULL);

    {
        pElem = hDoc.FirstChildElement().ToElement();
        // should always have a valid root but handle gracefully if it doesn't
        if (!pElem)
        {
        }
        ;

        // save this for later
        hRoot = tinyxml2::XMLHandle(pElem);
    }

    tinyxml2::XMLElement* dataNode = hRoot.FirstChildElement("Data").FirstChild().ToElement();

    deserializesettings(dataNode, screen_settings);
}

bool Game::savesettings(void)
{
    if (graphics.screenbuffer == NULL)
    {
        return false;
    }

    ScreenSettings screen_settings;
    graphics.screenbuffer->GetSettings(&screen_settings);

    return savesettings(&screen_settings);
}

bool Game::savesettings(const ScreenSettings* screen_settings)
{
    tinyxml2::XMLDocument doc;
    bool already_exists = FILESYSTEM_loadTiXml2Document("saves/settings.vvv", doc);
    if (!already_exists)
    {
        puts("No settings.vvv found. Creating new file");
    }

    xml::update_declaration(doc);

    tinyxml2::XMLElement* root = xml::update_element(doc, "Settings");

    xml::update_comment(root, " Settings (duplicated from unlock.vvv) ");

    tinyxml2::XMLElement* dataNode = xml::update_element(root, "Data");

    serializesettings(dataNode, screen_settings);

    return FILESYSTEM_saveTiXml2Document("saves/settings.vvv", doc);
}

void Game::customstart(void)
{
    jumpheld = true;

    savex = edsavex;
    savey = edsavey;
    saverx = edsaverx;
    savery = edsavery;

    savegc = edsavegc;
    savedir = edsavedir; //Worldmap Start
    savepoint = 0;
    gravitycontrol = savegc;

    state = 0;
    deathseq = -1;
    lifeseq = 0;
}

void Game::start(void)
{
    jumpheld = true;

    savex = 232;
    savey = 113;
    saverx = 104;
    savery = 110;
    savegc = 0;
    savedir = 1; //Worldmap Start
    savepoint = 0;
    gravitycontrol = savegc;

    state = 0;
    deathseq = -1;
    lifeseq = 0;

    if (!nocutscenes) music.play(5);
}

void Game::deathsequence(void)
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
    if (INBOUNDS_VEC(i, obj.entities))
    {
        obj.entities[i].colour = 1;

        obj.entities[i].invis = false;
    }
    if (deathseq == 30)
    {
        if (nodeathmode)
        {
            music.fadeout();
            gameoverdelay = 60;
        }
        deathcounts++;
        music.playef(2);
        if (INBOUNDS_VEC(i, obj.entities))
        {
            obj.entities[i].invis = true;
        }
        if (map.finalmode)
        {
            if (roomx - 41 >= 0 && roomx - 41 < 20 && roomy - 48 >= 0 && roomy - 48 < 20)
            {
                map.roomdeathsfinal[roomx - 41 + (20 * (roomy - 48))]++;
                currentroomdeaths = map.roomdeathsfinal[roomx - 41 + (20 * (roomy - 48))];
            }
        }
        else
        {
            if (roomx - 100 >= 0 && roomx - 100 < 20 && roomy - 100 >= 0 && roomy - 100 < 20)
            {
                map.roomdeaths[roomx - 100 + (20*(roomy - 100))]++;
                currentroomdeaths = map.roomdeaths[roomx - 100 + (20 * (roomy - 100))];
            }
        }
    }
    if (INBOUNDS_VEC(i, obj.entities))
    {
        if (deathseq == 25) obj.entities[i].invis = true;
        if (deathseq == 20) obj.entities[i].invis = true;
        if (deathseq == 16) obj.entities[i].invis = true;
        if (deathseq == 14) obj.entities[i].invis = true;
        if (deathseq == 12) obj.entities[i].invis = true;
        if (deathseq < 10) obj.entities[i].invis = true;
    }
    if (!nodeathmode)
    {
        if (INBOUNDS_VEC(i, obj.entities) && deathseq <= 1) obj.entities[i].invis = false;
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

    state = 0;
    deathseq = -1;
    lifeseq = 0;
}

void Game::loadquick(void)
{
    tinyxml2::XMLDocument doc;
    if (!FILESYSTEM_loadTiXml2Document("saves/qsave.vvv", doc)) return;

    readmaingamesave(doc);
}

void Game::readmaingamesave(tinyxml2::XMLDocument& doc)
{
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;
    tinyxml2::XMLHandle hRoot(NULL);


    {
        pElem=hDoc.FirstChildElement().ToElement();
        // should always have a valid root but handle gracefully if it does
        if (!pElem)
        {
            printf("Save Not Found\n");
        }

        // save this for later
        hRoot=tinyxml2::XMLHandle(pElem);
    }

    for( pElem = hRoot.FirstChildElement( "Data" ).FirstChild().ToElement(); pElem; pElem=pElem->NextSiblingElement())
    {
        const char* pKey = pElem->Value();;
        const char* pText = pElem->GetText() ;
        if(pText == NULL)
        {
            pText = "";
        }

        LOAD_ARRAY_RENAME(worldmap, map.explored)

        LOAD_ARRAY_RENAME(flags, obj.flags)

        LOAD_ARRAY(crewstats)

        LOAD_ARRAY_RENAME(collect, obj.collect)

        if (SDL_strcmp(pKey, "finalmode") == 0)
        {
            map.finalmode = help.Int(pText);
        }
        if (SDL_strcmp(pKey, "finalstretch") == 0)
        {
            map.finalstretch = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "savex") == 0)
        {
            savex = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "savey") == 0)
        {
            savey = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "saverx") == 0)
        {
            saverx = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "savery") == 0)
        {
            savery = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "savegc") == 0)
        {
            savegc = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "savedir") == 0)
        {
            savedir= help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "savepoint") == 0)
        {
            savepoint = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "companion") == 0)
        {
            companion = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "lastsaved") == 0)
        {
            lastsaved = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "teleportscript") == 0)
        {
            teleportscript = pText;
        }
        else if (SDL_strcmp(pKey, "supercrewmate") == 0)
        {
            supercrewmate = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "scmprogress") == 0)
        {
            scmprogress = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "scmmoveme") == 0)
        {
            scmmoveme = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "frames") == 0)
        {
            frames = help.Int(pText);
            frames = 0;
        }
        else if (SDL_strcmp(pKey, "seconds") == 0)
        {
            seconds = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "minutes") == 0)
        {
            minutes = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "hours") == 0)
        {
            hours = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "deathcounts") == 0)
        {
            deathcounts = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "totalflips") == 0)
        {
            totalflips = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "hardestroom") == 0)
        {
            hardestroom = pText;
        }
        else if (SDL_strcmp(pKey, "hardestroomdeaths") == 0)
        {
            hardestroomdeaths = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "currentsong") == 0)
        {
            int song = help.Int(pText);
            if (song != -1)
            {
                music.play(song);
            }
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
        music.play(playmusic);
        return;
    }

    std::string levelfile = savfile.substr(7);
    tinyxml2::XMLDocument doc;
    if (!FILESYSTEM_loadTiXml2Document(("saves/"+levelfile+".vvv").c_str(), doc)) return;

    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;
    tinyxml2::XMLHandle hRoot(NULL);


    {
        pElem=hDoc.FirstChildElement().ToElement();
        // should always have a valid root but handle gracefully if it does
        if (!pElem)
        {
            printf("Save Not Found\n");
        }

        // save this for later
        hRoot=tinyxml2::XMLHandle(pElem);
    }

    for( pElem = hRoot.FirstChildElement( "Data" ).FirstChild().ToElement(); pElem; pElem=pElem->NextSiblingElement())
    {
        const char* pKey = pElem->Value();
        const char* pText = pElem->GetText() ;
        if(pText == NULL)
        {
            pText = "";
        }

        LOAD_ARRAY_RENAME(worldmap, map.explored)

        LOAD_ARRAY_RENAME(flags, obj.flags)

        LOAD_ARRAY_RENAME(moods, obj.customcrewmoods)

        LOAD_ARRAY(crewstats)

        LOAD_ARRAY_RENAME(collect, obj.collect)

        LOAD_ARRAY_RENAME(customcollect, obj.customcollect)

        if (SDL_strcmp(pKey, "finalmode") == 0)
        {
            map.finalmode = help.Int(pText);
        }
        if (SDL_strcmp(pKey, "finalstretch") == 0)
        {
            map.finalstretch = help.Int(pText);
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


        if (SDL_strcmp(pKey, "savex") == 0)
        {
            savex = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "savey") == 0)
        {
            savey = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "saverx") == 0)
        {
            saverx = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "savery") == 0)
        {
            savery = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "savegc") == 0)
        {
            savegc = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "savedir") == 0)
        {
            savedir= help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "savepoint") == 0)
        {
            savepoint = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "companion") == 0)
        {
            companion = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "lastsaved") == 0)
        {
            lastsaved = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "teleportscript") == 0)
        {
            teleportscript = pText;
        }
        else if (SDL_strcmp(pKey, "supercrewmate") == 0)
        {
            supercrewmate = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "scmprogress") == 0)
        {
            scmprogress = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "scmmoveme") == 0)
        {
            scmmoveme = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "frames") == 0)
        {
            frames = help.Int(pText);
            frames = 0;
        }
        else if (SDL_strcmp(pKey, "seconds") == 0)
        {
            seconds = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "minutes") == 0)
        {
            minutes = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "hours") == 0)
        {
            hours = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "deathcounts") == 0)
        {
            deathcounts = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "totalflips") == 0)
        {
            totalflips = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "hardestroom") == 0)
        {
            hardestroom = pText;
        }
        else if (SDL_strcmp(pKey, "hardestroomdeaths") == 0)
        {
            hardestroomdeaths = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "currentsong") == 0)
        {
            int song = help.Int(pText);
            if (song != -1)
            {
                music.play(song);
            }
        }
        else if (SDL_strcmp(pKey, "showminimap") == 0)
        {
            map.customshowmm = help.Int(pText);
        }

    }

    map.showteleporters = true;
    if(obj.flags[12]) map.showtargets = true;

}

void Game::loadsummary(void)
{
    tinyxml2::XMLDocument docTele;
    if (!FILESYSTEM_loadTiXml2Document("saves/tsave.vvv", docTele))
    {
        telesummary = "";
    }
    else
    {
        tinyxml2::XMLHandle hDoc(&docTele);
        tinyxml2::XMLElement* pElem;
        tinyxml2::XMLHandle hRoot(NULL);


        {
            pElem=hDoc.FirstChildElement().ToElement();
            // should always have a valid root but handle gracefully if it does
            if (!pElem)
            {
                printf("Save Not Found\n");
            }

            // save this for later
            hRoot=tinyxml2::XMLHandle(pElem);
        }
        int l_minute, l_second, l_hours;
        l_minute = l_second= l_hours = 0;
        int l_saveX = 0;
        int l_saveY = 0;
        for( pElem = hRoot.FirstChildElement( "Data" ).FirstChild().ToElement(); pElem; pElem=pElem->NextSiblingElement())
        {
            const char* pKey = pElem->Value();
            const char* pText = pElem->GetText() ;

            if (SDL_strcmp(pKey, "summary") == 0)
            {
                telesummary = pText;
            }

            else if (SDL_strcmp(pKey, "seconds") == 0)
            {
                l_second = help.Int(pText);
            }
            else if (SDL_strcmp(pKey, "minutes") == 0)
            {
                l_minute = help.Int(pText);
            }
            else if (SDL_strcmp(pKey, "hours") == 0)
            {
                l_hours = help.Int(pText);
            }
            else if (SDL_strcmp(pKey, "savery") == 0)
            {
                l_saveY = help.Int(pText);
            }
            else if (SDL_strcmp(pKey, "saverx") == 0)
            {
                l_saveX = help.Int(pText);
            }
            else if (SDL_strcmp(pKey, "trinkets") == 0)
            {
                tele_trinkets = help.Int(pText);
            }
            else if (SDL_strcmp(pKey, "finalmode") == 0)
            {
                map.finalmode = help.Int(pText);
            }
            else if (SDL_strcmp(pKey, "finalstretch") == 0)
            {
                map.finalstretch = help.Int(pText);
            }

            LOAD_ARRAY_RENAME(crewstats, tele_crewstats)

        }
        tele_gametime = giventimestring(l_hours,l_minute, l_second);
        tele_currentarea = map.currentarea(map.area(l_saveX, l_saveY));
    }

    tinyxml2::XMLDocument doc;
    if (!FILESYSTEM_loadTiXml2Document("saves/qsave.vvv", doc))
    {
        quicksummary = "";
    }
    else
    {
        tinyxml2::XMLHandle hDoc(&doc);
        tinyxml2::XMLElement* pElem;
        tinyxml2::XMLHandle hRoot(NULL);


        {
            pElem=hDoc.FirstChildElement().ToElement();
            // should always have a valid root but handle gracefully if it does
            if (!pElem)
            {
                printf("Save Not Found\n");
            }

            // save this for later
            hRoot=tinyxml2::XMLHandle(pElem);
        }
        int l_minute, l_second, l_hours;
        l_minute = l_second= l_hours = 0;
        int l_saveX = 0;
        int l_saveY = 0;
        for( pElem = hRoot.FirstChildElement( "Data" ).FirstChild().ToElement(); pElem; pElem=pElem->NextSiblingElement())
        {
            const char* pKey = pElem->Value();
            const char* pText = pElem->GetText() ;

            if (SDL_strcmp(pKey, "summary") == 0)
            {
                quicksummary = pText;
            }

            else if (SDL_strcmp(pKey, "seconds") == 0)
            {
                l_second = help.Int(pText);
            }
            else if (SDL_strcmp(pKey, "minutes") == 0)
            {
                l_minute = help.Int(pText);
            }
            else if (SDL_strcmp(pKey, "hours") == 0)
            {
                l_hours = help.Int(pText);
            }
            else if (SDL_strcmp(pKey, "savery") == 0)
            {
                l_saveY = help.Int(pText);
            }
            else if (SDL_strcmp(pKey, "saverx") == 0)
            {
                l_saveX = help.Int(pText);
            }
            else if (SDL_strcmp(pKey, "trinkets") == 0)
            {
                quick_trinkets = help.Int(pText);
            }
            else if (SDL_strcmp(pKey, "finalmode") == 0)
            {
                map.finalmode = help.Int(pText);
            }
            else if (SDL_strcmp(pKey, "finalstretch") == 0)
            {
                map.finalstretch = help.Int(pText);
            }

            LOAD_ARRAY_RENAME(crewstats, quick_crewstats)

        }

        quick_gametime = giventimestring(l_hours,l_minute, l_second);
        quick_currentarea = map.currentarea(map.area(l_saveX, l_saveY));
    }



}

void Game::initteleportermode(void)
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

bool Game::savetele(void)
{
    if (map.custommode || inspecial())
    {
        //Don't trash save data!
        return false;
    }

    tinyxml2::XMLDocument doc;
    bool already_exists = FILESYSTEM_loadTiXml2Document("saves/tsave.vvv", doc);
    if (!already_exists)
    {
        puts("No tsave.vvv found. Creating new file");
    }
    telesummary = writemaingamesave(doc);

    if(!FILESYSTEM_saveTiXml2Document("saves/tsave.vvv", doc))
    {
        printf("Could Not Save game!\n");
        printf("Failed: %s%s\n", saveFilePath, "tsave.vvv");
        return false;
    }
    printf("Game saved\n");
    return true;
}


bool Game::savequick(void)
{
    if (map.custommode || inspecial())
    {
        //Don't trash save data!
        return false;
    }

    tinyxml2::XMLDocument doc;
    bool already_exists = FILESYSTEM_loadTiXml2Document("saves/qsave.vvv", doc);
    if (!already_exists)
    {
        puts("No qsave.vvv found. Creating new file");
    }
    quicksummary = writemaingamesave(doc);

    if(!FILESYSTEM_saveTiXml2Document("saves/qsave.vvv", doc))
    {
        printf("Could Not Save game!\n");
        printf("Failed: %s%s\n", saveFilePath, "qsave.vvv");
        return false;
    }
    printf("Game saved\n");
    return true;
}

// Returns summary of save
std::string Game::writemaingamesave(tinyxml2::XMLDocument& doc)
{
    //TODO make this code a bit cleaner.

    if (map.custommode || inspecial())
    {
        //Don't trash save data!
        return "";
    }

    xml::update_declaration(doc);

    tinyxml2::XMLElement * root = xml::update_element(doc, "Save");

    xml::update_comment(root, " Save file " );

    tinyxml2::XMLElement * msgs = xml::update_element(root, "Data");


    //Flags, map and stats

    std::string mapExplored;
    for(size_t i = 0; i < SDL_arraysize(map.explored); i++ )
    {
        mapExplored += help.String(map.explored[i]) + ",";
    }
    xml::update_tag(msgs, "worldmap", mapExplored.c_str());

    std::string flags;
    for(size_t i = 0; i < SDL_arraysize(obj.flags); i++ )
    {
        flags += help.String((int) obj.flags[i]) + ",";
    }
    xml::update_tag(msgs, "flags", flags.c_str());

    std::string crewstatsString;
    for(size_t i = 0; i < SDL_arraysize(crewstats); i++ )
    {
        crewstatsString += help.String(crewstats[i]) + ",";
    }
    xml::update_tag(msgs, "crewstats", crewstatsString.c_str());

    std::string collect;
    for(size_t i = 0; i < SDL_arraysize(obj.collect); i++ )
    {
        collect += help.String((int) obj.collect[i]) + ",";
    }
    xml::update_tag(msgs, "collect", collect.c_str());

    //Position

    xml::update_tag(msgs, "savex", savex);

    xml::update_tag(msgs, "savey", savey);

    xml::update_tag(msgs, "saverx", saverx);

    xml::update_tag(msgs, "savery", savery);

    xml::update_tag(msgs, "savegc", savegc);

    xml::update_tag(msgs, "savedir", savedir);

    xml::update_tag(msgs, "savepoint", savepoint);

    xml::update_tag(msgs, "trinkets", trinkets());


    //Special stats

    if (music.nicefade)
    {
        xml::update_tag(msgs, "currentsong", music.nicechange);
    }
    else
    {
        xml::update_tag(msgs, "currentsong", music.currentsong);
    }

    xml::update_tag(msgs, "teleportscript", teleportscript.c_str());
    xml::update_tag(msgs, "companion", companion);

    xml::update_tag(msgs, "lastsaved", lastsaved);
    xml::update_tag(msgs, "supercrewmate", (int) supercrewmate);

    xml::update_tag(msgs, "scmprogress", scmprogress);
    xml::update_tag(msgs, "scmmoveme", (int) scmmoveme);


    xml::update_tag(msgs, "frames", frames);
    xml::update_tag(msgs, "seconds", seconds);

    xml::update_tag(msgs, "minutes", minutes);
    xml::update_tag(msgs, "hours", hours);

    xml::update_tag(msgs, "deathcounts", deathcounts);
    xml::update_tag(msgs, "totalflips", totalflips);

    xml::update_tag(msgs, "hardestroom", hardestroom.c_str());
    xml::update_tag(msgs, "hardestroomdeaths", hardestroomdeaths);

    xml::update_tag(msgs, "finalmode", (int) map.finalmode);
    xml::update_tag(msgs, "finalstretch", (int) map.finalstretch);


    std::string summary = savearea + ", " + timestring();
    xml::update_tag(msgs, "summary", summary.c_str());

    return summary;
}


bool Game::customsavequick(std::string savfile)
{
    const std::string levelfile = savfile.substr(7);

    tinyxml2::XMLDocument doc;
    bool already_exists = FILESYSTEM_loadTiXml2Document(("saves/" + levelfile + ".vvv").c_str(), doc);
    if (!already_exists)
    {
        printf("No %s.vvv found. Creating new file\n", levelfile.c_str());
    }

    xml::update_declaration(doc);

    tinyxml2::XMLElement * root = xml::update_element(doc, "Save");

    xml::update_comment(root, " Save file ");

    tinyxml2::XMLElement * msgs = xml::update_element(root, "Data");


    //Flags, map and stats

    std::string mapExplored;
    for(size_t i = 0; i < SDL_arraysize(map.explored); i++ )
    {
        mapExplored += help.String(map.explored[i]) + ",";
    }
    xml::update_tag(msgs, "worldmap", mapExplored.c_str());

    std::string flags;
    for(size_t i = 0; i < SDL_arraysize(obj.flags); i++ )
    {
        flags += help.String((int) obj.flags[i]) + ",";
    }
    xml::update_tag(msgs, "flags", flags.c_str());

    std::string moods;
    for(size_t i = 0; i < SDL_arraysize(obj.customcrewmoods); i++ )
    {
        moods += help.String(obj.customcrewmoods[i]) + ",";
    }
    xml::update_tag(msgs, "moods", moods.c_str());

    std::string crewstatsString;
    for(size_t i = 0; i < SDL_arraysize(crewstats); i++ )
    {
        crewstatsString += help.String(crewstats[i]) + ",";
    }
    xml::update_tag(msgs, "crewstats", crewstatsString.c_str());

    std::string collect;
    for(size_t i = 0; i < SDL_arraysize(obj.collect); i++ )
    {
        collect += help.String((int) obj.collect[i]) + ",";
    }
    xml::update_tag(msgs, "collect", collect.c_str());

    std::string customcollect;
    for(size_t i = 0; i < SDL_arraysize(obj.customcollect); i++ )
    {
        customcollect += help.String((int) obj.customcollect[i]) + ",";
    }
    xml::update_tag(msgs, "customcollect", customcollect.c_str());

    //Position

    xml::update_tag(msgs, "savex", savex);

    xml::update_tag(msgs, "savey", savey);

    xml::update_tag(msgs, "saverx", saverx);

    xml::update_tag(msgs, "savery", savery);

    xml::update_tag(msgs, "savegc", savegc);

    xml::update_tag(msgs, "savedir", savedir);

    xml::update_tag(msgs, "savepoint", savepoint);

    xml::update_tag(msgs, "trinkets", trinkets());

    xml::update_tag(msgs, "crewmates", crewmates());


    //Special stats

    if (music.nicefade)
    {
        xml::update_tag(msgs, "currentsong", music.nicechange );
    }
    else
    {
        xml::update_tag(msgs, "currentsong", music.currentsong);
    }

    xml::update_tag(msgs, "teleportscript", teleportscript.c_str());
    xml::update_tag(msgs, "companion", companion);

    xml::update_tag(msgs, "lastsaved", lastsaved);
    xml::update_tag(msgs, "supercrewmate", (int) supercrewmate);

    xml::update_tag(msgs, "scmprogress", scmprogress);
    xml::update_tag(msgs, "scmmoveme", (int) scmmoveme);


    xml::update_tag(msgs, "frames", frames);
    xml::update_tag(msgs, "seconds", seconds);

    xml::update_tag(msgs, "minutes", minutes);
    xml::update_tag(msgs, "hours", hours);

    xml::update_tag(msgs, "deathcounts", deathcounts);
    xml::update_tag(msgs, "totalflips", totalflips);

    xml::update_tag(msgs, "hardestroom", hardestroom.c_str());
    xml::update_tag(msgs, "hardestroomdeaths", hardestroomdeaths);

    xml::update_tag(msgs, "showminimap", (int) map.customshowmm);

    std::string summary = savearea + ", " + timestring();
    xml::update_tag(msgs, "summary", summary.c_str());

    customquicksummary = summary;

    if(!FILESYSTEM_saveTiXml2Document(("saves/"+levelfile+".vvv").c_str(), doc))
    {
        printf("Could Not Save game!\n");
        printf("Failed: %s%s%s\n", saveFilePath, levelfile.c_str(), ".vvv");
        return false;
    }
    printf("Game saved\n");
    return true;
}


void Game::loadtele(void)
{
    tinyxml2::XMLDocument doc;
    if (!FILESYSTEM_loadTiXml2Document("saves/tsave.vvv", doc)) return;

    readmaingamesave(doc);
}

std::string Game::unrescued(void)
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

void Game::gameclock(void)
{
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

std::string Game::timestring(void)
{
    std::string tempstring = "";
    if (hours > 0)
    {
        tempstring += help.String(hours) + ":";
    }
    tempstring += help.twodigits(minutes) + ":" + help.twodigits(seconds);
    return tempstring;
}

std::string Game::partimestring(void)
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

std::string Game::resulttimestring(void)
{
    //given result time in seconds:
    std::string tempstring = "";
    if (timetrialresulttime >= 60)
    {
        tempstring = help.twodigits(int((timetrialresulttime - (timetrialresulttime % 60)) / 60)) + ":"
                     + help.twodigits(timetrialresulttime % 60);
    }
    else
    {
        tempstring = "00:" + help.twodigits(timetrialresulttime);
    }
    tempstring += "." + help.twodigits(timetrialresultframes*100 / 30);
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

void Game::returnmenu(void)
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
        createmenu(t, true);
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
        currentmenuoption = 0;
    }

    currentmenuname = t;
    menuyoff = 0;
    int maxspacing = 30; // maximum value for menuspacing, can only become lower.
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
        menuyoff = -10;
        maxspacing = 15;
        break;
#if !defined(NO_CUSTOM_LEVELS)
    case Menu::playerworlds:
        option("play a level");
 #if !defined(NO_EDITOR)
        option("level editor");
 #endif
        option("open level folder", FILESYSTEM_openDirectoryEnabled());
        option("back to menu");
        menuyoff = -40;
        maxspacing = 15;
        break;
    case Menu::levellist:
        if(ed.ListOfMetaData.size()==0)
        {
            option("ok");
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
                    for(size_t j=0; j<customlevelstats.size(); j++)
                    {
                        if(ed.ListOfMetaData[i].filename.substr(7) == customlevelstats[j].name)
                        {
                            tvar=j;
                            break;
                        }
                    }
                    const char* prefix;
                    if(tvar>=0)
                    {
                        switch (customlevelstats[tvar].score)
                        {
                        case 0:
                        {
                            static const char tmp[] = "   ";
                            prefix = tmp;
                            break;
                        }
                        case 1:
                        {
                            static const char tmp[] = " * ";
                            prefix = tmp;
                            break;
                        }
                        case 3:
                        {
                            static const char tmp[] = "** ";
                            prefix = tmp;
                            break;
                        }
                        default:
                            SDL_assert(0 && "Unhandled menu text prefix!");
                            prefix = "";
                            break;
                        }
                    }
                    else
                    {
                        static const char tmp[] = "   ";
                        prefix = tmp;
                    }
                    char text[MENU_TEXT_BYTES];
                    SDL_snprintf(text, sizeof(text), "%s%s", prefix, ed.ListOfMetaData[i].title.c_str());
                    for (size_t ii = 0; text[ii] != '\0'; ++ii)
                    {
                        text[ii] = SDL_tolower(text[ii]);
                    }
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

            menuxoff = 20;
            menuyoff = 70-(menuoptions.size()*10);
            menuspacing = 5;
            return; // skip automatic centering, will turn out bad with levels list
        }
        break;
#endif
    case Menu::quickloadlevel:
        option("continue from save");
        option("start from beginning");
        option("back to levels");
        menuyoff = -30;
        break;
    case Menu::youwannaquit:
        option("yes, quit");
        option("no, return");
        menuyoff = -20;
        break;
    case Menu::errornostart:
        option("ok");
        menuyoff = -20;
        break;
    case Menu::graphicoptions:
        option("toggle fullscreen");
        option("scaling mode");
        option("resize to nearest", graphics.screenbuffer->isWindowed);
        option("toggle filter");
        option("toggle analogue");
        option("toggle fps");
        option("toggle vsync");
        option("return");
        menuyoff = -10;
        break;
    case Menu::ed_settings:
        option("change description");
        option("edit scripts");
        option("change music");
        option("editor ghosts");
        option("load level");
        option("save level");
        option("graphic options");
        option("game options");
        option("quit to main menu");

        menuyoff = -20;
        maxspacing = 15;
        break;
    case Menu::ed_desc:
        option("change name");
        option("change author");
        option("change description");
        option("change website");
        option("back to settings");

        menuyoff = 6;
        maxspacing = 15;
        break;
    case Menu::ed_music:
        option("next song");
        option("previous song");
        option("back");
        menuyoff = 16;
        maxspacing = 15;
        break;
    case Menu::ed_quit:
        option("yes, save and quit");
        option("no, quit without saving");
        option("return to editor");
        menuyoff = 8;
        maxspacing = 15;
        break;
    case Menu::options:
        option("accessibility options");
        option("advanced options");
#if !defined(MAKEANDPLAY)
        if (ingame_titlemode && unlock[18])
#endif
        {
            option("flip mode");
        }
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
        menuyoff = 0;
        break;
    case Menu::advancedoptions:
        option("toggle mouse");
        option("unfocus pause");
        option("fake load screen");
        option("room name background");
        option("glitchrunner mode");
        option("return");
        menuyoff = 0;
        break;
    case Menu::accessibility:
        option("animated backgrounds");
        option("screen effects");
        option("text outline");
        option("invincibility", !ingame_titlemode || (!insecretlab && !intimetrial && !nodeathmode));
        option("slowdown", !ingame_titlemode || (!insecretlab && !intimetrial && !nodeathmode));
        option("return");
        menuyoff = 0;
        break;
    case Menu::controller:
        option("analog stick sensitivity");
        option("bind flip");
        option("bind enter");
        option("bind menu");
        option("bind restart");
        option("return");
        menuyoff = 10;
        break;
    case Menu::cleardatamenu:
        option("no! don't delete");
        option("yes, delete everything");
        menuyoff = 64;
        break;
    case Menu::setinvincibility:
        option("no, return to options");
        option("yes, enable");
        menuyoff = 64;
        break;
    case Menu::setslowdown:
        option("normal speed");
        option("80% speed");
        option("60% speed");
        option("40% speed");
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
        menuyoff = -20;
        break;
    case Menu::credits:
        option("next page");
        option("last page");
        option("return");
        menuyoff = 64;
        break;
    case Menu::credits2:
        option("next page");
        option("previous page");
        option("return");
        menuyoff = 64;
        break;
    case Menu::credits25:
        option("next page");
        option("previous page");
        option("return");
        menuyoff = 64;
        break;
    case Menu::credits3:
        option("next page");
        option("previous page");
        option("return");
        menuyoff = 64;
        break;
    case Menu::credits4:
        option("next page");
        option("previous page");
        option("return");
        menuyoff = 64;
        break;
    case Menu::credits5:
        option("next page");
        option("previous page");
        option("return");
        menuyoff = 64;
        break;
    case Menu::credits6:
        option("first page");
        option("previous page");
        option("return");
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
                createmenu(Menu::unlocktimetrial, true);
                savestatsandsettings();
            }
            else if (temp > 1)
            {
                createmenu(Menu::unlocktimetrials, true);
                savestatsandsettings();
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
                createmenu(Menu::unlocknodeathmode, true);
                savestatsandsettings();
            }
            //Alright then! Flip mode?
            else if (unlock[5] && !unlocknotify[18])
            {
                unlock[18] = true;
                unlocknotify[18] = true;
                createmenu(Menu::unlockflipmode, true);
                savestatsandsettings();
            }
            //What about the intermission levels?
            else if (unlock[7] && !unlocknotify[16])
            {
                unlock[16] = true;
                unlocknotify[16] = true;
                createmenu(Menu::unlockintermission, true);
                savestatsandsettings();
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
                    menuyoff = -30;
                }
                else
                {
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
        menuyoff = 70;
        break;
    case Menu::newgamewarning:
        option("start new game");
        option("return to menu");
        menuyoff = 64;
        break;
    case Menu::playmodes:
        option("time trials", !map.invincibility && slowdown == 30);
        option("intermissions", unlock[16]);
        option("no death mode", unlock[17] && !map.invincibility && slowdown == 30);
        option("flip mode", unlock[18]);
        option("return to play menu");
        menuyoff = 8;
        maxspacing = 20;
        break;
    case Menu::intermissionmenu:
        option("play intermission 1");
        option("play intermission 2");
        option("return to play menu");
        menuyoff = -35;
        break;
    case Menu::playint1:
        option("Vitellary");
        option("Vermilion");
        option("Verdigris");
        option("Victoria");
        option("return");
        menuyoff = 10;
        break;
    case Menu::playint2:
        option("Vitellary");
        option("Vermilion");
        option("Verdigris");
        option("Victoria");
        option("return");
        menuyoff = 10;
        break;
    case Menu::continuemenu:
        map.settowercolour(3);
        option("continue from teleporter");
        option("continue from quicksave");
        option("return to play menu");
        menuyoff = 20;
        break;
    case Menu::startnodeathmode:
        option("disable cutscenes");
        option("enable cutscenes");
        option("return to play menu");
        menuyoff = 40;
        break;
    case Menu::gameover:
        menucountdown = 120;
        menudest=Menu::gameover2;
        break;
    case Menu::gameover2:
        option("return to play menu");
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
        menuyoff = 0;
        maxspacing = 15;
        break;
    case Menu::nodeathmodecomplete:
        menucountdown = 90;
        menudest = Menu::nodeathmodecomplete2;
        break;
    case Menu::nodeathmodecomplete2:
        option("return to play menu");
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
        menuyoff = 70;
        break;
    case Menu::gamecompletecontinue:
        option("return to play menu");
        menuyoff = 70;
        break;
    case Menu::errorsavingsettings:
        option("ok");
        option("silence");
        menuyoff = 10;
        break;
    }

    // Automatically center the menu. We must check the width of the menu with the initial horizontal spacing.
    // If it's too wide, reduce the horizontal spacing by 5 and retry.
    // Try to limit the menu width to 272 pixels: 320 minus 16*2 for square brackets, minus 8*2 padding.
    // The square brackets fall outside the menu width (i.e. selected menu options are printed 16 pixels to the left)
    bool done_once = false;
    int menuwidth = 0;
    for (; !done_once || (menuwidth > 272 && menuspacing > 0); maxspacing -= 5)
    {
        done_once = true;
        menuspacing = maxspacing;
        menuwidth = 0;
        for (size_t i = 0; i < menuoptions.size(); i++)
        {
            int width = i*menuspacing + graphics.len(menuoptions[i].text);
            if (width > menuwidth)
                menuwidth = width;
        }
    }
    menuxoff = (320-menuwidth)/2;
}

void Game::deletequick(void)
{
    if( !FILESYSTEM_delete( "saves/qsave.vvv" ) )
        puts("Error deleting saves/qsave.vvv");
    else
        quicksummary = "";
}

void Game::deletetele(void)
{
    if( !FILESYSTEM_delete( "saves/tsave.vvv" ) )
        puts("Error deleting saves/tsave.vvv");
    else
        telesummary = "";
}

void Game::swnpenalty(void)
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

int Game::crewrescued(void)
{
    int temp = 0;
    for (size_t i = 0; i < SDL_arraysize(crewstats); i++)
    {
        if (crewstats[i])
        {
            temp++;
        }
    }
    return temp;
}

void Game::resetgameclock(void)
{
    frames = 0;
    seconds = 0;
    minutes = 0;
    hours = 0;
}

int Game::trinkets(void)
{
    int temp = 0;
    for (size_t i = 0; i < SDL_arraysize(obj.collect); i++)
    {
        if (obj.collect[i])
        {
            temp++;
        }
    }
    return temp;
}

int Game::crewmates(void)
{
    int temp = 0;
    for (size_t i = 0; i < SDL_arraysize(obj.customcollect); i++)
    {
        if (obj.customcollect[i])
        {
            temp++;
        }
    }
    return temp;
}

bool Game::anything_unlocked(void)
{
    for (size_t i = 0; i < SDL_arraysize(unlock); i++)
    {
        if (unlock[i] &&
        (i == 8 // Secret Lab
        || (i >= 9 && i <= 14) // any Time Trial
        || i == 16 // Intermission replays
        || i == 17 // No Death Mode
        || i == 18)) // Flip Mode
        {
            return true;
        }
    }
    return false;
}

bool Game::save_exists(void)
{
    return telesummary != "" || quicksummary != "";
}

void Game::quittomenu(void)
{
    gamestate = TITLEMODE;
    graphics.fademode = 4;
    FILESYSTEM_unmountassets();
    graphics.titlebg.tdrawback = true;
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
        if (map.custommodeforreal)
        {
            returntomenu(Menu::levellist);
        }
        else
        {
            //Returning from editor
            returntomenu(Menu::playerworlds);
        }
    }
    else if (save_exists() || anything_unlocked())
    {
        returntomenu(Menu::play);
        if (!insecretlab)
        {
            //Select "continue"
            currentmenuoption = 0;
        }
    }
    else
    {
        createmenu(Menu::mainmenu);
    }
    script.hardreset();
}

void Game::returntolab(void)
{
    gamestate = GAMEMODE;
    graphics.fademode = 4;
    map.gotoroom(119, 107);
    int player = obj.getplayer();
    if (INBOUNDS_VEC(player, obj.entities))
    {
        obj.entities[player].xp = 132;
        obj.entities[player].yp = 137;
    }
    gravitycontrol = 0;

    savepoint = 0;
    saverx = 119;
    savery = 107;
    savex = 132;
    savey = 137;
    savegc = 0;
    if (INBOUNDS_VEC(player, obj.entities))
    {
        savedir = obj.entities[player].dir;
    }

    music.play(11);
}

#if !defined(NO_CUSTOM_LEVELS)
void Game::returntoeditor(void)
{
    gamestate = EDITORMODE;

    graphics.textbox.clear();
    hascontrol = true;
    advancetext = false;
    completestop = false;
    state = 0;
    graphics.showcutscenebars = false;
    graphics.fademode = 0;

    ed.keydelay = 6;
    ed.settingskey = true;
    ed.oldnotedelay = 0;
    ed.notedelay = 0;
    ed.roomnamehide = 0;

    graphics.backgrounddrawn=false;
    music.fadeout();
    //If warpdir() is used during playtesting, we need to set it back after!
    for (int j = 0; j < ed.maxheight; j++)
    {
        for (int i = 0; i < ed.maxwidth; i++)
        {
           ed.level[i+(j*ed.maxwidth)].warpdir=ed.kludgewarpdir[i+(j*ed.maxwidth)];
        }
    }
    graphics.titlebg.scrolldir = 0;
}
#endif

void Game::returntoingame(void)
{
    ingame_titlemode = false;
    mapheld = true;
#if !defined(NO_CUSTOM_LEVELS) && !defined(NO_EDITOR)
    if (ingame_editormode)
    {
        ingame_editormode = false;
        returntomenu(Menu::ed_settings);
        gamestate = EDITORMODE;
        ed.settingskey = true;
    }
    else
#endif
    {
        returntomenu(kludge_ingametemp);
        gamestate = MAPMODE;
        graphics.flipmode = graphics.setflipmode;
        if (!map.custommode && !graphics.flipmode)
        {
            obj.flags[73] = true;
        }
    }
}

void Game::unlockAchievement(const char *name) {
#if !defined(MAKEANDPLAY)
    if (!map.custommode) NETWORK_unlockAchievement(name);
#endif
}

void Game::mapmenuchange(const int newgamestate)
{
    prevgamestate = gamestate;
    gamestate = newgamestate;
    graphics.resumegamemode = false;
    mapheld = true;

    if (prevgamestate == GAMEMODE)
    {
        graphics.menuoffset = 240;
        if (map.extrarow)
        {
            graphics.menuoffset -= 10;
        }
    }
    else
    {
        graphics.menuoffset = 0;
    }
    graphics.oldmenuoffset = graphics.menuoffset;
}

void Game::copyndmresults(void)
{
    ndmresultcrewrescued = crewrescued();
    ndmresulttrinkets = trinkets();
    ndmresulthardestroom = hardestroom;
    SDL_memcpy(ndmresultcrewstats, crewstats, sizeof(ndmresultcrewstats));
}
