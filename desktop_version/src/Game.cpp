#define GAME_DEFINITION
#include "Game.h"

#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <tinyxml2.h>

#include "ButtonGlyphs.h"
#include "Constants.h"
#include "CustomLevels.h"
#include "DeferCallbacks.h"
#include "Editor.h"
#include "Entity.h"
#include "Enums.h"
#include "FileSystemUtils.h"
#include "Font.h"
#include "GlitchrunnerMode.h"
#include "Graphics.h"
#include "Localization.h"
#include "LocalizationStorage.h"
#include "KeyPoll.h"
#include "MakeAndPlay.h"
#include "Map.h"
#include "Music.h"
#include "Network.h"
#include "RoomnameTranslator.h"
#include "Screen.h"
#include "Script.h"
#include "Unused.h"
#include "UtilityClass.h"
#include "VFormat.h"
#include "Vlogging.h"
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

static const char* get_summary(
    const char* filename,
    const char* savename,
    tinyxml2::XMLDocument& doc
) {
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;
    bool success;
    const char* retval = "";

    success = FILESYSTEM_loadTiXml2Document(filename, doc);
    if (!success)
    {
        vlog_info("%s not found", savename);
        goto end;
    }

    if (doc.Error())
    {
        vlog_error("Error parsing %s: %s", savename, doc.ErrorStr());
        goto end;
    }

    for (pElem = hDoc
        .FirstChildElement()
        .FirstChildElement("Data")
        .FirstChildElement()
        .ToElement();
    pElem != NULL;
    pElem = pElem->NextSiblingElement())
    {
        const char* pKey = pElem->Value();
        const char* pText = pElem->GetText();

        if (pText == NULL)
        {
            pText = "";
        }

        if (SDL_strcmp(pKey, "summary") == 0)
        {
            retval = pText;
        }
    }

end:
    return retval;
}

void Game::init(void)
{
    SDL_strlcpy(magic, "[vVvVvV]game", sizeof(magic));

    roomx = 0;
    roomy = 0;
    prevroomx = 0;
    prevroomy = 0;
    saverx = 0;
    savery = 0;
    savecolour = 0;

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


    quickrestartkludge = false;

    tapleft = 0;
    tapright = 0;

    press_right = false;
    press_left = false;
    press_action = false;
    press_map = false;
    press_interact = false;
    interactheld = false;
    separate_interact = false;
    mapheld = false;


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
    activity_y = 0;
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
    menutestmode = false;
    current_credits_list_index = 0;
    menuxoff = 0;
    menuyoff = 0;
    menucountdown = 0;
    levelpage=0;
    playcustomlevel=0;

    silence_settings_error = false;

    deathcounts = 0;
    gameoverdelay = 0;
    framecounter = 0;
    seed_use_sdl_getticks = false;
    editor_disabled = false;
    resetgameclock();
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
    timetrialcheater = false;
    timetrialresulttime = 0;
    timetrialresultframes = 0;
    timetrialresultshinytarget = 0;
    timetrialresulttrinkets = 0;
    timetrialresultpar = 0;
    timetrialresultdeaths = 0;
    start_translator_exploring = false;
    translator_exploring = false;
    translator_exploring_allowtele = false;
    translator_cutscene_test = false;

    totalflips = 0;
    hardestroom = "Welcome Aboard";
    hardestroomdeaths = 0;
    currentroomdeaths=0;

    inertia = 1.1f;
    swnmode = false;
    swntimer = 0;
    swngame = SWN_NONE; // Not playing sine wave ninja!
    swnstate = 0;
    swnstate2 = 0;
    swnstate3 = 0;
    swnstate4 = 0;
    swndelay = 0;
    swndeaths = 0;
    supercrewmate = false;
    scmhurt = false;
    scmprogress = 0;
    swncolstate = 0;
    swncoldelay = 0;
    swnrecord = 0;
    swnbestrank = 0;
    swnrank = 0;
    swnmessage = 0;

    clearcustomlevelstats();

    saveFilePath = FILESYSTEM_getUserSaveDirectory();

    tinyxml2::XMLDocument doc;
    quicksummary = get_summary("saves/qsave.vvv", "qsave.vvv", doc);


    tinyxml2::XMLDocument docTele;
    telesummary = get_summary("saves/tsave.vvv", "tsave.vvv", doc);

    screenshake = flashlight = 0 ;

    stat_trinkets = 0;

    state = 1;
    statedelay = 0;
    statelocked = false;
    //updatestate();

    skipfakeload = false;

    ghostsenabled = false;

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

    over30mode = true;
    showingametimer = false;

    ingame_titlemode = false;
    ingame_editormode = false;

    kludge_ingametemp = Menu::mainmenu;
    slidermode = SLIDER_NONE;

    disablepause = false;
    disableaudiopause = false;
    disabletemporaryaudiopause = true;
    inputdelay = false;
    rumble = false;

    old_skip_message_timer = 0;
    skip_message_timer = 0;

    setdefaultcontrollerbuttons();
}

void Game::setdefaultcontrollerbuttons(void)
{
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
    if (controllerButton_interact.size() < 1)
    {
        controllerButton_interact.push_back(SDL_CONTROLLER_BUTTON_X);
    }
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
        if (INBOUNDS_VEC(i, obj.entities) && (lifeseq <= 0 || noflashingmode))
        {
            obj.entities[i].invis = false;
        }
    }
}

void Game::clearcustomlevelstats(void)
{
    //just clearing the array
    customlevelstats.clear();
}


void Game::updatecustomlevelstats(std::string clevel, int cscore)
{
    if (!map.custommodeforreal)
    {
        /* We are playtesting, don't update level stats */
        return;
    }

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

void Game::deletecustomlevelstats(void)
{
    customlevelstats.clear();

    if (!FILESYSTEM_delete("saves/levelstats.vvv"))
    {
        vlog_error("Error deleting levelstats.vvv");
    }
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
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);

    if (!FILESYSTEM_loadTiXml2Document("saves/levelstats.vvv", doc))
    {
        //No levelstats file exists; start new
        customlevelstats.clear();
        savecustomlevelstats();
        return;
    }

    if (doc.Error())
    {
        vlog_error("Error parsing levelstats.vvv: %s", doc.ErrorStr());
        return;
    }

    customlevelstats.clear();

    // Old system
    std::vector<std::string> customlevelnames;
    std::vector<int> customlevelscores;

    tinyxml2::XMLElement* pElem;
    tinyxml2::XMLElement* firstElement;

    firstElement = hDoc
        .FirstChildElement()
        .FirstChildElement("Data")
        .FirstChildElement()
        .ToElement();

    // First pass, look for the new system of storing stats
    // If they don't exist, then fall back to the old system
    for (pElem = firstElement; pElem != NULL; pElem = pElem->NextSiblingElement())
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
    for (pElem = firstElement; pElem; pElem=pElem->NextSiblingElement())
    {
        const char* pKey = pElem->Value();
        const char* pText = pElem->GetText() ;
        if(pText == NULL)
        {
            pText = "";
        }

        if (SDL_strcmp(pKey, "customlevelscore") == 0 && pText[0] != '\0')
        {
            char buffer[16];
            size_t start = 0;

            while (next_split_s(buffer, sizeof(buffer), &start, pText, ','))
            {
                customlevelscores.push_back(help.Int(buffer));
            }
        }

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
    for (size_t i = 0; i < SDL_min(customlevelnames.size(), customlevelscores.size()); i++)
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
        vlog_info("No levelstats.vvv found. Creating new file");
    }
    else if (doc.Error())
    {
        vlog_error("Error parsing existing levelstats.vvv: %s", doc.ErrorStr());
        vlog_info("Creating new levelstats.vvv");
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
        vlog_info("Level stats saved");
    }
    else
    {
        vlog_error("Could Not Save level stats!");
        vlog_error("Failed: %s%s", saveFilePath, "levelstats.vvv");
    }
}

void Game::levelcomplete_textbox(void)
{
    graphics.createtextboxflipme("", -1, 12, 165, 165, 255);
    graphics.addline("                                    ");
    graphics.addline("");
    graphics.addline("");
    graphics.textboxprintflags(PR_FONT_8X8);
    graphics.textboxcenterx();
}

void Game::crewmate_textbox(const int r, const int g, const int b)
{
    const int extra_cjk_height = (font::height(PR_FONT_INTERFACE) * 4) - 32;
    graphics.createtextboxflipme("", -1, 64 + 8 + 16 - extra_cjk_height/2, r, g, b);

    /* This is a special case for wrapping, we MUST have two lines.
     * So just make sure it can't fit in one line. */
    const char* text = loc::gettext("You have rescued a crew member!");
    std::string wrapped = font::string_wordwrap_balanced(PR_FONT_INTERFACE, text, font::len(PR_FONT_INTERFACE, text)-1);

    size_t startline = 0;
    size_t newline;
    do {
        size_t pos_n = wrapped.find('\n', startline);
        size_t pos_p = wrapped.find('|', startline);
        newline = SDL_min(pos_n, pos_p);
        graphics.addline(wrapped.substr(startline, newline-startline));
        startline = newline+1;
    } while (newline != std::string::npos);

    graphics.addline("");
    graphics.textboxprintflags(PR_FONT_INTERFACE);
    graphics.textboxcentertext();
    float spaces_per_8 = font::len(PR_FONT_INTERFACE, " ")/8.0f;
    graphics.textboxpad(SDL_ceilf(5/spaces_per_8), SDL_ceilf(2/spaces_per_8));
    graphics.textboxcenterx();
}

void Game::remaining_textbox(void)
{
    const int remaining = 6 - crewrescued();
    char buffer[SCREEN_WIDTH_CHARS + 1];
    if (remaining > 0)
    {
        loc::gettext_plural_fill(buffer, sizeof(buffer), "{n_crew|wordy} remain", "{n_crew|wordy} remains", "n_crew:int", remaining);
    }
    else
    {
        SDL_strlcpy(buffer, loc::gettext("All Crew Members Rescued!"), sizeof(buffer));
    }

    // In CJK, the "You have rescued" box becomes so big we should lower this one a bit...
    const int cjk_lowering = font::height(PR_FONT_INTERFACE) - 8;
    graphics.createtextboxflipme(buffer, -1, 128 + 16 + cjk_lowering, TEXT_COLOUR("gray"));
    graphics.textboxprintflags(PR_FONT_INTERFACE);
    graphics.textboxpad(2, 2);
    graphics.textboxcenterx();
}

void Game::actionprompt_textbox(void)
{
    char buffer[SCREEN_WIDTH_CHARS + 1];
    vformat_buf(
        buffer, sizeof(buffer),
        loc::gettext("Press {button} to continue"),
        "button:but",
        vformat_button(ActionSet_InGame, Action_InGame_ACTION)
    );
    graphics.createtextboxflipme(buffer, -1, 196, TEXT_COLOUR("cyan"));
    graphics.textboxprintflags(PR_FONT_INTERFACE);
    graphics.textboxpad(1, 1);
    graphics.textboxcenterx();
}

void Game::savetele_textbox(void)
{
    if (inspecial() || map.custommode)
    {
        return;
    }

    if (savetele())
    {
        graphics.createtextboxflipme(loc::gettext("Game Saved"), -1, 12, TEXT_COLOUR("gray"));
        graphics.textboxprintflags(PR_FONT_INTERFACE);
        graphics.textboxpad(3, 3);
        graphics.textboxcenterx();
        graphics.textboxtimer(25);
    }
    else
    {
        graphics.createtextboxflipme(loc::gettext("ERROR: Could not save game!"), -1, 12, TEXT_COLOUR("red"));
        graphics.textboxprintflags(PR_FONT_INTERFACE);
        graphics.textboxwrap(2);
        graphics.textboxpad(1, 1);
        graphics.textboxcenterx();
        graphics.textboxtimer(50);
    }
}

void Game::setstate(const int gamestate)
{
    if (!statelocked)
    {
        state = gamestate;
    }
}

void Game::incstate(void)
{
    if (!statelocked)
    {
        state++;
    }
}

void Game::setstatedelay(const int delay)
{
    if (!statelocked)
    {
        statedelay = delay;
    }
}

void Game::lockstate(void)
{
    statelocked = true;
}

void Game::unlockstate(void)
{
    statelocked = false;
}

void Game::updatestate(void)
{
    statedelay--;
    if (statedelay <= 0)
    {
        statedelay = 0;
        glitchrunkludge=false;
    }
    if (statedelay <= 0)
    {
        switch(state)
        {
        case 0:
            //Do nothing here! Standard game state

            if (script.running)
            {
                if (pausescript && !advancetext)
                {
                    /* Prevent softlocks if we somehow don't have advancetext */
                    pausescript = false;
                }
            }
            else
            {
                if (completestop)
                {
                    /* Close potential collection dialogue if warping to ship */
                    graphics.textboxremove();
                    graphics.showcutscenebars = false;
                }
                /* Prevent softlocks if there's no cutscene running right now */
                hascontrol = true;
                completestop = false;
            }
            break;
        case 1:
            //Game initilisation
            setstate(0);
            break;
        case 2:
            //Opening cutscene
            advancetext = true;
            hascontrol = false;
            setstate(3);
            graphics.createtextbox("To do: write quick", 50, 80, TEXT_COLOUR("cyan"));
            graphics.addline("intro to story!");
            graphics.textboxprintflags(PR_FONT_8X8);
            //Oh no! what happen to rest of crew etc crash into dimension
            break;
        case 4:
            //End of opening cutscene for now
            graphics.createtextbox(BUTTONGLYPHS_get_wasd_text(), -1, 195, TEXT_COLOUR("gray"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            graphics.textboxwrap(4);
            graphics.textboxcentertext();
            graphics.textboxpad(2, 2);
            graphics.textboxcenterx();
            graphics.textboxtimer(60);
            setstate(0);
            break;
        case 5:
            //Demo over
            advancetext = true;
            hascontrol = false;

            startscript = true;
            newscript="returntohub";
            obj.removetrigger(5);
            setstate(6);
            break;
        case 7:
            //End of opening cutscene for now
            graphics.textboxremove();
            hascontrol = true;
            advancetext = false;
            setstate(0);
            break;
        case 8:
            //Enter dialogue
            obj.removetrigger(8);
            if (!obj.flags[13])
            {
                obj.flags[13] = true;

                char buffer[SCREEN_WIDTH_CHARS*3 + 1];
                vformat_buf(
                    buffer, sizeof(buffer),
                    loc::gettext("Press {button} to view map and quicksave"),
                    "button:but",
                    vformat_button(ActionSet_InGame, Action_InGame_Map)
                );

                graphics.createtextbox(buffer, -1, 155, TEXT_COLOUR("gray"));
                graphics.textboxprintflags(PR_FONT_INTERFACE);
                graphics.textboxwrap(4);
                graphics.textboxcentertext();
                graphics.textboxpad(2, 2);
                graphics.textboxcenterx();
                graphics.textboxtimer(60);
            }
            setstate(0);
            break;

        case 9:
            if (!map.custommode && nocompetitive())
            {
                returntolab();

                startscript = true;
                newscript = "disableaccessibility";

                setstate(0);
                break;
            }

            //Start SWN Minigame Mode B
            obj.removetrigger(9);

            swnmode = true;
            swngame = SWN_START_SUPERGRAVITRON_STEP_1;
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

            setstate(0);
            break;

        case 10:
            //Start SWN Minigame Mode A
            obj.removetrigger(10);

            swnmode = true;
            swngame = SWN_START_GRAVITRON_STEP_1;
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

            setstate(0);
            break;

        case 11:
        {
            //Intermission 1 instructional textbox, depends on last saved
            graphics.textboxremovefast();
            const char* floorceiling = graphics.flipmode ? "ceiling" : "floor";
            const char* crewmate;
            switch (lastsaved)
            {
            case 2:
                crewmate = "Vitellary";
                break;
            case 3:
                crewmate = "Vermilion";
                break;
            case 4:
                crewmate = "Verdigris";
                break;
            case 5:
                crewmate = "Victoria";
                break;
            default:
                crewmate = "your companion";
            }
            char english[SCREEN_WIDTH_TILES*3 + 1]; /* ASCII only */
            vformat_buf(english, sizeof(english),
                "When you're NOT standing on the {floorceiling}, {crewmate} will stop and wait for you.",
                "floorceiling:str, crewmate:str",
                floorceiling, crewmate
            );
            graphics.createtextbox(loc::gettext(english), -1, 3, TEXT_COLOUR("gray"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            graphics.textboxwrap(2);
            graphics.textboxpadtowidth(36*8);
            graphics.textboxcenterx();
            graphics.textboxtimer(180);
            setstate(0);
            break;
        }
        case 12:
            //Intermission 1 instructional textbox, depends on last saved
            obj.removetrigger(12);
            if (!obj.flags[61])
            {
                obj.flags[61] = true;
                graphics.textboxremovefast();
                const char* english;
                switch (lastsaved)
                {
                case 2:
                case 3:
                case 4:
                    english = "You can't continue to the next room until he is safely across.";
                    break;
                case 5:
                    english = "You can't continue to the next room until she is safely across.";
                    break;
                default:
                    english = "You can't continue to the next room until they are safely across.";
                }
                graphics.createtextbox(loc::gettext(english), -1, 3, TEXT_COLOUR("gray"));
                graphics.textboxprintflags(PR_FONT_INTERFACE);
                graphics.textboxwrap(2);
                graphics.textboxpadtowidth(36*8);
                graphics.textboxcenterx();
                graphics.textboxtimer(120);
            }
            setstate(0);
            break;
        case 13:
            //textbox removal
            obj.removetrigger(13);
            graphics.textboxremovefast();
            setstate(0);
            break;
        case 14:
        {
            //Intermission 1 instructional textbox, depends on last saved
            const char* floorceiling = graphics.flipmode ? "ceiling" : "floor";
            const char* crewmate;
            switch (lastsaved)
            {
            case 2:
                crewmate = "Vitellary";
                break;
            case 3:
                crewmate = "Vermilion";
                break;
            case 4:
                crewmate = "Verdigris";
                break;
            case 5:
                crewmate = "Victoria";
                break;
            default:
                crewmate = "your companion";
            }
            char english[SCREEN_WIDTH_TILES*3 + 1]; /* ASCII only */
            vformat_buf(english, sizeof(english),
                "When you're standing on the {floorceiling}, {crewmate} will try to walk to you.",
                "floorceiling:str, crewmate:str",
                floorceiling, crewmate
            );
            graphics.createtextbox(loc::gettext(english), -1, 3, TEXT_COLOUR("gray"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            graphics.textboxwrap(2);
            graphics.textboxpadtowidth(36*8);
            graphics.textboxcenterx();
            graphics.textboxtimer(280);

            setstate(0);
            break;
        }
        case 15:
        {
            //leaving the naughty corner
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[obj.getplayer()].tile = 0;
            }
            setstate(0);
            break;
        }
        case 16:
        {
            //entering the naughty corner
            int i = obj.getplayer();
            if(INBOUNDS_VEC(i, obj.entities) && obj.entities[i].tile == 0)
            {
                obj.entities[i].tile = 144;
                music.playef(Sound_CRY);
            }
            setstate(0);
            break;
        }

        case 17:
            //Arrow key tutorial
            obj.removetrigger(17);
            if (BUTTONGLYPHS_keyboard_is_active())
            {
                graphics.createtextbox(loc::gettext("If you prefer, you can press UP or DOWN instead of ACTION to flip."), -1, 187, TEXT_COLOUR("gray"));
                graphics.textboxprintflags(PR_FONT_INTERFACE);
                graphics.textboxwrap(2);
                graphics.textboxcentertext();
                graphics.textboxpad(1, 1);
                graphics.textboxcenterx();
                graphics.textboxtimer(100);
            }
            setstate(0);
            break;

        case 20:
            if (!obj.flags[1])
            {
                obj.flags[1] = true;
                setstate(0);
                graphics.textboxremove();
            }
            obj.removetrigger(20);
            break;
        case 21:
            if (!obj.flags[2])
            {
                obj.flags[2] = true;
                setstate(0);
                graphics.textboxremove();
            }
            obj.removetrigger(21);
            break;
        case 22:
            if (!obj.flags[3])
            {
                graphics.textboxremovefast();
                obj.flags[3] = true;
                setstate(0);

                char buffer[SCREEN_WIDTH_CHARS*3 + 1];
                vformat_buf(
                    buffer, sizeof(buffer),
                    loc::gettext("Press {button} to flip"),
                    "button:but",
                    vformat_button(ActionSet_InGame, Action_InGame_ACTION)
                );
                graphics.createtextbox(buffer, -1, 25, TEXT_COLOUR("gray"));
                graphics.textboxprintflags(PR_FONT_INTERFACE);
                graphics.textboxwrap(4);
                graphics.textboxcentertext();
                graphics.textboxpad(2, 2);
                graphics.textboxcenterx();
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
                setstate(0);
            }
            obj.removetrigger(30);
            setstate(0);
            break;
        case 31:
            //state = 55;  setstatedelay(50);
            setstate(0);
            setstatedelay(0);
            if (!obj.flags[6])
            {
                obj.flags[6] = true;

                obj.flags[5] = true;
                startscript = true;
                newscript="communicationstation";
                setstate(0);
                setstatedelay(0);
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
                setstate(0);
            }
            obj.removetrigger(32);
            setstate(0);
            break;
        case 33:
            //Generic "run script"
            if (!obj.flags[9])
            {
                obj.flags[9] = true;
                startscript = true;
                newscript="rescueblue";
                setstate(0);
            }
            obj.removetrigger(33);
            setstate(0);
            break;
        case 34:
            //Generic "run script"
            if (!obj.flags[10])
            {
                obj.flags[10] = true;
                startscript = true;
                newscript="rescueyellow";
                setstate(0);
            }
            obj.removetrigger(34);
            setstate(0);
            break;
        case 35:
            //Generic "run script"
            if (!obj.flags[11])
            {
                obj.flags[11] = true;
                startscript = true;
                newscript="rescuegreen";
                setstate(0);
            }
            obj.removetrigger(35);
            setstate(0);
            break;
        case 36:
            //Generic "run script"
            if (!obj.flags[8])
            {
                obj.flags[8] = true;
                startscript = true;
                newscript="rescuered";
                setstate(0);
            }
            obj.removetrigger(36);
            setstate(0);
            break;

        case 37:
            //Generic "run script"
            if (companion == 0)
            {
                startscript = true;
                newscript="int2_yellow";
                setstate(0);
            }
            obj.removetrigger(37);
            setstate(0);
            break;
        case 38:
            //Generic "run script"
            if (companion == 0)
            {
                startscript = true;
                newscript="int2_red";
                setstate(0);
            }
            obj.removetrigger(38);
            setstate(0);
            break;
        case 39:
            //Generic "run script"
            if (companion == 0)
            {
                startscript = true;
                newscript="int2_green";
                setstate(0);
            }
            obj.removetrigger(39);
            setstate(0);
            break;
        case 40:
            //Generic "run script"
            if (companion == 0)
            {
                startscript = true;
                newscript="int2_blue";
                setstate(0);
            }
            obj.removetrigger(40);
            setstate(0);
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
                setstate(0);
            }
            obj.removetrigger(41);
            setstate(0);
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
                setstate(0);
            }
            obj.removetrigger(42);
            setstate(0);
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
                setstate(0);
            }
            obj.removetrigger(43);
            setstate(0);
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
                setstate(0);
            }
            obj.removetrigger(44);
            setstate(0);
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
                setstate(0);
            }
            obj.removetrigger(45);
            setstate(0);
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
                setstate(0);
            }
            obj.removetrigger(46);
            setstate(0);
            break;

        case 47:
            //Generic "run script"
            if (!obj.flags[69])
            {
                obj.flags[69] = true;
                startscript = true;
                newscript="trenchwarfare";
                setstate(0);
            }
            obj.removetrigger(47);
            setstate(0);
            break;
        case 48:
            //Generic "run script"
            if (!obj.flags[70])
            {
                obj.flags[70] = true;
                startscript = true;
                newscript="trinketcollector";
                setstate(0);
            }
            obj.removetrigger(48);
            setstate(0);
            break;
        case 49:
            //Start final level music
            if (!obj.flags[71])
            {
                obj.flags[71] = true;
                music.niceplay(Music_PREDESTINEDFATEREMIX);
                setstate(0);
            }
            obj.removetrigger(49);
            setstate(0);
            break;

        case 50:
            music.playef(Sound_VIOLET);
            graphics.createtextbox(loc::gettext("Help! Can anyone hear this message?"), 5, 8, TEXT_COLOUR("purple"));
            graphics.textboxcommsrelay();
            graphics.textboxtimer(60);
            incstate();
            setstatedelay(100);
            break;
        case 51:
            music.playef(Sound_VIOLET);
            graphics.createtextbox(loc::gettext("Verdigris? Are you out there? Are you ok?"), 5, 8, TEXT_COLOUR("purple"));
            graphics.textboxcommsrelay();
            graphics.textboxtimer(60);
            incstate();
            setstatedelay(100);
            break;
        case 52:
            music.playef(Sound_VIOLET);
            graphics.createtextbox(loc::gettext("Please help us! We've crashed and need assistance!"), 5, 8, TEXT_COLOUR("purple"));
            graphics.textboxcommsrelay();
            graphics.textboxtimer(60);
            incstate();
            setstatedelay(100);
            break;
        case 53:
            music.playef(Sound_VIOLET);
            graphics.createtextbox(loc::gettext("Hello? Anyone out there?"), 5, 8, TEXT_COLOUR("purple"));
            graphics.textboxcommsrelay();
            graphics.textboxtimer(60);
            incstate();
            setstatedelay(100);
            break;
        case 54:
            music.playef(Sound_VIOLET);
            graphics.createtextbox(loc::gettext("This is Doctor Violet from the D.S.S. Souleye! Please respond!"), 5, 8, TEXT_COLOUR("purple"));
            graphics.textboxcommsrelay();
            graphics.textboxtimer(60);
            incstate();
            setstatedelay(100);
            break;
        case 55:
            music.playef(Sound_VIOLET);
            graphics.createtextbox(loc::gettext("Please... Anyone..."), 5, 8, TEXT_COLOUR("purple"));
            graphics.textboxcommsrelay();
            graphics.textboxtimer(60);
            incstate();
            setstatedelay(100);
            break;
        case 56:
            music.playef(Sound_VIOLET);
            graphics.createtextbox(loc::gettext("Please be alright, everyone..."), 5, 8, TEXT_COLOUR("purple"));
            graphics.textboxcommsrelay();
            graphics.textboxtimer(60);
            setstate(50);
            setstatedelay(100);
            break;


        case 80:
            //Used to return to menu from the game
            if (graphics.fademode == FADE_FULLY_BLACK)
            {
                incstate();
            }
            break;
        case 81:
            quittomenu();
            music.play(Music_PRESENTINGVVVVVV); //should be after quittomenu()
            setstate(0);
            break;

        case 82:
            //Time Trial Complete!
            obj.removetrigger(82);
            if (map.custommode && !map.custommodeforreal)
            {
                returntoeditor();
                ed.show_note(loc::gettext("Time trial completed"));
                break;
            }

            if (translator_exploring)
            {
                translator_exploring_allowtele = true;
                setstate(0);
                break;
            }
            hascontrol = false;

            if (timetrialcheater)
            {
                SDL_zeroa(obj.collect);
            }

            timetrialresulttime = help.hms_to_seconds(hours, minutes, seconds);
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
                if (timetrialrank >= 3)
                {
                    switch (timetriallevel)
                    {
                    case TimeTrial_SPACESTATION1:
                        unlockAchievement("vvvvvvtimetrial_station1_fixed");
                        break;
                    case TimeTrial_LABORATORY:
                        unlockAchievement("vvvvvvtimetrial_lab_fixed");
                        break;
                    case TimeTrial_TOWER:
                        unlockAchievement("vvvvvvtimetrial_tower_fixed");
                        break;
                    case TimeTrial_SPACESTATION2:
                        unlockAchievement("vvvvvvtimetrial_station2_fixed");
                        break;
                    case TimeTrial_WARPZONE:
                        unlockAchievement("vvvvvvtimetrial_warp_fixed");
                        break;
                    case TimeTrial_FINALLEVEL:
                        unlockAchievement("vvvvvvtimetrial_final_fixed");
                    }
                }
            }

            savestatsandsettings();

            graphics.fademode = FADE_START_FADEOUT;
            music.fadeout();
            incstate();
            break;
        case 83:
            frames--;
            if (graphics.fademode == FADE_FULLY_BLACK)
            {
                incstate();
            }
            break;
        case 84:
            quittomenu();
            createmenu(Menu::timetrialcomplete);
            setstate(0);
            break;


        case 85:
            //Cutscene skip version of final level change
            obj.removetrigger(85);
            //Init final stretch
            incstate();
            music.playef(Sound_FLASH);
            music.play(Music_POSITIVEFORCE);
            obj.flags[72] = true;

            screenshake = 10;
            flashlight = 5;
            map.finalstretch = true;
            map.warpx = false;
            map.warpy = false;
            map.background = 6;

            map.final_colormode = true;
            map.final_colorframe = 1;

            setstate(0);
            break;

            //From 90-100 are run scripts for the eurogamer expo only, remove later
        case 90:
            //Generic "run script"
            startscript = true;
            newscript="startexpolevel_station1";
            obj.removetrigger(90);
            setstate(0);
            break;
        case 91:
            //Generic "run script"
            startscript = true;
            newscript="startexpolevel_lab";
            obj.removetrigger(91);
            setstate(0);
            break;
        case 92:
            //Generic "run script"
            startscript = true;
            newscript="startexpolevel_warp";
            obj.removetrigger(92);
            setstate(0);
            break;
        case 93:
            //Generic "run script"
            startscript = true;
            newscript="startexpolevel_tower";
            obj.removetrigger(93);
            setstate(0);
            break;
        case 94:
            //Generic "run script"
            startscript = true;
            newscript="startexpolevel_station2";
            obj.removetrigger(94);
            setstate(0);
            break;
        case 95:
            //Generic "run script"
            startscript = true;
            newscript="startexpolevel_final";
            obj.removetrigger(95);
            setstate(0);
            break;

        case 96:
            //Used to return to gravitron to game
            if (graphics.fademode == FADE_FULLY_BLACK)
            {
                incstate();
            }
            break;
        case 97:
            returntolab();
            setstate(0);
            break;

        case 100:
            //
            //                       Meeting crewmate in the warpzone
            //
            obj.removetrigger(100);
            if (!obj.flags[4])
            {
                obj.flags[4] = true;
                incstate();
            }
            break;
        case 101:
        {


            int i = obj.getplayer();
            hascontrol = false;
            if (INBOUNDS_VEC(i, obj.entities) && obj.entities[i].onroof > 0 && gravitycontrol == 1)
            {
                gravitycontrol = 0;
                music.playef(Sound_UNFLIP);
            }
            if (INBOUNDS_VEC(i, obj.entities) && obj.entities[i].onground > 0)
            {
                incstate();
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
            graphics.textboxprintflags(PR_FONT_8X8);
            incstate();
            music.playef(Sound_VERDIGRIS);
        }
        break;
        case 104:
            graphics.createtextbox("I'm glad you're ok!", 135, 152, TEXT_COLOUR("cyan"));
            graphics.textboxprintflags(PR_FONT_8X8);
            incstate();
            music.playef(Sound_VIRIDIAN);
            graphics.textboxactive();
            break;
        case 106:
        {
            graphics.createtextbox("I've been trying to find a", 74, 70, 164, 255, 164);
            graphics.addline("way out, but I keep going");
            graphics.addline("around in circles...");
            graphics.textboxprintflags(PR_FONT_8X8);
            incstate();
            music.playef(Sound_CRY);
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
            graphics.createtextbox("Don't worry! I have a", 125, 152, TEXT_COLOUR("cyan"));
            graphics.addline("teleporter key!");
            graphics.textboxprintflags(PR_FONT_8X8);
            incstate();
            music.playef(Sound_VIRIDIAN);
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
            graphics.createtextbox("Follow me!", 185, 154, TEXT_COLOUR("cyan"));
            graphics.textboxprintflags(PR_FONT_8X8);
            incstate();
            music.playef(Sound_VIRIDIAN);
            graphics.textboxactive();

        }
        break;
        case 112:
            graphics.textboxremove();
            hascontrol = true;
            advancetext = false;

            setstate(0);
            break;

        case 115:
            //
            //                       Test script for space station, totally delete me!
            //
            hascontrol = false;
            incstate();
        break;
        case 116:
            advancetext = true;
            hascontrol = false;

            graphics.createtextbox("Sorry Eurogamers! Teleporting around", 60 - 20, 200, 255, 64, 64);
            graphics.addline("the map doesn't work in this version!");
            graphics.textboxprintflags(PR_FONT_8X8);
            graphics.textboxcenterx();
            incstate();
            break;
        case 118:
            graphics.textboxremove();
            hascontrol = true;
            advancetext = false;

            setstate(0);
            break;

        case 120:
            //
            //                       Meeting crewmate in the space station
            //
            obj.removetrigger(120);
            if (!obj.flags[5])
            {
                obj.flags[5] = true;
                incstate();
            }
            break;
        case 121:
        {

            int i = obj.getplayer();
            hascontrol = false;
            if (INBOUNDS_VEC(i, obj.entities) && obj.entities[i].onground > 0 && gravitycontrol == 0)
            {
                gravitycontrol = 1;
                music.playef(Sound_UNFLIP);
            }
            if (INBOUNDS_VEC(i, obj.entities) && obj.entities[i].onroof > 0)
            {
                incstate();
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

            graphics.createtextbox("Captain! You're ok!", 60-10, 90-40, TEXT_COLOUR("yellow"));
            graphics.textboxprintflags(PR_FONT_8X8);
            incstate();
            music.playef(Sound_VITELLARY);
            break;
        }
        case 124:
        {
            graphics.createtextbox("I've found a teleporter, but", 60-20, 90 - 40, TEXT_COLOUR("yellow"));
            graphics.addline("I can't get it to go anywhere...");
            graphics.textboxprintflags(PR_FONT_8X8);
            incstate();
            music.playef(Sound_CRY);
            graphics.textboxactive();
            break;
        }
        case 126:
            graphics.createtextbox("I can help with that!", 125, 152-40, TEXT_COLOUR("cyan"));
            graphics.textboxprintflags(PR_FONT_8X8);
            incstate();
            music.playef(Sound_VIRIDIAN);
            graphics.textboxactive();
            break;
        case 128:
            graphics.createtextbox("I have the teleporter", 130, 152-35, TEXT_COLOUR("cyan"));
            graphics.addline("codex for our ship!");
            graphics.textboxprintflags(PR_FONT_8X8);
            incstate();
            music.playef(Sound_VIRIDIAN);
            graphics.textboxactive();
            break;

        case 130:
        {
            graphics.createtextbox("Yey! Let's go home!", 60-30, 90-35, TEXT_COLOUR("yellow"));
            graphics.textboxprintflags(PR_FONT_8X8);
            incstate();
            music.playef(Sound_VITELLARY);
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

            setstate(0);
            break;

        case 200:
            //Init final stretch
            incstate();
            music.playef(Sound_FLASH);
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
            setstate(0);
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
            setstate(0);
            break;

        case 1000:
            graphics.showcutscenebars = true;
            hascontrol = false;
            completestop = true;
            incstate();
            setstatedelay(15);
            break;
        case 1001:
        {
            //Found a trinket!
            advancetext = true;
            incstate();
            graphics.createtextboxflipme(loc::gettext("Congratulations!\n\nYou have found a shiny trinket!"), 50, 85, TEXT_COLOUR("gray"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            int h = graphics.textboxwrap(2);
            graphics.textboxcentertext();
            graphics.textboxpad(1, 1);
            graphics.textboxcenterx();

            int max_trinkets;

            if(map.custommode)
            {
                max_trinkets = cl.numtrinkets();
            }
            else
            {
                max_trinkets = 20;
            }

            char buffer[SCREEN_WIDTH_CHARS + 1];
            vformat_buf(
                buffer, sizeof(buffer),
                loc::gettext("{n_trinkets|wordy} out of {max_trinkets|wordy}"),
                "n_trinkets:int, max_trinkets:int",
                trinkets(), max_trinkets
            );
            graphics.createtextboxflipme(buffer, 50, 95+h, TEXT_COLOUR("gray"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            graphics.textboxwrap(2);
            graphics.textboxcentertext();
            graphics.textboxpad(1, 1);
            graphics.textboxcenterx();
            break;
        }
        case 1002:
            if (!advancetext)
            {
                // Prevent softlocks if we somehow don't have advancetext
                incstate();
            }
            break;
        case 1003:
            graphics.textboxremove();
            hascontrol = true;
            advancetext = false;
            completestop = false;
            setstate(0);
            if (music.currentsong > -1)
            {
                music.fadeMusicVolumeIn(3000);
            }
            graphics.showcutscenebars = false;
            break;

        case 1010:
            graphics.showcutscenebars = true;
            hascontrol = false;
            completestop = true;
            incstate();
            setstatedelay(15);
            break;
        case 1011:
        {
            //Found a crewmate!
            advancetext = true;
            incstate();
            graphics.createtextboxflipme(loc::gettext("Congratulations!\n\nYou have found a lost crewmate!"), 50, 85, TEXT_COLOUR("gray"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            int h = graphics.textboxwrap(2);
            graphics.textboxcentertext();
            graphics.textboxpad(1, 1);
            graphics.textboxcenterx();

            if(cl.numcrewmates()-crewmates()==0)
            {
                graphics.createtextboxflipme(loc::gettext("All crewmates rescued!"), 50, 95+h, TEXT_COLOUR("gray"));
            }
            else
            {
                char buffer[SCREEN_WIDTH_CHARS + 1];
                loc::gettext_plural_fill(
                    buffer, sizeof(buffer),
                    "{n_crew|wordy} remain", "{n_crew|wordy} remains",
                    "n_crew:int",
                    cl.numcrewmates()-crewmates()
                );
                graphics.createtextboxflipme(buffer, 50, 95+h, TEXT_COLOUR("gray"));
            }
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            graphics.textboxwrap(4);
            graphics.textboxcentertext();
            graphics.textboxpad(2, 2);
            graphics.textboxcenterx();
            break;
        }
        case 1012:
            if (!advancetext)
            {
                // Prevent softlocks if we somehow don't have advancetext
                incstate();
            }
            break;
        case 1013:
            graphics.textboxremove();
            hascontrol = true;
            advancetext = false;
            completestop = false;
            setstate(0);

            if(cl.numcrewmates()-crewmates()==0)
            {
                if(map.custommodeforreal)
                {
                    graphics.fademode = FADE_START_FADEOUT;
                    setstate(1014);
                }
                else
                {
                    returntoeditor();
                    ed.show_note(loc::gettext("Level completed"));
                }
            }
            else
            {
                if (cl.levmusic > 0)
                {
                    music.fadeMusicVolumeIn(3000);
                }
            }
            graphics.showcutscenebars = false;
            break;
        case 1014:
            frames--;
            if (graphics.fademode == FADE_FULLY_BLACK)
            {
                incstate();
            }
            break;
        case 1015:
            //Update level stats
            /* FIXME: Have to add check to not save stats for the dumb hack
             * `special/stdin.vvvvvv` filename... see elsewhere, grep for
             * `special/stdin`! */
            if(cl.numcrewmates()-crewmates()==0 && customlevelfilename != "levels/special/stdin.vvvvvv")
            {
                //Finished level
                if (trinkets() >= cl.numtrinkets())
                {
                    //and got all the trinkets!
                    updatecustomlevelstats(customlevelfilename, 3);
                }
                else
                {
                    updatecustomlevelstats(customlevelfilename, 1);
                }
            }

            quittomenu();
            music.play(Music_PRESENTINGVVVVVV); //should be after quittomenu()
            setstate(0);
            break;


        case 2000:
            //Game Saved!
            savetele_textbox();
            setstate(0);
            break;

        case 2500:

            music.play(Music_PAUSE);
            //Activating a teleporter (appear)
            incstate();
            setstatedelay(15);
            flashlight = 5;
            screenshake = 90;
            music.playef(Sound_FLASH);
            break;
        case 2501:
            //Activating a teleporter 2
            incstate();
            setstatedelay(0);
            flashlight = 5;
            screenshake = 0;
            //we're done here!
            music.playef(Sound_TELEPORT);
            break;
        case 2502:
        {
            //Activating a teleporter 2
            incstate();
            setstatedelay(5);

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
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 2504:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                //obj.entities[i].xp += 10;
            }
            break;
        }
        case 2505:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 8;
            }
            break;
        }
        case 2506:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 6;
            }
            break;
        }
        case 2507:
        {
            incstate();
            break;
        }
        case 2508:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 2;
            }
            break;
        }
        case 2509:
        {
            incstate();
            setstatedelay(15);
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
            graphics.createtextbox("Hello?", 125+24, 152-20, TEXT_COLOUR("cyan"));
            graphics.textboxprintflags(PR_FONT_8X8);
            incstate();
            music.playef(Sound_VIRIDIAN);
            graphics.textboxactive();
            break;
        case 2512:
            advancetext = true;
            hascontrol = false;
            graphics.createtextbox("Is anyone there?", 125+8, 152-24, TEXT_COLOUR("cyan"));
            graphics.textboxprintflags(PR_FONT_8X8);
            incstate();
            music.playef(Sound_VIRIDIAN);
            graphics.textboxactive();
            break;
        case 2514:
            graphics.textboxremove();
            hascontrol = true;
            advancetext = false;

            setstate(0);
            music.play(Music_POTENTIALFORANYTHING);
            break;


        case 3000:
            //Activating a teleporter (long version for level complete)
            incstate();
            setstatedelay(30);
            flashlight = 5;
            screenshake = 90;
            music.playef(Sound_FLASH);
            break;
        case 3001:
            //Activating a teleporter 2
            incstate();
            setstatedelay(15);
            flashlight = 5;
            music.playef(Sound_FLASH);
            break;
        case 3002:
            //Activating a teleporter 2
            incstate();
            setstatedelay(15);
            flashlight = 5;
            music.playef(Sound_FLASH);
            break;
        case 3003:
            //Activating a teleporter 2
            incstate();
            setstatedelay(15);
            flashlight = 5;
            music.playef(Sound_FLASH);
            break;
        case 3004:
            //Activating a teleporter 2
            incstate();
            setstatedelay(0);
            flashlight = 5;
            screenshake = 0;
            //we're done here!
            music.playef(Sound_TELEPORT);
            break;
        case 3005:
        {
            //Activating a teleporter 2
            incstate();
            setstatedelay(50);
            switch(companion)
            {
            case 6:
                setstate(3006);
                break; //Warp Zone
            case 7:
                setstate(3020);
                break; //Space Station
            case 8:
                setstate(3040);
                break; //Lab
            case 9:
                setstate(3060);
                break; //Tower
            case 10:
                setstate(3080);
                break; //Intermission 2
            case 11:
                setstate(3085);
                break; //Intermission 1
            }

            if (translator_exploring_allowtele)
            {
                setstate(3090);
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
            unlocknum(Unlock_WARPZONE_COMPLETE);
            lastsaved = 4;
            music.play(Music_PATHCOMPLETE);
            incstate();
            setstatedelay(75);

            levelcomplete_textbox();
            break;
        case 3007:
            incstate();
            setstatedelay(45);

            crewmate_textbox(175, 174, 174);
            break;
        case 3008:
            incstate();
            setstatedelay(45);

            remaining_textbox();
            break;
        case 3009:
            incstate();
            setstatedelay(0);

            actionprompt_textbox();
            break;
        case 3010:
            if (jumppressed)
            {
                incstate();
                setstatedelay(30);
                graphics.textboxremove();
            }
            break;
        case 3011:
            setstate(3070);
            setstatedelay(0);
            break;

        case 3020:
            //Level complete! (Space Station 2)
            unlocknum(Unlock_SPACESTATION2_COMPLETE);
            lastsaved = 2;
            music.play(Music_PATHCOMPLETE);
            incstate();
            setstatedelay(75);


            levelcomplete_textbox();
            break;
        case 3021:
            incstate();
            setstatedelay(45);

            crewmate_textbox(174, 175, 174);
            break;
        case 3022:
            incstate();
            setstatedelay(45);

            remaining_textbox();
            break;
        case 3023:
            incstate();
            setstatedelay(0);

            actionprompt_textbox();
            break;
        case 3024:
            if (jumppressed)
            {
                incstate();
                setstatedelay(30);
                graphics.textboxremove();
            }
            break;
        case 3025:
            setstate(3070);
            setstatedelay(0);
            break;

        case 3040:
            //Level complete! (Lab)
            unlocknum(Unlock_LABORATORY_COMPLETE);
            lastsaved = 5;
            music.play(Music_PATHCOMPLETE);
            incstate();
            setstatedelay(75);

            levelcomplete_textbox();
            break;
        case 3041:
            incstate();
            setstatedelay(45);

            crewmate_textbox(174, 174, 175);
            break;
        case 3042:
            incstate();
            setstatedelay(45);

            remaining_textbox();
            break;
        case 3043:
            incstate();
            setstatedelay(0);

            actionprompt_textbox();
            break;
        case 3044:
            if (jumppressed)
            {
                incstate();
                setstatedelay(30);
                graphics.textboxremove();
            }
            break;
        case 3045:
            setstate(3070);
            setstatedelay(0);
            break;

        case 3050:
            //Level complete! (Space Station 1)
            unlocknum(Unlock_SPACESTATION1_COMPLETE);
            lastsaved = 1;
            music.play(Music_PATHCOMPLETE);
            incstate();
            setstatedelay(75);


            levelcomplete_textbox();
            break;
        case 3051:
            incstate();
            setstatedelay(45);

            crewmate_textbox(175, 175, 174);
            break;
        case 3052:
            incstate();
            setstatedelay(45);

            remaining_textbox();
            break;
        case 3053:
            incstate();
            setstatedelay(0);

            actionprompt_textbox();
            break;
        case 3054:
            if (jumppressed)
            {
                incstate();
                setstatedelay(30);
                graphics.textboxremove();
                teleportscript = "";
            }
            break;
        case 3055:
            graphics.fademode = FADE_START_FADEOUT;
            incstate();
            setstatedelay(10);
            break;
        case 3056:
            if (graphics.fademode == FADE_FULLY_BLACK)
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
                setstate(0);
            }
            break;


        case 3060:
            //Level complete! (Tower)
            unlocknum(Unlock_TOWER_COMPLETE);
            lastsaved = 3;
            music.play(Music_PATHCOMPLETE);
            incstate();
            setstatedelay(75);

            levelcomplete_textbox();
            break;
        case 3061:
            incstate();
            setstatedelay(45);

            crewmate_textbox(175, 174, 175);
            break;
        case 3062:
            incstate();
            setstatedelay(45);

            remaining_textbox();
            break;
        case 3063:
            incstate();
            setstatedelay(0);

            actionprompt_textbox();
            break;
        case 3064:
            if (jumppressed)
            {
                incstate();
                setstatedelay(30);
                graphics.textboxremove();
            }
            break;
        case 3065:
            setstate(3070);
            setstatedelay(0);
            break;


        case 3070:
            graphics.fademode = FADE_START_FADEOUT;
            incstate();
            break;
        case 3071:
            if (graphics.fademode == FADE_FULLY_BLACK)
            {
                incstate();
            }
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
                setstate(0);
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
                setstate(0);
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
                setstate(0);
            }
            else
            {
                startscript = true;
                newscript="regularreturn";
                setstate(0);
            }
            break;

        case 3080:
            //returning from an intermission, very like 3070
            if (inintermission)
            {
                graphics.fademode = FADE_START_FADEOUT;
                companion = 0;
                setstate(3100);
            }
            else
            {
                unlocknum(Unlock_INTERMISSION2_COMPLETE);
                graphics.fademode = FADE_START_FADEOUT;
                companion = 0;
                incstate();
            }
            break;
        case 3081:
            if (graphics.fademode == FADE_FULLY_BLACK)
            {
                incstate();
            }
            break;
        case 3082:
            map.finalmode = false;
            startscript = true;
            newscript="regularreturn";
            setstate(0);
            break;

        case 3085:
            //returning from an intermission, very like 3070
            //return to menu from here
            if (inintermission)
            {
                companion = 0;
                supercrewmate = false;
                incstate();
                graphics.fademode = FADE_START_FADEOUT;
                music.fadeout();
                setstate(3100);
            }
            else
            {
                unlocknum(Unlock_INTERMISSION1_COMPLETE);
                graphics.fademode = FADE_START_FADEOUT;
                companion = 0;
                supercrewmate = false;
                incstate();
            }
            break;
        case 3086:
            if (graphics.fademode == FADE_FULLY_BLACK)
            {
                incstate();
            }
            break;
        case 3087:
            map.finalmode = false;
            startscript = true;
            newscript="regularreturn";
            setstate(0);
            break;

        case 3090:
            /* Teleporting in translator_exploring should be just like
             * the intermission replays: simply return to the menu */
            companion = 0;
            supercrewmate = false;
            graphics.fademode = FADE_START_FADEOUT;
            music.fadeout();
            setstate(3100);
            break;
        case 3091:
            /* Different Final Level ending for translator_exploring */
            music.fadeout();
            incstate();
            setstatedelay(60);
            break;
        case 3092:
            graphics.fademode = FADE_START_FADEOUT;
            setstate(3100);
            break;

        case 3100:
            if (graphics.fademode == FADE_FULLY_BLACK)
            {
                incstate();
            }
            break;
        case 3101:
            quittomenu();
            music.play(Music_PRESENTINGVVVVVV); //should be after quittomenu();
            setstate(0);
            break;

        case 3500:
            music.fadeout();
            incstate();
            setstatedelay(120);
            break;
        case 3501:
            //Game complete!
            unlockAchievement("vvvvvvgamecomplete");
            unlocknum(UnlockTrophy_GAME_COMPLETE);
            crewstats[0] = true;
            incstate();
            setstatedelay(75);
            music.play(Music_PLENARY);

            graphics.createtextboxflipme("", -1, 12, 164, 165, 255);
            graphics.addline("                                    ");
            graphics.addline("");
            graphics.addline("");
            graphics.textboxprintflags(PR_FONT_8X8);
            graphics.textboxcenterx();
            break;
        case 3502:
        {
            incstate();
            setstatedelay(45+15);

            graphics.createtextboxflipme(loc::gettext("All Crew Members Rescued!"), -1, 64, TEXT_COLOUR("transparent"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            char buffer[SCREEN_WIDTH_CHARS + 1];
            timestringcenti(buffer, sizeof(buffer));
            savetime = buffer;
            break;
        }
        case 3503:
        {
            incstate();
            setstatedelay(45);

            const char* label = loc::gettext("Trinkets Found:");
            char buffer[SCREEN_WIDTH_CHARS + 1];
            vformat_buf(buffer, sizeof(buffer),
                loc::gettext("{gamecomplete_n_trinkets|wordy}"),
                "gamecomplete_n_trinkets:int",
                trinkets()
            );
            graphics.createtextboxflipme(label, 170-font::len(PR_FONT_INTERFACE, label), 84, TEXT_COLOUR("transparent"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            graphics.createtextboxflipme(buffer, 180, 84, TEXT_COLOUR("transparent"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            break;
        }
        case 3504:
        {
            incstate();
            setstatedelay(45+15);

            const char* label = loc::gettext("Game Time:");
            std::string tempstring = savetime;
            graphics.createtextboxflipme(label, 170-font::len(PR_FONT_INTERFACE, label), 96, TEXT_COLOUR("transparent"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            graphics.createtextboxflipme(tempstring, 180, 96, TEXT_COLOUR("transparent"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            break;
        }
        case 3505:
        {
            incstate();
            setstatedelay(45);

            const char* label = loc::gettext("Total Flips:");
            graphics.createtextboxflipme(label, 170-font::len(PR_FONT_INTERFACE, label), 123, TEXT_COLOUR("transparent"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            graphics.createtextboxflipme(help.String(totalflips), 180, 123, TEXT_COLOUR("transparent"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            break;
        }
        case 3506:
        {
            incstate();
            setstatedelay(45+15);

            const char* label = loc::gettext("Total Deaths:");
            graphics.createtextboxflipme(label, 170-font::len(PR_FONT_INTERFACE, label), 135, TEXT_COLOUR("transparent"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            graphics.createtextboxflipme(help.String(deathcounts), 180, 135, TEXT_COLOUR("transparent"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            break;
        }
        case 3507:
        {
            incstate();
            setstatedelay(45+15);

            char buffer[SCREEN_WIDTH_CHARS + 1];
            loc::gettext_plural_fill(
                buffer, sizeof(buffer),
                "Hardest Room (with {n_deaths} deaths)",
                "Hardest Room (with {n_deaths} death)",
                "n_deaths:int",
                hardestroomdeaths
            );
            graphics.createtextboxflipme(buffer, -1, 158, TEXT_COLOUR("transparent"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            graphics.createtextboxflipme(hardestroom, -1, 170, TEXT_COLOUR("transparent"));
            graphics.textboxprintflags(PR_FONT_INTERFACE);
            break;
        }
        case 3508:
            incstate();
            setstatedelay(0);

            actionprompt_textbox();
            break;
        case 3509:
            if (jumppressed)
            {
                incstate();
                setstatedelay(30);
                graphics.textboxremove();
            }
            break;
        case 3510:
            //Save stats and stuff here
            if (!obj.flags[73])
            {
                //flip mode complete
                unlockAchievement("vvvvvvgamecompleteflip");
                unlocknum(UnlockTrophy_FLIPMODE_COMPLETE);
            }

#ifndef MAKEANDPLAY
            if (!map.custommode)
            {
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
            }
#endif

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


            if (nodeathmode)
            {
                unlockAchievement("vvvvvvmaster"); //bloody hell
                unlocknum(UnlockTrophy_NODEATHMODE_COMPLETE);
                setstate(3520);
                setstatedelay(0);
            }
            else
            {
                setstatedelay(120);
                incstate();
            }

            savestatsandsettings();
            break;
        case 3511:
        {
            //Activating a teleporter (long version for level complete)
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].colour = 102;
            }

            incstate();
            setstatedelay(30);
            flashlight = 5;
            screenshake = 90;
            music.playef(Sound_FLASH);
            break;
        }
        case 3512:
            //Activating a teleporter 2
            incstate();
            setstatedelay(15);
            flashlight = 5;
            music.playef(Sound_FLASH);
            break;
        case 3513:
            //Activating a teleporter 2
            incstate();
            setstatedelay(15);
            flashlight = 5;
            music.playef(Sound_FLASH);
            break;
        case 3514:
            //Activating a teleporter 2
            incstate();
            setstatedelay(15);
            flashlight = 5;
            music.playef(Sound_FLASH);
            break;
        case 3515:
        {
            //Activating a teleporter 2
            incstate();
            setstatedelay(0);
            flashlight = 5;
            screenshake = 0;

            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].colour = 0;
                obj.entities[i].invis = true;
            }

            //we're done here!
            music.playef(Sound_TELEPORT);
            setstatedelay(60);
            break;
        }
        case 3516:
            graphics.fademode = FADE_START_FADEOUT;
            incstate();
            break;
        case 3517:
            if (graphics.fademode == FADE_FULLY_BLACK)
            {
                incstate();
                setstatedelay(30);
            }
            break;
        case 3518:
            graphics.fademode = FADE_START_FADEIN;
            setstate(0);
            setstatedelay(30);

            map.finalmode = false;
            map.final_colormode = false;
            map.final_mapcol = 0;
            map.final_colorframe = 0;
            map.finalstretch = false;
            obj.flags[72] = false;

            graphics.setbars(320);

            teleport_to_new_area = true;
            teleportscript = "gamecomplete";
            break;

        case 3520:
            //NO DEATH MODE COMPLETE JESUS
            hascontrol = false;
            crewstats[0] = true;

            graphics.fademode = FADE_START_FADEOUT;
            incstate();
            break;
        case 3521:
            if (graphics.fademode == FADE_FULLY_BLACK)
            {
                incstate();
            }
            break;
        case 3522:
            copyndmresults();
            quittomenu();
            createmenu(Menu::nodeathmodecomplete);
            setstate(0);
            break;

        case 4000:
            //Activating a teleporter (short version)
            state++; // Increment manually -- gamestate modification might be locked at this point
            statedelay = 10;
            flashlight = 5;
            screenshake = 10;
            music.playef(Sound_FLASH);
            break;
        case 4001:
            //Activating a teleporter 2
            state++;
            statedelay = 0;
            flashlight = 5;
            screenshake = 0;
            //we're done here!
            music.playef(Sound_TELEPORT);
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
            unlockstate();
            break;

        case 4010:
            //Activating a teleporter (default appear)
            incstate();
            setstatedelay(15);
            flashlight = 5;
            screenshake = 90;
            music.playef(Sound_FLASH);
            break;
        case 4011:
            //Activating a teleporter 2
            incstate();
            setstatedelay(0);
            flashlight = 5;
            screenshake = 0;
            music.playef(Sound_TELEPORT);
            break;
        case 4012:
        {
            //Activating a teleporter 2
            incstate();
            setstatedelay(5);

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
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4014:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4015:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 8;
            }
            break;
        }
        case 4016:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 6;
            }
            break;
        }
        case 4017:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 3;
            }
            break;
        }
        case 4018:
        {
            incstate();
            setstatedelay(15);
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
            setstate(0);
            break;
        }

        case 4020:
            //Activating a teleporter (default appear)
            incstate();
            setstatedelay(15);
            flashlight = 5;
            screenshake = 90;
            music.playef(Sound_FLASH);
            break;
        case 4021:
            //Activating a teleporter 2
            incstate();
            setstatedelay(0);
            flashlight = 5;
            screenshake = 0;
            music.playef(Sound_TELEPORT);
            break;
        case 4022:
        {
            //Activating a teleporter 2
            incstate();
            setstatedelay(5);

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
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 12;
            }
            break;
        }
        case 4024:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 12;
            }
            break;
        }
        case 4025:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4026:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 8;
            }
            break;
        }
        case 4027:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 5;
            }
            break;
        }
        case 4028:
        {
            incstate();
            setstatedelay(15);
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
            setstate(0);
            break;

        case 4030:
            //Activating a teleporter (default appear)
            incstate();
            setstatedelay(15);
            flashlight = 5;
            screenshake = 90;
            music.playef(Sound_FLASH);
            break;
        case 4031:
            //Activating a teleporter 2
            incstate();
            setstatedelay(0);
            flashlight = 5;
            screenshake = 0;
            music.playef(Sound_TELEPORT);
            break;
        case 4032:
        {
            //Activating a teleporter 2
            incstate();
            setstatedelay(5);

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
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 12;
            }
            break;
        }
        case 4034:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 12;
            }
            break;
        }
        case 4035:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 10;
            }
            break;
        }
        case 4036:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 8;
            }
            break;
        }
        case 4037:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 5;
            }
            break;
        }
        case 4038:
        {
            incstate();
            setstatedelay(15);
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
            setstate(0);
            break;

        case 4040:
            //Activating a teleporter (default appear)
            incstate();
            setstatedelay(15);
            flashlight = 5;
            screenshake = 90;
            music.playef(Sound_FLASH);
            break;
        case 4041:
            //Activating a teleporter 2
            incstate();
            setstatedelay(0);
            flashlight = 5;
            screenshake = 0;
            music.playef(Sound_TELEPORT);
            break;
        case 4042:
        {
            //Activating a teleporter 2
            incstate();
            setstatedelay(5);

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
            incstate();
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
            incstate();
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
            incstate();
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
            incstate();
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
            incstate();
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
            incstate();
            setstatedelay(15);
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
            setstate(0);
            break;

        case 4050:
            //Activating a teleporter (default appear)
            incstate();
            setstatedelay(15);
            flashlight = 5;
            screenshake = 90;
            music.playef(Sound_FLASH);
            break;
        case 4051:
            //Activating a teleporter 2
            incstate();
            setstatedelay(0);
            flashlight = 5;
            screenshake = 0;
            music.playef(Sound_TELEPORT);
            break;
        case 4052:
        {
            //Activating a teleporter 2
            incstate();
            setstatedelay(5);

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
            incstate();
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
            incstate();
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
            incstate();
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
            incstate();
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
            incstate();
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
            incstate();
            setstatedelay(15);
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
            setstate(0);
            break;

        case 4060:
            //Activating a teleporter (default appear)
            incstate();
            setstatedelay(15);
            flashlight = 5;
            screenshake = 90;
            music.playef(Sound_FLASH);
            break;
        case 4061:
            //Activating a teleporter 2
            incstate();
            setstatedelay(0);
            flashlight = 5;
            screenshake = 0;
            music.playef(Sound_TELEPORT);
            break;
        case 4062:
        {
            //Activating a teleporter 2
            incstate();
            setstatedelay(5);

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
            incstate();
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
            incstate();
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
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 25;
            }
            break;
        }
        case 4066:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 25;
            }
            break;
        }
        case 4067:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp -= 20;
            }
            break;
        }
        case 4068:
        {
            incstate();
            setstatedelay(15);
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
            setstate(0);
            break;


        case 4070:
            //Activating a teleporter (special for final script, player has colour changed to match rescued crewmate)
            incstate();
            setstatedelay(15);
            flashlight = 5;
            screenshake = 90;
            music.playef(Sound_FLASH);
            break;
        case 4071:
            //Activating a teleporter 2
            incstate();
            setstatedelay(0);
            flashlight = 5;
            screenshake = 0;
            music.playef(Sound_TELEPORT);
            break;
        case 4072:
        {
            //Activating a teleporter 2
            incstate();
            setstatedelay(5);

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
                obj.entities[i].colour = graphics.crewcolour(lastsaved);

                obj.entities[i].ay = -6;
                obj.entities[i].ax = 6;
                obj.entities[i].vy = -6;
                obj.entities[i].vx = 6;
            }
            break;
        }
        case 4073:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4074:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4075:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 8;
            }
            break;
        }
        case 4076:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 6;
            }
            break;
        }
        case 4077:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 3;
            }
            break;
        }
        case 4078:
        {
            incstate();
            setstatedelay(15);
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 1;
            }
            break;
        }
        case 4079:
            setstate(0);
            startscript = true;
            newscript = "finallevel_teleporter";
            break;

        case 4080:
            //Activating a teleporter (default appear)
            incstate();
            setstatedelay(15);
            flashlight = 5;
            screenshake = 90;
            music.playef(Sound_FLASH);
            break;
        case 4081:
            //Activating a teleporter 2
            incstate();
            setstatedelay(0);
            flashlight = 5;
            screenshake = 0;
            music.playef(Sound_TELEPORT);
            break;
        case 4082:
        {
            //Activating a teleporter 2
            incstate();
            setstatedelay(5);

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
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4084:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4085:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 8;
            }
            break;
        }
        case 4086:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 6;
            }
            break;
        }
        case 4087:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 3;
            }
            break;
        }
        case 4088:
        {
            incstate();
            setstatedelay(15);
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
            setstate(0);
            break;

        case 4090:
            //Activating a teleporter (default appear)
            incstate();
            setstatedelay(15);
            flashlight = 5;
            screenshake = 90;
            music.playef(Sound_FLASH);
            break;
        case 4091:
            //Activating a teleporter 2
            incstate();
            setstatedelay(0);
            flashlight = 5;
            screenshake = 0;
            music.playef(Sound_TELEPORT);
            break;
        case 4092:
        {
            //Activating a teleporter 2
            incstate();
            setstatedelay(5);

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
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4094:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 10;
            }
            break;
        }
        case 4095:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 8;
            }
            break;
        }
        case 4096:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 6;
            }
            break;
        }
        case 4097:
        {
            incstate();
            int i = obj.getplayer();
            if (INBOUNDS_VEC(i, obj.entities))
            {
                obj.entities[i].xp += 3;
            }
            break;
        }
        case 4098:
        {
            incstate();
            setstatedelay(15);
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
            setstate(0);
            break;
        }
    }
}

void Game::gethardestroom(void)
{
    if (currentroomdeaths > hardestroomdeaths)
    {
        hardestroomdeaths = currentroomdeaths;

        if (map.roomname[0] == '\0')
        {
            hardestroom = loc::gettext_roomname_special(map.hiddenname);
        }
        else
        {
            hardestroom = loc::gettext_roomname(map.custommode, roomx, roomy, map.roomname, map.roomname_special);
        }
    }
}

void Game::deletestats(void)
{
    if (!FILESYSTEM_delete("saves/unlock.vvv"))
    {
        vlog_error("Error deleting saves/unlock.vvv");
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
        swnrecord = 0;
        swnbestrank = 0;
        bestgamedeaths = -1;
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
        vlog_error("Error deleting saves/settings.vvv");
    }
}

void Game::unlocknum( int t )
{
#ifdef MAKEANDPLAY
    UNUSED(t);
#else
    if (map.custommode)
    {
        //Don't let custom levels unlock things!
        return;
    }

    unlock[t] = true;
    savestatsandsettings();
#endif
}

static bool stats_loaded = false;

void Game::loadstats(struct ScreenSettings* screen_settings)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;
    tinyxml2::XMLElement* dataNode;

    stats_loaded = true;

    if (!FILESYSTEM_loadTiXml2Document("saves/unlock.vvv", doc))
    {
        // Save unlock.vvv only. Maybe we have a settings.vvv laying around too,
        // and we don't want to overwrite that!
        savestats(screen_settings);
        return;
    }

    if (doc.Error())
    {
        vlog_error("Error parsing unlock.vvv: %s", doc.ErrorStr());
        return;
    }

    dataNode = hDoc
        .FirstChildElement()
        .FirstChildElement("Data")
        .FirstChildElement()
        .ToElement();

    for (pElem = dataNode; pElem != NULL; pElem=pElem->NextSiblingElement())
    {
        const char* pKey = pElem->Value();
        const char* pText = pElem->GetText() ;

        if (pText == NULL)
        {
            pText = "";
        }

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

void Game::deserializesettings(tinyxml2::XMLElement* dataNode, struct ScreenSettings* screen_settings)
{
    // Don't duplicate controller buttons!
    controllerButton_flip.clear();
    controllerButton_map.clear();
    controllerButton_esc.clear();
    controllerButton_restart.clear();
    controllerButton_interact.clear();

    for (tinyxml2::XMLElement* pElem = dataNode;
    pElem != NULL;
    pElem = pElem->NextSiblingElement())
    {
        const char* pKey = pElem->Value();
        const char* pText = pElem->GetText();

        if (pText == NULL)
        {
            pText = "";
        }

        if (SDL_strcmp(pKey, "fullscreen") == 0)
        {
            screen_settings->fullscreen = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "stretch") == 0)
        {
            screen_settings->scalingMode = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "useLinearFilter") == 0)
        {
            screen_settings->linearFilter = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "window_display") == 0)
        {
            screen_settings->windowDisplay = help.Int(pText);
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

        if (SDL_strcmp(pKey, "disableaudiopause") == 0)
        {
            disableaudiopause = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "over30mode") == 0)
        {
            over30mode = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "inputdelay") == 0)
        {
            inputdelay = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "rumble") == 0)
        {
            rumble = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "glitchrunnermode") == 0)
        {
            GlitchrunnerMode_set(GlitchrunnerMode_string_to_enum(pText));
        }

        if (SDL_strcmp(pKey, "showingametimer") == 0)
        {
            showingametimer = help.Int(pText);
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

        if (SDL_strcmp(pKey, "musicvolume") == 0)
        {
            music.user_music_volume = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "soundvolume") == 0)
        {
            music.user_sound_volume = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "separate_interact") == 0)
        {
            separate_interact = help.Int(pText);
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

        if (SDL_strcmp(pKey, "interactButton") == 0)
        {
            SDL_GameControllerButton newButton;
            if (GetButtonFromString(pText, &newButton))
            {
                controllerButton_interact.push_back(newButton);
            }
        }

        if (SDL_strcmp(pKey, "controllerSensitivity") == 0)
        {
            key.sensitivity = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "lang") == 0)
        {
            loc::lang = std::string(pText);
        }

        if (SDL_strcmp(pKey, "lang_set") == 0)
        {
            loc::lang_set = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "english_sprites") == 0)
        {
            loc::english_sprites = help.Int(pText);
        }

        if (SDL_strcmp(pKey, "new_level_font") == 0)
        {
            loc::new_level_font = std::string(pText);
        }

        if (SDL_strcmp(pKey, "roomname_translator") == 0 && loc::show_translator_menu)
        {
            roomname_translator::set_enabled(help.Int(pText));
        }

    }

    setdefaultcontrollerbuttons();
}

bool Game::savestats(bool sync /*= true*/)
{
    struct ScreenSettings screen_settings;
    SDL_zero(screen_settings);
    gameScreen.GetSettings(&screen_settings);

    return savestats(&screen_settings, sync);
}

bool Game::savestats(const struct ScreenSettings* screen_settings, bool sync /*= true*/)
{
    tinyxml2::XMLDocument doc;
    bool already_exists;

    if (!stats_loaded)
    {
        vlog_warn("Stats not loaded! Not writing unlock.vvv.");
        return false;
    }

    already_exists = FILESYSTEM_loadTiXml2Document("saves/unlock.vvv", doc);
    if (!already_exists)
    {
        vlog_info("No unlock.vvv found. Creating new file");
    }
    else if (doc.Error())
    {
        vlog_error("Error parsing existing unlock.vvv: %s", doc.ErrorStr());
        vlog_info("Creating new unlock.vvv");
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

    return FILESYSTEM_saveTiXml2Document("saves/unlock.vvv", doc, sync);
}

bool Game::savestatsandsettings(void)
{
    const bool stats_saved = savestats(false);

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

void Game::serializesettings(tinyxml2::XMLElement* dataNode, const struct ScreenSettings* screen_settings)
{
    tinyxml2::XMLDocument& doc = xml::get_document(dataNode);

    xml::update_tag(dataNode, "fullscreen", (int) screen_settings->fullscreen);

    xml::update_tag(dataNode, "stretch", screen_settings->scalingMode);

    xml::update_tag(dataNode, "useLinearFilter", (int) screen_settings->linearFilter);

    xml::update_tag(dataNode, "window_display", screen_settings->windowDisplay);

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

    xml::update_tag(dataNode, "disableaudiopause", (int) disableaudiopause);

    xml::update_tag(dataNode, "notextoutline", (int) graphics.notextoutline);

    xml::update_tag(dataNode, "translucentroomname", (int) graphics.translucentroomname);

    xml::update_tag(dataNode, "over30mode", (int) over30mode);

    xml::update_tag(dataNode, "inputdelay", (int) inputdelay);

    xml::update_tag(dataNode, "rumble", (int) rumble);

    xml::update_tag(
        dataNode,
        "glitchrunnermode",
        GlitchrunnerMode_enum_to_string(GlitchrunnerMode_get())
    );

    xml::update_tag(dataNode, "showingametimer", (int) showingametimer);

    xml::update_tag(dataNode, "vsync", (int) screen_settings->useVsync);

    xml::update_tag(dataNode, "musicvolume", music.user_music_volume);

    xml::update_tag(dataNode, "soundvolume", music.user_sound_volume);

    xml::update_tag(dataNode, "separate_interact", (int) separate_interact);

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
        || SDL_strcmp(name, "restartButton") == 0
        || SDL_strcmp(name, "interactButton") == 0)
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
    for (size_t i = 0; i < controllerButton_interact.size(); i += 1)
    {
        tinyxml2::XMLElement* msg = doc.NewElement("interactButton");
        msg->LinkEndChild(doc.NewText(help.String((int) controllerButton_interact[i]).c_str()));
        dataNode->LinkEndChild(msg);
    }

    xml::update_tag(dataNode, "controllerSensitivity", key.sensitivity);

    xml::update_tag(dataNode, "lang", loc::lang.c_str());
    xml::update_tag(dataNode, "lang_set", (int) loc::lang_set);
    xml::update_tag(dataNode, "english_sprites", (int) loc::english_sprites);
    xml::update_tag(dataNode, "new_level_font", loc::new_level_font.c_str());
    xml::update_tag(dataNode, "roomname_translator", (int) roomname_translator::enabled);
}

static bool settings_loaded = false;

void Game::loadsettings(struct ScreenSettings* screen_settings)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* dataNode;

    settings_loaded = true;

    if (!FILESYSTEM_loadTiXml2Document("saves/settings.vvv", doc))
    {
        savesettings(screen_settings);
        return;
    }

    if (doc.Error())
    {
        vlog_error("Error parsing settings.vvv: %s", doc.ErrorStr());
        return;
    }

    dataNode = hDoc
        .FirstChildElement()
        .FirstChildElement("Data")
        .FirstChildElement()
        .ToElement();

    deserializesettings(dataNode, screen_settings);
}

bool Game::savesettings(void)
{
    struct ScreenSettings screen_settings;
    SDL_zero(screen_settings);
    gameScreen.GetSettings(&screen_settings);

    return savesettings(&screen_settings);
}

bool Game::savesettings(const struct ScreenSettings* screen_settings)
{
    tinyxml2::XMLDocument doc;
    bool already_exists;

    if (!settings_loaded)
    {
        vlog_warn("Settings not loaded! Not writing settings.vvv.");
        return false;
    }

    already_exists = FILESYSTEM_loadTiXml2Document("saves/settings.vvv", doc);
    if (!already_exists)
    {
        vlog_info("No settings.vvv found. Creating new file");
    }
    else if (doc.Error())
    {
        vlog_error("Error parsing existing settings.vvv: %s", doc.ErrorStr());
        vlog_info("Creating new settings.vvv");
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

    setstate(0);
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

    setstate(0);
    deathseq = -1;
    lifeseq = 0;

    if (!nocutscenes)
    {
        music.play(Music_PAUSE);
    }
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

            /* Fix a bug being able to play music on the Game Over screen */
            music.nicefade = false;
        }
        deathcounts++;
        music.playef(Sound_CRY);
        key.controllerRumble(0xDFFF,200);
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
    if (INBOUNDS_VEC(i, obj.entities) && !noflashingmode)
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
    setstate(0);
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

    setstate(0);
    deathseq = -1;
    lifeseq = 0;
}

void Game::loadquick(void)
{
    tinyxml2::XMLDocument doc;
    if (!FILESYSTEM_loadTiXml2Document("saves/qsave.vvv", doc)) return;

    readmaingamesave("qsave.vvv", doc);
}

void Game::readmaingamesave(const char* savename, tinyxml2::XMLDocument& doc)
{
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;

    if (doc.Error())
    {
        vlog_error("Error parsing %s: %s", savename, doc.ErrorStr());
        return;
    }

    for (pElem = hDoc
        .FirstChildElement()
        .FirstChildElement("Data")
        .FirstChildElement()
        .ToElement();
    pElem != NULL;
    pElem = pElem->NextSiblingElement())
    {
        const char* pKey = pElem->Value();
        const char* pText = pElem->GetText();
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
        else if (SDL_strcmp(pKey, "frames") == 0)
        {
            frames = help.Int(pText);
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
        else if (SDL_strcmp(pKey, "showtargets") == 0)
        {
            map.showtargets = help.Int(pText);
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

void Game::customloadquick(const std::string& savfile)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;
    std::string levelfile;

    if (cliplaytest)
    {
        savex = playx;
        savey = playy;
        saverx = playrx;
        savery = playry;
        savegc = playgc;
        if (playmusic > -1)
        {
            music.play(playmusic);
        }
        return;
    }

    levelfile = savfile.substr(7);
    if (!FILESYSTEM_loadTiXml2Document(("saves/"+levelfile+".vvv").c_str(), doc))
    {
        vlog_error("%s.vvv not found", levelfile.c_str());
        return;
    }

    if (doc.Error())
    {
        vlog_error("Error parsing %s.vvv: %s", levelfile.c_str(), doc.ErrorStr());
        return;
    }

    for (pElem = hDoc
        .FirstChildElement()
        .FirstChildElement("Data")
        .FirstChildElement()
        .ToElement();
    pElem != NULL;
    pElem = pElem->NextSiblingElement())
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
        else if (SDL_strcmp(pKey, "savecolour") == 0)
        {
            savecolour = help.Int(pText);
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
        else if (SDL_strcmp(pKey, "frames") == 0)
        {
            frames = help.Int(pText);
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
        else if (SDL_strcmp(pKey, "lang_custom") == 0)
        {
            loc::lang_custom = pText;
            if (pText[0] != '\0')
            {
                loc::loadtext_custom(NULL);
            }
        }
        else if (SDL_strcmp(pKey, "showminimap") == 0)
        {
            map.customshowmm = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "disabletemporaryaudiopause") == 0)
        {
            disabletemporaryaudiopause = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "showtrinkets") == 0)
        {
            map.showtrinkets = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "roomname") == 0)
        {
            map.setroomname(pText);
            map.roomnameset = true;
            map.roomname_special = true;
        }
    }
}

struct Summary
{
    const char* summary;
    int seconds;
    int minutes;
    int hours;
    int savex;
    int savey;
    int trinkets;
    bool finalmode;
    bool crewstats[Game::numcrew];
};

static void loadthissummary(
    const char* filename,
    struct Summary* summary,
    tinyxml2::XMLDocument& doc
) {
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;

    if (doc.Error())
    {
        vlog_error("Error parsing %s: %s", filename, doc.ErrorStr());
        return;
    }

    for (pElem = hDoc
        .FirstChildElement()
        .FirstChildElement("Data")
        .FirstChildElement()
        .ToElement();
    pElem != NULL;
    pElem = pElem->NextSiblingElement())
    {
        const char* pKey = pElem->Value();
        const char* pText = pElem->GetText();

        if (pText == NULL)
        {
            pText = "";
        }

        if (pText == NULL)
        {
            pText = "";
        }

        if (SDL_strcmp(pKey, "summary") == 0)
        {
            summary->summary = pText;
        }

        else if (SDL_strcmp(pKey, "seconds") == 0)
        {
            summary->seconds = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "minutes") == 0)
        {
            summary->minutes = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "hours") == 0)
        {
            summary->hours = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "saverx") == 0)
        {
            summary->savex = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "savery") == 0)
        {
            summary->savey = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "trinkets") == 0)
        {
            summary->trinkets = help.Int(pText);
        }
        else if (SDL_strcmp(pKey, "finalmode") == 0)
        {
            map.finalmode = help.Int(pText);
        }

        LOAD_ARRAY_RENAME(crewstats, summary->crewstats)
    }
}

void Game::loadsummary(void)
{
    tinyxml2::XMLDocument doc;

    if (!FILESYSTEM_loadTiXml2Document("saves/tsave.vvv", doc))
    {
        telesummary = "";
    }
    else
    {
        struct Summary summary;
        SDL_zero(summary);

        loadthissummary("tsave.vvv", &summary, doc);

        telesummary = summary.summary;
        tele_gametime = giventimestring(
            summary.hours,
            summary.minutes,
            summary.seconds
        );
        map.finalmode = summary.finalmode;
        tele_currentarea = map.currentarea(
            map.area(summary.savex, summary.savey)
        );
        SDL_memcpy(tele_crewstats, summary.crewstats, sizeof(tele_crewstats));
        tele_trinkets = summary.trinkets;
    }

    if (!FILESYSTEM_loadTiXml2Document("saves/qsave.vvv", doc))
    {
        quicksummary = "";
    }
    else
    {
        struct Summary summary;
        SDL_zero(summary);

        loadthissummary("qsave.vvv", &summary, doc);

        quicksummary = summary.summary;
        quick_gametime = giventimestring(
            summary.hours,
            summary.minutes,
            summary.seconds
        );
        map.finalmode = summary.finalmode;
        quick_currentarea = map.currentarea(
            map.area(summary.savex, summary.savey)
        );
        SDL_memcpy(quick_crewstats, summary.crewstats, sizeof(quick_crewstats));
        quick_trinkets = summary.trinkets;
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
        vlog_info("No tsave.vvv found. Creating new file");
    }
    else if (doc.Error())
    {
        vlog_error("Error parsing existing tsave.vvv: %s", doc.ErrorStr());
        vlog_info("Creating new tsave.vvv");
    }

    telesummary = writemaingamesave(doc);

    if(!FILESYSTEM_saveTiXml2Document("saves/tsave.vvv", doc))
    {
        vlog_error("Could Not Save game!");
        vlog_error("Failed: %s%s", saveFilePath, "tsave.vvv");
        return false;
    }
    vlog_info("Game saved");
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
        vlog_info("No qsave.vvv found. Creating new file");
    }
    else if (doc.Error())
    {
        vlog_error("Error parsing existing qsave.vvv: %s", doc.ErrorStr());
        vlog_info("Creating new qsave.vvv");
    }

    quicksummary = writemaingamesave(doc);

    if(!FILESYSTEM_saveTiXml2Document("saves/qsave.vvv", doc))
    {
        vlog_error("Could Not Save game!");
        vlog_error("Failed: %s%s", saveFilePath, "qsave.vvv");
        return false;
    }
    vlog_info("Game saved");
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

    xml::update_tag(msgs, "showtargets", (int) map.showtargets);

    xml::update_tag(msgs, "teleportscript", teleportscript.c_str());
    xml::update_tag(msgs, "companion", companion);

    xml::update_tag(msgs, "lastsaved", lastsaved);
    xml::update_tag(msgs, "supercrewmate", (int) supercrewmate);

    xml::update_tag(msgs, "scmprogress", scmprogress);


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


bool Game::customsavequick(const std::string& savfile)
{
    const std::string levelfile = savfile.substr(7);

    tinyxml2::XMLDocument doc;
    bool already_exists = FILESYSTEM_loadTiXml2Document(("saves/" + levelfile + ".vvv").c_str(), doc);
    if (!already_exists)
    {
        vlog_info("No %s.vvv found. Creating new file", levelfile.c_str());
    }
    else if (doc.Error())
    {
        vlog_error("Error parsing existing %s.vvv: %s", levelfile.c_str(), doc.ErrorStr());
        vlog_info("Creating new %s.vvv", levelfile.c_str());
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

    xml::update_tag(msgs, "savecolour", savecolour);

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

    xml::update_tag(msgs, "lang_custom", loc::lang_custom.c_str());

    xml::update_tag(msgs, "teleportscript", teleportscript.c_str());
    xml::update_tag(msgs, "companion", companion);

    xml::update_tag(msgs, "lastsaved", lastsaved);
    xml::update_tag(msgs, "supercrewmate", (int) supercrewmate);

    xml::update_tag(msgs, "scmprogress", scmprogress);


    xml::update_tag(msgs, "frames", frames);
    xml::update_tag(msgs, "seconds", seconds);

    xml::update_tag(msgs, "minutes", minutes);
    xml::update_tag(msgs, "hours", hours);

    xml::update_tag(msgs, "deathcounts", deathcounts);
    xml::update_tag(msgs, "totalflips", totalflips);

    xml::update_tag(msgs, "hardestroom", hardestroom.c_str());
    xml::update_tag(msgs, "hardestroomdeaths", hardestroomdeaths);

    xml::update_tag(msgs, "showminimap", (int) map.customshowmm);

    xml::update_tag(msgs, "disabletemporaryaudiopause", (int) disabletemporaryaudiopause);

    xml::update_tag(msgs, "showtrinkets", (int) map.showtrinkets);

    if (map.roomnameset)
    {
        xml::update_tag(msgs, "roomname", map.roomname);
    }
    else
    {
        // If there's roomname tags, remove them. There will probably only always be one, but just in case...
        tinyxml2::XMLElement* element;
        while ((element = msgs->FirstChildElement("roomname")) != NULL)
        {
            doc.DeleteNode(element);
        }
    }

    std::string summary = savearea + ", " + timestring();
    xml::update_tag(msgs, "summary", summary.c_str());

    customquicksummary = summary;

    if(!FILESYSTEM_saveTiXml2Document(("saves/"+levelfile+".vvv").c_str(), doc))
    {
        vlog_error("Could Not Save game!");
        vlog_error("Failed: %s%s%s", saveFilePath, levelfile.c_str(), ".vvv");
        return false;
    }
    vlog_info("Game saved");
    return true;
}


void Game::loadtele(void)
{
    tinyxml2::XMLDocument doc;
    if (!FILESYSTEM_loadTiXml2Document("saves/tsave.vvv", doc)) return;

    readmaingamesave("tsave.vvv", doc);
}

std::string Game::unrescued(void)
{
    //Randomly return the name of an unrescued crewmate
    //Localization is handled with regular cutscene dialogue
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
    if (timetrialcountdown > 0)
    {
        return;
    }

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
    return timetstring(help.hms_to_seconds(hrs, min, sec));
}

std::string Game::timestring(void)
{
    return giventimestring(hours, minutes, seconds);
}

std::string Game::resulttimestring(void)
{
    //given result time in seconds:
    char output[SCREEN_WIDTH_CHARS + 1];
    help.format_time(output, sizeof(output), timetrialresulttime, timetrialresultframes, true);
    return output;
}

std::string Game::timetstring( int t )
{
    //given par time in seconds:
    char output[SCREEN_WIDTH_CHARS + 1];
    help.format_time(output, sizeof(output), t, -1, true);
    return output;
}

void Game::timestringcenti(char* buffer, const size_t buffer_size)
{
    help.format_time(buffer, buffer_size, help.hms_to_seconds(hours, minutes, seconds), frames, true);
}

void Game::returnmenu(void)
{
    if (menustack.empty())
    {
        vlog_error("Error: returning to previous menu frame on empty stack!");
        return;
    }

    /* FIXME: Super bad kludge, don't hardcode this! */
    if (currentmenuname == Menu::ed_music)
    {
        music.fadeout();
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
    if (t == Menu::mainmenu && !menutestmode)
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
        if (ingame_titlemode)
        {
            /* We shouldn't be here! */
            SDL_assert(0 && "Entering main menu from in-game options!");
            break;
        }
#if !defined(MAKEANDPLAY)
        option(loc::gettext("play"));
#endif
        option(loc::gettext("levels"));
        option(loc::gettext("options"));
        if (loc::show_translator_menu)
        {
            option(loc::gettext("translator"));
        }
        option(loc::gettext("credits"));
        option(loc::gettext("quit"));
        menuyoff = -10;
        maxspacing = 15;
        break;
    case Menu::playerworlds:
        option(loc::gettext("play a level"));
        option(loc::gettext("level editor"), !editor_disabled);
        if (!editor_disabled)
        {
            option(loc::gettext("open level folder"), FILESYSTEM_openDirectoryEnabled());
            option(loc::gettext("show level folder path"));
        }
        option(loc::gettext("return"));
        menuyoff = -40;
        maxspacing = 15;
        break;
    case Menu::confirmshowlevelspath:
        option(loc::gettext("no, don't show me"));
        option(loc::gettext("yes, reveal the path"));
        menuyoff = -10;
        break;
    case Menu::showlevelspath:
        option(loc::gettext("return to levels"));
        menuyoff = 60;
        break;
    case Menu::levellist:
        if(cl.ListOfMetaData.size()==0)
        {
            option(loc::gettext("ok"));
            menuyoff = -20;
        }
        else
        {
            for(int i=0; i<(int) cl.ListOfMetaData.size(); i++) // FIXME: int/size_t! -flibit
            {
                if(i>=levelpage*8 && i< (levelpage*8)+8)
                {
                    //This is, er, suboptimal. Whatever, life optimisation and all that
                    int tvar=-1;
                    for(size_t j=0; j<customlevelstats.size(); j++)
                    {
                        if(cl.ListOfMetaData[i].filename.substr(7) == customlevelstats[j].name)
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
                    SDL_snprintf(text, sizeof(text), "%s%s", prefix, cl.ListOfMetaData[i].title.c_str());
                    for (size_t ii = 0; text[ii] != '\0'; ++ii)
                    {
                        text[ii] = SDL_tolower(text[ii]);
                    }
                    option(
                        text,
                        true,
                        cl.ListOfMetaData[i].title_is_gettext ? PR_FONT_INTERFACE : PR_FONT_IDX(
                            cl.ListOfMetaData[i].level_main_font_idx
                        )
                    );
                }
            }
            if (cl.ListOfMetaData.size() > 8)
            {
                if((size_t) ((levelpage*8)+8) <cl.ListOfMetaData.size())
                {
                    option(loc::gettext("next page"));
                }
                else
                {
                    option(loc::gettext("first page"));
                }
                if (levelpage == 0)
                {
                    option(loc::gettext("last page"));
                }
                else
                {
                    option(loc::gettext("previous page"));
                }
            }
            option(loc::gettext("return"));

            menuxoff = 20;
            menuyoff = 70-(menuoptions.size()*10);
            menuspacing = 5;
            return; // skip automatic centering, will turn out bad with levels list
        }
        break;
    case Menu::quickloadlevel:
        option(loc::gettext("continue from save"));
        option(loc::gettext("start from beginning"));
        option(loc::gettext("delete save"));
        option(loc::gettext("back to levels"));
        menuyoff = -30;
        break;
    case Menu::deletequicklevel:
        option(loc::gettext("no! don't delete"));
        option(loc::gettext("yes, delete save"));
        menuyoff = 64;
        break;
    case Menu::youwannaquit:
        option(loc::gettext("yes, quit"));
        option(loc::gettext("no, return"));
        menuyoff = -20;
        break;
    case Menu::errornostart:
        option(loc::gettext("ok"));
        menuyoff = -20;
        break;
    case Menu::gameplayoptions:
#if !defined(MAKEANDPLAY)
        if (ingame_titlemode && unlock[Unlock_FLIPMODE])
#endif
        {
                option(loc::gettext("flip mode"));
        }
        option(loc::gettext("toggle fps"));
        option(loc::gettext("speedrun options"));
        option(loc::gettext("advanced options"));
        option(loc::gettext("clear main game data"));
        option(loc::gettext("clear custom level data"));
        option(loc::gettext("return"));
        menuyoff = -10;
        maxspacing = 15;
        break;
    case Menu::graphicoptions:
        if (!gameScreen.isForcedFullscreen())
        {
            option(loc::gettext("toggle fullscreen"));
        }
        option(loc::gettext("scaling mode"));
        if (!gameScreen.isForcedFullscreen())
        {
            option(loc::gettext("resize to nearest"), gameScreen.isWindowed);
        }
        option(loc::gettext("toggle filter"));
        option(loc::gettext("toggle analogue"));
        option(loc::gettext("toggle vsync"));
        option(loc::gettext("return"));
        menuyoff = -10;
        maxspacing = 15;
        break;
    case Menu::ed_settings:
        option(loc::gettext("change description"));
        option(loc::gettext("edit scripts"));
        option(loc::gettext("change music"));
        option(loc::gettext("editor ghosts"));
        option(loc::gettext("load level"));
        option(loc::gettext("save level"));
        option(loc::gettext("options"));
        option(loc::gettext("quit to main menu"));

        menuyoff = -20;
        maxspacing = 15;
        break;
    case Menu::ed_desc:
        option(loc::gettext("change name"));
        option(loc::gettext("change author"));
        option(loc::gettext("change description"));
        option(loc::gettext("change website"));
        option(loc::gettext("change font"));
        option(loc::gettext("return"));

        menuyoff = 6;
        maxspacing = 15;
        break;
    case Menu::ed_music:
        option(loc::gettext("next song"));
        option(loc::gettext("previous song"));
        option(loc::gettext("return"));
        menuyoff = 16;
        maxspacing = 15;
        break;
    case Menu::ed_quit:
        option(loc::gettext("yes, save and quit"));
        option(loc::gettext("no, quit without saving"));
        option(loc::gettext("return to editor"));
        menuyoff = 8;
        maxspacing = 15;
        break;
    case Menu::ed_font:
    {
        int option_match = -1;
        for (uint8_t i = 0; i < font::font_idx_options_n; i++)
        {
            uint8_t idx = font::font_idx_options[i];
            option(font::get_main_font_display_name(idx), true, PR_FONT_IDX(idx));
            if (font::level_font_is_main_idx(idx))
            {
                option_match = i;
            }
        }

        currentmenuoption = option_match != -1 ? option_match : 0;
        maxspacing = 15;
        break;
    }
    case Menu::options:
        option(loc::gettext("gameplay"));
        option(loc::gettext("graphics"));
        option(loc::gettext("audio"));
        option(loc::gettext("game pad"));
        option(loc::gettext("accessibility"));
        option(loc::gettext("language"), graphics.textboxes.empty());
        option(loc::gettext("return"));
        menuyoff = 0;
        maxspacing = 15;
        break;
    case Menu::speedrunneroptions:
        option(loc::gettext("glitchrunner mode"));
        option(loc::gettext("input delay"));
        option(loc::gettext("interact button"));
        option(loc::gettext("fake load screen"));
        option(loc::gettext("toggle in-game timer"));
        option(loc::gettext("english sprites"));
        option(loc::gettext("return"));
        menuyoff = 0;
        maxspacing = 15;
        break;
    case Menu::setglitchrunner:
    {
        int i;

        option(loc::gettext("none"));

        for (i = 1; i < GlitchrunnerNumVersions; ++i)
        {
            option(loc::gettext(GlitchrunnerMode_enum_to_string((enum GlitchrunnerMode) i)));
        }
        break;
    }
    case Menu::advancedoptions:
        option(loc::gettext("unfocus pause"));
        option(loc::gettext("unfocus audio pause"));
        option(loc::gettext("room name background"));
        option(loc::gettext("return"));
        menuyoff = 0;
        maxspacing = 15;
        break;
    case Menu::audiooptions:
        option(loc::gettext("music volume"));
        option(loc::gettext("sound volume"));
        if (music.mmmmmm)
        {
            option(loc::gettext("soundtrack"));
        }
        option(loc::gettext("return"));
        menuyoff = 0;
        maxspacing = 15;
        break;
    case Menu::accessibility:
#if !defined(MAKEANDPLAY)
        option(loc::gettext("unlock play modes"));
#endif
        option(loc::gettext("invincibility"), !ingame_titlemode || !incompetitive());
        option(loc::gettext("slowdown"), !ingame_titlemode || !incompetitive());
        option(loc::gettext("animated backgrounds"));
        option(loc::gettext("screen effects"));
        option(loc::gettext("text outline"));
        option(loc::gettext("return"));
        menuyoff = 0;
        maxspacing = 15;
        break;
    case Menu::controller:
        option(loc::gettext("analog stick sensitivity"));
        option(loc::gettext("bind flip"));
        option(loc::gettext("bind enter"));
        option(loc::gettext("bind menu"));
        option(loc::gettext("bind restart"));
        option(loc::gettext("bind interact"), separate_interact);
        option(loc::gettext("toggle rumble"));
        option(loc::gettext("return"));
        menuyoff = 0;
        maxspacing = 10;
        break;
    case Menu::language:
        if (loc::languagelist.empty())
        {
            option(loc::gettext("ok"));
            menuyoff = -20;
        }
        else
        {
            for (size_t i = 0; i < loc::languagelist.size(); i++)
            {
                if (loc::languagelist[i].nativename.empty())
                    option(loc::languagelist[i].code.c_str());
                else
                    option(loc::languagelist[i].nativename.c_str(), true, PR_FONT_IDX(loc::languagelist[i].font_idx));
            }

            menuyoff = 70-(menuoptions.size()*10);
            maxspacing = 5;
        }
        break;
    case Menu::translator_main:
        option(loc::gettext("translator options"));
        option(loc::gettext("maintenance"));
        option(loc::gettext("open lang folder"), FILESYSTEM_openDirectoryEnabled());
        option(loc::gettext("return"));
        menuyoff = 0;
        break;
    case Menu::translator_options:
        option(loc::gettext("language statistics"));
        option(loc::gettext("translate room names"));
        option(loc::gettext("explore game"));
        option(loc::gettext("menu test"));
        option(loc::gettext("cutscene test"), loc::lang != "en");
        option(loc::gettext("limits check"));
        option(loc::gettext("return"));
        menuyoff = 0;
        break;
    case Menu::translator_options_limitscheck:
        option(loc::gettext("next page"));
        option(loc::gettext("return"));
        menuyoff = 64;
        break;
    case Menu::translator_options_stats:
        option(loc::gettext("return"));
        menuyoff = 64;
        break;
    case Menu::translator_options_exploregame:
        option(loc::gettext("space station 1"));
        option(loc::gettext("the laboratory"));
        option(loc::gettext("the tower"));
        option(loc::gettext("space station 2"));
        option(loc::gettext("the warp zone"));
        option(loc::gettext("intermission 1"));
        option(loc::gettext("intermission 2"));
        option(loc::gettext("the final level"));
        option(loc::gettext("return"));
        menuyoff = -20;
        break;
    case Menu::translator_options_cutscenetest:
        for (
            size_t i = (cutscenetest_menu_page*14);
            i < (cutscenetest_menu_page*14)+14 && i < loc::testable_script_ids.size();
            i++
        )
        {
            option(loc::testable_script_ids[i].c_str());
        }
        if((cutscenetest_menu_page*14)+14 < loc::testable_script_ids.size())
        {
            option(loc::gettext("next page"));
        }
        else
        {
            option(loc::gettext("first page"));
        }
        if (cutscenetest_menu_page == 0)
        {
            option(loc::gettext("last page"));
        }
        else
        {
            option(loc::gettext("previous page"));
        }
        option(loc::gettext("from clipboard"));
        option(loc::gettext("return"));

        menuxoff = 20;
        menuyoff = 55-(menuoptions.size()*10);
        menuspacing = 5;
        return; // skip automatic centering, will turn out bad with scripts list
    case Menu::translator_maintenance:
        option(loc::gettext("sync language files"));
        option(loc::gettext("global statistics"), false);
        option(loc::gettext("global limits check"));
        option(loc::gettext("return"));
        menuyoff = 0;
        break;
    case Menu::translator_maintenance_sync:
        option(loc::gettext("sync"));
        option(loc::gettext("return"));
        menuyoff = 64;
        break;
    case Menu::translator_error_setlangwritedir:
        option(loc::gettext("ok"));
        menuyoff = 10;
        break;
    case Menu::cleardatamenu:
    case Menu::clearcustomdatamenu:
        option(loc::gettext("no! don't delete"));
        option(loc::gettext("yes, delete everything"));
        menuyoff = 64;
        break;
    case Menu::setinvincibility:
        option(loc::gettext("no, return to options"));
        option(loc::gettext("yes, enable"));
        menuyoff = 64;
        break;
    case Menu::setslowdown:
        option(loc::gettext("normal speed"));
        option(loc::gettext("80% speed"));
        option(loc::gettext("60% speed"));
        option(loc::gettext("40% speed"));
        menuyoff = 16;
        break;
    case Menu::unlockmenu:
        option(loc::gettext("unlock time trials"));
        option(loc::gettext("unlock intermissions"), !unlock[Unlock_INTERMISSION_REPLAYS]);
        option(loc::gettext("unlock no death mode"), !unlock[Unlock_NODEATHMODE]);
        option(loc::gettext("unlock flip mode"), !unlock[Unlock_FLIPMODE]);
        option(loc::gettext("unlock ship jukebox"), (stat_trinkets<20));
        option(loc::gettext("unlock secret lab"), !unlock[Unlock_SECRETLAB]);
        option(loc::gettext("return"));
        menuyoff = -20;
        break;
    case Menu::credits:
        option(loc::gettext("next page"));
        option(loc::gettext("last page"));
        option(loc::gettext("return"));
        menuyoff = 64;
        break;
    case Menu::credits2:
    case Menu::credits25:
    case Menu::credits3:
    case Menu::credits4:
    case Menu::credits5:
        option(loc::gettext("next page"));
        option(loc::gettext("previous page"));
        option(loc::gettext("return"));
        menuyoff = 64;
        break;
    case Menu::credits6:
        option(loc::gettext("first page"));
        option(loc::gettext("previous page"));
        option(loc::gettext("return"));
        menuyoff = 64;
        break;
    case Menu::play:
    {
        //Ok, here's where the unlock stuff comes into it:
        //First up, time trials:
        int temp = 0;
        if (unlock[Unlock_SPACESTATION1_COMPLETE]
            && stat_trinkets >= 3
            && !unlocknotify[Unlock_TIMETRIAL_SPACESTATION1])
        {
            temp++;
        }
        if (unlock[Unlock_LABORATORY_COMPLETE]
            && stat_trinkets >= 6
            && !unlocknotify[Unlock_TIMETRIAL_LABORATORY])
        {
            temp++;
        }
        if (unlock[Unlock_TOWER_COMPLETE]
            && stat_trinkets >= 9
            && !unlocknotify[Unlock_TIMETRIAL_TOWER])
        {
            temp++;
        }
        if (unlock[Unlock_SPACESTATION2_COMPLETE]
            && stat_trinkets >= 12
            && !unlocknotify[Unlock_TIMETRIAL_SPACESTATION2])
        {
            temp++;
        }
        if (unlock[Unlock_WARPZONE_COMPLETE]
            && stat_trinkets >= 15
            && !unlocknotify[Unlock_TIMETRIAL_WARPZONE])
        {
            temp++;
        }
        if (unlock[UnlockTrophy_GAME_COMPLETE]
            && stat_trinkets >= 18
            && !unlocknotify[Unlock_TIMETRIAL_FINALLEVEL])
        {
            temp++;
        }
        if (temp > 0)
        {
            //you've unlocked a time trial!
            if (unlock[Unlock_SPACESTATION1_COMPLETE] && stat_trinkets >= 3)
            {
                unlocknotify[Unlock_TIMETRIAL_SPACESTATION1] = true;
                unlock[Unlock_TIMETRIAL_SPACESTATION1] = true;
            }
            if (unlock[Unlock_LABORATORY_COMPLETE] && stat_trinkets >= 6)
            {
                unlocknotify[Unlock_TIMETRIAL_LABORATORY] = true;
                unlock[Unlock_TIMETRIAL_LABORATORY] = true;
            }
            if (unlock[Unlock_TOWER_COMPLETE] && stat_trinkets >= 9)
            {
                unlocknotify[Unlock_TIMETRIAL_TOWER] = true;
                unlock[Unlock_TIMETRIAL_TOWER] = true;
            }
            if (unlock[Unlock_SPACESTATION2_COMPLETE] && stat_trinkets >= 12)
            {
                unlocknotify[Unlock_TIMETRIAL_SPACESTATION2] = true;
                unlock[Unlock_TIMETRIAL_SPACESTATION2] = true;
            }
            if (unlock[Unlock_WARPZONE_COMPLETE] && stat_trinkets >= 15)
            {
                unlocknotify[Unlock_TIMETRIAL_WARPZONE] = true;
                unlock[Unlock_TIMETRIAL_WARPZONE] = true;
            }
            if (unlock[UnlockTrophy_GAME_COMPLETE] && stat_trinkets >= 18)
            {
                unlocknotify[Unlock_TIMETRIAL_FINALLEVEL] = true;
                unlock[Unlock_TIMETRIAL_FINALLEVEL] = true;
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
            if (bestrank[TimeTrial_SPACESTATION1] >= 2) temp++;
            if (bestrank[TimeTrial_LABORATORY] >= 2) temp++;
            if (bestrank[TimeTrial_TOWER] >= 2) temp++;
            if (bestrank[TimeTrial_SPACESTATION2] >= 2) temp++;
            if (bestrank[TimeTrial_WARPZONE] >= 2) temp++;
            if (bestrank[TimeTrial_FINALLEVEL] >= 2) temp++;
            if (temp >= 4 && !unlocknotify[Unlock_NODEATHMODE])
            {
                //Unlock No Death Mode
                unlocknotify[Unlock_NODEATHMODE] = true;
                unlock[Unlock_NODEATHMODE] = true;
                createmenu(Menu::unlocknodeathmode, true);
                savestatsandsettings();
            }
            //Alright then! Flip mode?
            else if (unlock[UnlockTrophy_GAME_COMPLETE]
                && !unlocknotify[Unlock_FLIPMODE])
            {
                unlock[Unlock_FLIPMODE] = true;
                unlocknotify[Unlock_FLIPMODE] = true;
                createmenu(Menu::unlockflipmode, true);
                savestatsandsettings();
            }
            //What about the intermission levels?
            else if (unlock[Unlock_INTERMISSION2_COMPLETE]
                && !unlocknotify[Unlock_INTERMISSION_REPLAYS])
            {
                unlock[Unlock_INTERMISSION_REPLAYS] = true;
                unlocknotify[Unlock_INTERMISSION_REPLAYS] = true;
                createmenu(Menu::unlockintermission, true);
                savestatsandsettings();
            }
            else
            {
                if (save_exists())
                {
                    option(loc::gettext("continue"));
                }
                else
                {
                    option(loc::gettext("new game"));
                }
                //ok, secret lab! no notification, but test:
                if (unlock[Unlock_SECRETLAB])
                {
                    option(loc::gettext("secret lab"));
                }
                option(loc::gettext("play modes"));
                if (save_exists())
                {
                    option(loc::gettext("new game"));
                }
                option(loc::gettext("return"));
                if (unlock[Unlock_SECRETLAB])
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
        option(loc::gettext("proceed"));
        menuyoff = 70;
        break;
    case Menu::newgamewarning:
        option(loc::gettext("start new game"));
        option(loc::gettext("return"));
        menuyoff = 64;
        break;
    case Menu::playmodes:
        option(loc::gettext("time trials"), !nocompetitive_unless_translator());
        option(loc::gettext("intermissions"), unlock[Unlock_INTERMISSION_REPLAYS]);
        option(loc::gettext("no death mode"), unlock[Unlock_NODEATHMODE] && !nocompetitive());
        option(loc::gettext("flip mode"), unlock[Unlock_FLIPMODE]);
        option(loc::gettext("return"));
        menuyoff = 8;
        maxspacing = 20;
        break;
    case Menu::intermissionmenu:
        option(loc::gettext("play intermission 1"));
        option(loc::gettext("play intermission 2"));
        option(loc::gettext("return"));
        menuyoff = -35;
        break;
    case Menu::playint1:
        start_translator_exploring = false;
        option(loc::gettext_case("Vitellary", 1));
        option(loc::gettext_case("Vermilion", 1));
        option(loc::gettext_case("Verdigris", 1));
        option(loc::gettext_case("Victoria", 1));
        option(loc::gettext("return"));
        menuyoff = 10;
        break;
    case Menu::playint2:
        start_translator_exploring = false;
        option(loc::gettext_case("Vitellary", 1));
        option(loc::gettext_case("Vermilion", 1));
        option(loc::gettext_case("Verdigris", 1));
        option(loc::gettext_case("Victoria", 1));
        option(loc::gettext("return"));
        menuyoff = 10;
        break;
    case Menu::continuemenu:
        map.settowercolour(3);
        option(loc::gettext("continue from teleporter"));
        option(loc::gettext("continue from quicksave"));
        option(loc::gettext("return"));
        menuyoff = 20;
        break;
    case Menu::startnodeathmode:
        option(loc::gettext("disable cutscenes"));
        option(loc::gettext("enable cutscenes"));
        option(loc::gettext("return"));
        menuyoff = 40;
        break;
    case Menu::gameover:
        menucountdown = 120;
        menudest=Menu::gameover2;
        break;
    case Menu::gameover2:
        option(loc::gettext("return to play menu"));
        menuyoff = 80;
        break;
    case Menu::unlockmenutrials:
        option(loc::gettext("space station 1"), !unlock[Unlock_TIMETRIAL_SPACESTATION1]);
        option(loc::gettext("the laboratory"), !unlock[Unlock_TIMETRIAL_LABORATORY]);
        option(loc::gettext("the tower"), !unlock[Unlock_TIMETRIAL_TOWER]);
        option(loc::gettext("space station 2"), !unlock[Unlock_TIMETRIAL_SPACESTATION2]);
        option(loc::gettext("the warp zone"), !unlock[Unlock_TIMETRIAL_WARPZONE]);
        option(loc::gettext("the final level"), !unlock[Unlock_TIMETRIAL_FINALLEVEL]);

        option(loc::gettext("return"));
        menuyoff = 0;
        break;
    case Menu::timetrials:
        option(loc::gettext(unlock[Unlock_TIMETRIAL_SPACESTATION1] ? "space station 1" : "???"),
            unlock[Unlock_TIMETRIAL_SPACESTATION1]);
        option(loc::gettext(unlock[Unlock_TIMETRIAL_LABORATORY] ? "the laboratory" : "???"),
            unlock[Unlock_TIMETRIAL_LABORATORY]);
        option(loc::gettext(unlock[Unlock_TIMETRIAL_TOWER] ? "the tower" : "???"),
            unlock[Unlock_TIMETRIAL_TOWER]);
        option(loc::gettext(unlock[Unlock_TIMETRIAL_SPACESTATION2] ? "space station 2" : "???"),
            unlock[Unlock_TIMETRIAL_SPACESTATION2]);
        option(loc::gettext(unlock[Unlock_TIMETRIAL_WARPZONE] ? "the warp zone" : "???"),
            unlock[Unlock_TIMETRIAL_WARPZONE]);
        option(loc::gettext(unlock[Unlock_TIMETRIAL_FINALLEVEL] ? "the final level" : "???"),
            unlock[Unlock_TIMETRIAL_FINALLEVEL]);

        option(loc::gettext("return"));
        menuyoff = 0;
        maxspacing = 15;
        break;
    case Menu::nodeathmodecomplete:
        menucountdown = 90;
        menudest = Menu::nodeathmodecomplete2;
        break;
    case Menu::nodeathmodecomplete2:
        option(loc::gettext("return to play menu"));
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
        option(loc::gettext("return to play menu"));
        option(loc::gettext("try again"));
        menuyoff = 70;
        break;
    case Menu::gamecompletecontinue:
        option(loc::gettext("return to play menu"));
        menuyoff = 70;
        break;
    case Menu::errorsavingsettings:
        option(loc::gettext("ok"));
        option(loc::gettext("silence"));
        menuyoff = 10;
        break;
    case Menu::errorloadinglevel:
    case Menu::warninglevellist:
        option(loc::gettext("ok"));
        menuyoff = 50;
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
            int width = i*menuspacing + font::len(menuoptions[i].print_flags, menuoptions[i].text);
            if (width > menuwidth)
                menuwidth = width;
        }
    }
    menuxoff = (320-menuwidth)/2;
}

void Game::deletequick(void)
{
    if (inspecial() || map.custommode)
    {
        return;
    }

    if( !FILESYSTEM_delete( "saves/qsave.vvv" ) )
        vlog_error("Error deleting saves/qsave.vvv");
    else
        quicksummary = "";
}

void Game::deletetele(void)
{
    if (inspecial() || map.custommode)
    {
        return;
    }

    if( !FILESYSTEM_delete( "saves/tsave.vvv" ) )
        vlog_error("Error deleting saves/tsave.vvv");
    else
        telesummary = "";
}

void Game::customdeletequick(const std::string& file)
{
    const std::string path = "saves/" + file.substr(7) + ".vvv";

    if (!FILESYSTEM_delete(path.c_str()))
    {
        vlog_error("Error deleting %s", path.c_str());
    }
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

static void hardreset(void)
{
    script.hardreset();
}

static void returntoeditor_callback(void)
{
    extern Game game;
    game.returntoeditor();
    ed.show_note(loc::gettext("Level quits to menu"));
}

void Game::quittomenu(void)
{
    if (gamestate != EDITORMODE && map.custommode && !map.custommodeforreal)
    {
        /* We are playtesting! Go back to the editor
         * instead of losing unsaved changes. */
        /* This needs to be deferred, otherwise some state would persist. */
        DEFER_CALLBACK(returntoeditor_callback);
        return;
    }

    gamestate = TITLEMODE;
    graphics.fademode = FADE_START_FADEIN;
    FILESYSTEM_unmountAssets();
    loc::unloadtext_custom();
    font::unload_custom();
    cliplaytest = false;
    graphics.titlebg.tdrawback = true;
    graphics.flipmode = false;
    //Don't be stuck on the summary screen,
    //or "who do you want to play the level with?"
    //or "do you want cutscenes?"
    //or the confirm-load-quicksave menu
    if (translator_cutscene_test)
    {
        returntomenu(Menu::translator_options_cutscenetest);
    }
    else if (translator_exploring)
    {
        returntomenu(Menu::translator_options_exploregame);
    }
    else if (intimetrial)
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
            editor_disabled = !BUTTONGLYPHS_keyboard_is_available();
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
    /* We might not be at the end of the frame yet.
     * If we hardreset() now, some state might still persist. */
    DEFER_CALLBACK(hardreset);
}

void Game::returntolab(void)
{
    gamestate = GAMEMODE;
    graphics.fademode = FADE_START_FADEIN;
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

    music.play(Music_PIPEDREAM);
}

static void resetbg(void)
{
    graphics.backgrounddrawn = false;
}

void Game::returntoeditor(void)
{
    gamestate = EDITORMODE;

    graphics.textboxes.clear();
    hascontrol = true;
    advancetext = false;
    completestop = false;
    setstate(0);
    graphics.showcutscenebars = false;
    graphics.fademode = FADE_NONE;

    ed.keydelay = 6;
    ed.settingskey = true;
    ed.old_note_timer = 0;
    ed.note_timer = 0;
    ed.roomnamehide = 0;

    // Might've been changed in a script
    font::set_level_font(cl.level_font_name.c_str());

    DEFER_CALLBACK(resetbg);
    music.fadeout();
    //If warpdir() is used during playtesting, we need to set it back after!
    for (int j = 0; j < cl.maxheight; j++)
    {
        for (int i = 0; i < cl.maxwidth; i++)
        {
           cl.roomproperties[i+(j*cl.maxwidth)].warpdir=ed.kludgewarpdir[i+(j*cl.maxwidth)];
        }
    }
    graphics.titlebg.scrolldir = 0;
    graphics.backgrounddrawn = false;
    graphics.foregrounddrawn = false;
}

static void returntoingametemp(void)
{
    extern Game game;
    game.returntomenu(game.kludge_ingametemp);
}

static void returntoedsettings(void)
{
    extern Game game;
    game.returntomenu(Menu::ed_settings);
}

static void nextbgcolor(void)
{
    map.nexttowercolour();
}

static void setfademode(void)
{
    graphics.fademode = graphics.ingame_fademode;
}

static void setflipmode(void)
{
    graphics.flipmode = graphics.setflipmode;
}

void Game::returntoingame(void)
{
    ingame_titlemode = false;
    mapheld = true;

    if (ingame_editormode)
    {
        ingame_editormode = false;
        DEFER_CALLBACK(returntoedsettings);
        gamestate = EDITORMODE;
        ed.settingskey = true;
    }
    else
    {
        DEFER_CALLBACK(returntoingametemp);
        gamestate = MAPMODE;
        DEFER_CALLBACK(setflipmode);
        DEFER_CALLBACK(setfademode);
        if (!map.custommode && !graphics.setflipmode)
        {
            obj.flags[73] = true;
        }
    }
    DEFER_CALLBACK(nextbgcolor);
}

void Game::unlockAchievement(const char* name)
{
#ifdef MAKEANDPLAY
    UNUSED(name);
#else
    if (map.custommode)
    {
        return;
    }

    vlog_debug("Achievement \"%s\" unlocked.", name);
    NETWORK_unlockAchievement(name);
#endif
}

void Game::mapmenuchange(const enum GameGamestate newgamestate, const bool user_initiated)
{
    if (user_initiated && graphics.resumegamemode)
    {
        return;
    }

    prevgamestate = gamestate;
    gamestate = newgamestate;
    graphics.resumegamemode = false;
    mapheld = true;
    gameScreen.recacheTextures();

    if (prevgamestate == GAMEMODE)
    {
        graphics.menuoffset = 240;
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

static inline int get_framerate(const int slowdown)
{
    switch (slowdown)
    {
    case 30:
        return 34;
    case 24:
        return 41;
    case 18:
        return 55;
    case 12:
        return 83;
    }

    return 34;
}

int Game::get_timestep(void)
{
    switch (gamestate)
    {
    case GAMEMODE:
        return get_framerate(slowdown);
    default:
        return 34;
    }
}

bool Game::physics_frozen(void)
{
    return roomname_translator::is_pausing();
}

bool Game::incompetitive(void)
{
    return (
        !map.custommode
        && swnmode
        && (swngame == SWN_SUPERGRAVITRON ||
            swngame == SWN_START_SUPERGRAVITRON_STEP_1 ||
            swngame == SWN_START_SUPERGRAVITRON_STEP_2)
    )
    || intimetrial
    || nodeathmode;
}

bool Game::nocompetitive(void)
{
    return slowdown < 30 || map.invincibility;
}

bool Game::nocompetitive_unless_translator(void)
{
    return slowdown < 30 || (map.invincibility && !roomname_translator::enabled);
}

void Game::sabotage_time_trial(void)
{
    timetrialcheater = true;
    hours++;
    deathcounts += 100;
    timetrialparlost = true;
}

bool Game::isingamecompletescreen(void)
{
    return (state >= 3501 && state <= 3518) || (state >= 3520 && state <= 3522);
}
