#define CL_DEFINITION
#include "CustomLevels.h"

#include <physfs.h>
#include <stdio.h>
#include <string>
#include <tinyxml2.h>

#include "Alloc.h"
#include "Constants.h"
#include "Editor.h"
#include "Enums.h"
#include "FileSystemUtils.h"
#include "Font.h"
#include "Game.h"
#include "Graphics.h"
#include "GraphicsUtil.h"
#include "KeyPoll.h"
#include "Localization.h"
#include "LocalizationStorage.h"
#include "Map.h"
#include "Screen.h"
#include "Script.h"
#include "UTF8.h"
#include "UtilityClass.h"
#include "Vlogging.h"
#include "XMLUtils.h"

#ifdef _WIN32
#define SCNx32 "x"
#define SCNu32 "u"
#else
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#include <inttypes.h>
#endif

#define VMULT(y) (y * SCREEN_WIDTH_TILES * maxwidth)
#define Y_INBOUNDS(y) (y >= 0 && y < SCREEN_HEIGHT_TILES * maxheight)

RoomProperty::RoomProperty(void)
{
    tileset=0;
    tilecol=0;
    warpdir=0;
    platx1=0;
    platy1=0;
    platx2=320;
    platy2=240;
    platv=4;
    enemyx1=0;
    enemyy1=0;
    enemyx2=320;
    enemyy2=240;
    enemytype=0;
    enemyv=0;
    directmode=0;
}

customlevelclass::customlevelclass(void)
{
    reset();
}

// comparison, not case sensitive.
static bool compare_nocase (std::string first, std::string second)
{
    unsigned int i=0;
    while ( (i<first.length()) && (i<second.length()) )
    {
        if (SDL_tolower(first[i])<SDL_tolower(second[i]))
            return true;
        else if (SDL_tolower(first[i])>SDL_tolower(second[i]))
            return false;
        ++i;
    }
    if (first.length()<second.length())
        return true;
    else
        return false;
}

/* translate_title and translate_creator are used to display default title/author
 * as being translated, while they're actually stored in English in the level file.
 * This way we translate "Untitled Level" and "Unknown" without
 * spreading around translations in level files posted online! */
bool translate_title(const std::string& title)
{
    return title == "Untitled Level";
}

bool translate_creator(const std::string& creator)
{
    return creator == "Unknown";
}

static void levelZipCallback(const char* filename)
{
    if (!FILESYSTEM_isFile(filename))
    {
        return;
    }

    if (endsWith(filename, ".zip"))
    {
        FILESYSTEM_loadZip(filename);
    }
}

void customlevelclass::loadZips(void)
{
    FILESYSTEM_enumerateLevelDirFileNames(levelZipCallback);
}

static void replace_all(std::string& str, const std::string& from, const std::string& to)
{
    if (from.empty())
    {
        return;
    }

    size_t start_pos = 0;

    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); //In case `to` contains `from`, like replacing 'x' with 'yx'
    }
}

static std::string find_tag(const std::string& buf, const std::string& start, const std::string& end)
{
    size_t tag = buf.find(start);

    if (tag == std::string::npos)
    {
        //No start tag
        return "";
    }

    size_t tag_start = tag + start.size();
    size_t tag_close = buf.find(end, tag_start);

    if (tag_close == std::string::npos)
    {
        //No close tag
        return "";
    }

    size_t tag_len = tag_close - tag_start;
    std::string value(buf.substr(tag_start, tag_len));

    //Encode special XML entities
    replace_all(value, "&quot;", "\"");
    replace_all(value, "&amp;", "&");
    replace_all(value, "&apos;", "'");
    replace_all(value, "&lt;", "<");
    replace_all(value, "&gt;", ">");

    //Encode general XML entities
    size_t start_pos = 0;
    while ((start_pos = value.find("&#", start_pos)) != std::string::npos)
    {
        if (start_pos + 2 >= value.length())
        {
            return "";
        }

        bool hex = value[start_pos + 2] == 'x';
        size_t end = value.find(';', start_pos);

        if (end == std::string::npos)
        {
            return "";
        }

        size_t real_start = start_pos + 2 + ((int) hex);
        std::string number(value.substr(real_start, end - real_start));

        if (!is_positive_num(number.c_str(), hex))
        {
            return "";
        }

        uint32_t character = 0;
        if (hex)
        {
            SDL_sscanf(number.c_str(), "%" SCNx32, &character);
        }
        else
        {
            SDL_sscanf(number.c_str(), "%" SCNu32, &character);
        }
        value.replace(start_pos, end - start_pos + 1, UTF8_encode(character).bytes);
    }

    return value;
}

#define TAG_FINDER(NAME, TAG) \
static std::string NAME(const std::string& buf) \
{ \
    return find_tag(buf, "<" TAG ">", "</" TAG ">"); \
}

TAG_FINDER(find_metadata, "MetaData") //only for checking that it exists

TAG_FINDER(find_creator, "Creator")
TAG_FINDER(find_title, "Title")
TAG_FINDER(find_desc1, "Desc1")
TAG_FINDER(find_desc2, "Desc2")
TAG_FINDER(find_desc3, "Desc3")
TAG_FINDER(find_website, "website")
TAG_FINDER(find_font, "font")
TAG_FINDER(find_rtl, "rtl")

/* For CliPlaytestArgs */
TAG_FINDER(find_playtest, "Playtest")
TAG_FINDER(find_playx, "playx")
TAG_FINDER(find_playy, "playy")
TAG_FINDER(find_playrx, "playrx")
TAG_FINDER(find_playry, "playry")
TAG_FINDER(find_playgc, "playgc")
TAG_FINDER(find_playmusic, "playmusic")

#undef TAG_FINDER

static void levelMetaDataCallback(const char* filename)
{
    extern customlevelclass cl;
    LevelMetaData temp;
    std::string filename_ = filename;

    if (!endsWith(filename, ".vvvvvv")
    || !FILESYSTEM_isFile(filename)
    || FILESYSTEM_isMounted(filename))
    {
        return;
    }

    if (cl.getLevelMetaData(filename_, temp))
    {
        cl.ListOfMetaData.push_back(temp);
    }
    else
    {
        vlog_warn("Level %s not found :(", filename_.c_str());
    }
}

static void unloadZips(void)
{
    char** list = PHYSFS_getSearchPath();
    if (list == NULL)
    {
        return;
    }
    for (char** path = list; *path != NULL; path++)
    {
        if (SDL_strncmp(*path, "levels/", 7) == 0 && endsWith(*path, ".zip"))
        {
            PHYSFS_unmount(*path);
        }
    }
    PHYSFS_freeList(list);
}

void customlevelclass::getDirectoryData(void)
{

    ListOfMetaData.clear();

    FILESYSTEM_clearLevelDirError();

    unloadZips();

    loadZips();

    FILESYSTEM_enumerateLevelDirFileNames(levelMetaDataCallback);

    for(size_t i = 0; i < ListOfMetaData.size(); i++)
    {
        for(size_t k = 0; k < ListOfMetaData.size(); k++)
        {
            if(compare_nocase(ListOfMetaData[i].title, ListOfMetaData[k].title ))
            {
                std::swap(ListOfMetaData[i] , ListOfMetaData[k]);
            }
        }
    }

}
bool customlevelclass::getLevelMetaDataAndPlaytestArgs(const std::string& _path, LevelMetaData& _data, CliPlaytestArgs* pt_args)
{
    unsigned char *uMem;
    FILESYSTEM_loadFileToMemory(_path.c_str(), &uMem, NULL);

    if (uMem == NULL)
    {
        return false;
    }

    std::string buf((char*) uMem);

    VVV_free(uMem);

    if (find_metadata(buf) == "")
    {
        vlog_warn("Couldn't load metadata for %s", _path.c_str());
        return false;
    }

    _data.creator = find_creator(buf);
    _data.creator_is_gettext = translate_creator(_data.creator);
    _data.title = find_title(buf);
    _data.title_is_gettext = translate_title(_data.title);
    _data.Desc1 = find_desc1(buf);
    _data.Desc2 = find_desc2(buf);
    _data.Desc3 = find_desc3(buf);
    _data.website = find_website(buf);
    if (!font::find_main_font_by_name(find_font(buf).c_str(), &_data.level_main_font_idx))
    {
        _data.level_main_font_idx = font::get_font_idx_8x8();
    }
    _data.rtl = help.Int(find_rtl(buf).c_str());


    if (pt_args != NULL)
    {
        const std::string playtest = find_playtest(buf);
        if (playtest == "")
        {
            pt_args->valid = false;
        }
        else
        {
            pt_args->valid = true;
            pt_args->x = help.Int(find_playx(playtest).c_str());
            pt_args->y = help.Int(find_playy(playtest).c_str());
            pt_args->rx = help.Int(find_playrx(playtest).c_str());
            pt_args->ry = help.Int(find_playry(playtest).c_str());
            pt_args->gc = help.Int(find_playgc(playtest).c_str());
            pt_args->music = help.Int(find_playmusic(playtest).c_str());
        }
    }

    _data.filename = _path;
    return true;
}

bool customlevelclass::getLevelMetaData(const std::string& _path, LevelMetaData& _data)
{
    return getLevelMetaDataAndPlaytestArgs(_path, _data, NULL);
}

void customlevelclass::reset(void)
{
    version=2; //New smaller format change is 2

    mapwidth=5;
    mapheight=5;

    title="Untitled Level"; // Already translatable
    creator="Unknown";

    levmusic=0;

    customentities.clear();
    levmusic=0;

    for (int j = 0; j < maxheight; j++)
    {
        for (int i = 0; i < maxwidth; i++)
        {
            roomproperties[i+(j*maxwidth)].tileset=0;
            roomproperties[i+(j*maxwidth)].tilecol=(i+j)%32;
            roomproperties[i+(j*maxwidth)].roomname="";
            roomproperties[i+(j*maxwidth)].warpdir=0;
            roomproperties[i+(j*maxwidth)].platx1=0;
            roomproperties[i+(j*maxwidth)].platy1=0;
            roomproperties[i+(j*maxwidth)].platx2=320;
            roomproperties[i+(j*maxwidth)].platy2=240;
            roomproperties[i+(j*maxwidth)].platv=4;
            roomproperties[i+(j*maxwidth)].enemyx1=0;
            roomproperties[i+(j*maxwidth)].enemyy1=0;
            roomproperties[i+(j*maxwidth)].enemyx2=320;
            roomproperties[i+(j*maxwidth)].enemyy2=240;
            roomproperties[i+(j*maxwidth)].enemytype=0;
            roomproperties[i+(j*maxwidth)].enemyv=0;
            roomproperties[i+(j*maxwidth)].directmode=0;
        }
    }

    SDL_zeroa(contents);

    script.clearcustom();

    onewaycol_override = false;

    script.textbox_colours.clear();
    script.add_default_colours();
    map.specialroomnames.clear();
}

const int* customlevelclass::loadlevel( int rxi, int ryi )
{
    //Set up our buffer array to be picked up by mapclass
    rxi -= 100;
    ryi -= 100;
    if(rxi<0)rxi+=mapwidth;
    if(ryi<0)ryi+=mapheight;
    if(rxi>=mapwidth)rxi-=mapwidth;
    if(ryi>=mapheight)ryi-=mapheight;

    static int result[1200];

    for (int j = 0; j < 30; j++)
    {
        for (int i = 0; i < 40; i++)
        {
            result[i + j*40] = gettile(rxi, ryi, i, j);
        }
    }

    return result;
}

int customlevelclass::getlevelcol(const int tileset, const int tilecol)
{
    if(tileset==0)  //Space Station
    {
        return tilecol;
    }
    else if(tileset==1)   //Outside
    {
        return 32+tilecol;
    }
    else if(tileset==2)   //Lab
    {
        return 40+tilecol;
    }
    else if(tileset==3)   //Warp Zone
    {
        return 46+tilecol;
    }
    else if(tileset==4)   //Ship
    {
        return 52+tilecol;
    }
    return 0;
}

int customlevelclass::getenemycol(int t)
{
    switch(t)
    {
        //RED
    case 3:
    case 7:
    case 12:
    case 23:
    case 28:
    case 34:
    case 42:
    case 48:
    case 58:
        return 6;
        break;
        //GREEN
    case 5:
    case 9:
    case 22:
    case 25:
    case 29:
    case 31:
    case 38:
    case 46:
    case 52:
    case 53:
        return 7;
        break;
        //BLUE
    case 1:
    case 6:
    case 14:
    case 27:
    case 33:
    case 44:
    case 50:
    case 57:
        return 12;
        break;
        //YELLOW
    case 4:
    case 17:
    case 24:
    case 30:
    case 37:
    case 45:
    case 51:
    case 55:
        return 9;
        break;
        //PURPLE
    case 2:
    case 11:
    case 15:
    case 19:
    case 32:
    case 36:
    case 49:
        return 20;
        break;
        //CYAN
    case 8:
    case 10:
    case 13:
    case 18:
    case 26:
    case 35:
    case 41:
    case 47:
    case 54:
        return 11;
        break;
        //PINK
    case 16:
    case 20:
    case 39:
    case 43:
    case 56:
        return 8;
        break;
        //ORANGE
    case 21:
    case 40:
        return 17;
        break;
    default:
        return 6;
        break;
    }
}

int customlevelclass::getwarpbackground(int rx, int ry)
{
    const RoomProperty* const room = getroomprop(rx, ry);
    switch(room->tileset)
    {
    case 0: //Space Station
        switch(room->tilecol)
        {
        case 0:
            return 3;
            break;
        case 1:
            return 2;
            break;
        case 2:
            return 1;
            break;
        case 3:
            return 4;
            break;
        case 4:
            return 5;
            break;
        case 5:
            return 3;
            break;
        case 6:
            return 1;
            break;
        case 7:
            return 0;
            break;
        case 8:
            return 5;
            break;
        case 9:
            return 0;
            break;
        case 10:
            return 2;
            break;
        case 11:
            return 1;
            break;
        case 12:
            return 5;
            break;
        case 13:
            return 0;
            break;
        case 14:
            return 3;
            break;
        case 15:
            return 2;
            break;
        case 16:
            return 4;
            break;
        case 17:
            return 0;
            break;
        case 18:
            return 3;
            break;
        case 19:
            return 1;
            break;
        case 20:
            return 4;
            break;
        case 21:
            return 5;
            break;
        case 22:
            return 1;
            break;
        case 23:
            return 4;
            break;
        case 24:
            return 5;
            break;
        case 25:
            return 0;
            break;
        case 26:
            return 3;
            break;
        case 27:
            return 1;
            break;
        case 28:
            return 5;
            break;
        case 29:
            return 4;
            break;
        case 30:
            return 5;
            break;
        case 31:
            return 2;
            break;
        default:
            return 6;
            break;
        }
        break;
    case 1: //Outside
        switch(room->tilecol)
        {
        case 0:
            return 3;
            break;
        case 1:
            return 1;
            break;
        case 2:
            return 0;
            break;
        case 3:
            return 2;
            break;
        case 4:
            return 4;
            break;
        case 5:
            return 5;
            break;
        case 6:
            return 2;
            break;
        case 7:
            return 4;
            break;
        default:
            return 6;
            break;
        }
        break;
    case 2: //Lab
        switch(room->tilecol)
        {
        case 0:
            return 0;
            break;
        case 1:
            return 1;
            break;
        case 2:
            return 2;
            break;
        case 3:
            return 3;
            break;
        case 4:
            return 4;
            break;
        case 5:
            return 5;
            break;
        case 6:
            return 6;
            break;
        default:
            return 6;
            break;
        }
        break;
    case 3: //Warp Zone
        switch(room->tilecol)
        {
        case 0:
            return 0;
            break;
        case 1:
            return 1;
            break;
        case 2:
            return 2;
            break;
        case 3:
            return 3;
            break;
        case 4:
            return 4;
            break;
        case 5:
            return 5;
            break;
        case 6:
            return 6;
            break;
        default:
            return 6;
            break;
        }
        break;
    case 4: //Ship
        switch(room->tilecol)
        {
        case 0:
            return 5;
            break;
        case 1:
            return 0;
            break;
        case 2:
            return 4;
            break;
        case 3:
            return 2;
            break;
        case 4:
            return 3;
            break;
        case 5:
            return 1;
            break;
        case 6:
            return 6;
            break;
        default:
            return 6;
            break;
        }
        break;
    case 5: //Tower
        return 6;
        break;
    default:
        return 6;
        break;
    }
}

int customlevelclass::gettileidx(
    const int rx,
    const int ry,
    const int x,
    const int y
) {
    const int yoff = y + ry*30;
    int mult;
    int idx;

    if (Y_INBOUNDS(yoff))
    {
        mult = VMULT(yoff);
    }
    else
    {
        mult = 0;
    }

    idx = x + rx*40 + mult;

    return idx;
}

void customlevelclass::settile(
    const int rx,
    const int ry,
    const int x,
    const int y,
    const int t
) {
    const int idx = gettileidx(rx, ry, x, y);

    if (!INBOUNDS_ARR(idx, contents))
    {
        return;
    }

    contents[idx] = t;
}

int customlevelclass::gettile(
    const int rx,
    const int ry,
    const int x,
    const int y
) {
    const int idx = gettileidx(rx, ry, x, y);

    if (!INBOUNDS_ARR(idx, contents))
    {
        return 0;
    }

    return contents[idx];
}

int customlevelclass::getabstile(const int x, const int y)
{
    int idx;
    int yoff;

    if (Y_INBOUNDS(y))
    {
        yoff = VMULT(y);
    }
    else
    {
        yoff = 0;
    }

    idx = x + yoff;

    if (!INBOUNDS_ARR(idx, contents))
    {
        return 0;
    }

    return contents[idx];
}


int customlevelclass::getroompropidx(const int rx, const int ry)
{
    return rx + ry*maxwidth;
}

const RoomProperty* customlevelclass::getroomprop(const int rx, const int ry)
{
    const int idx = getroompropidx(rx, ry);

    if (INBOUNDS_ARR(idx, roomproperties))
    {
        return &roomproperties[idx];
    }

    static RoomProperty blank;
    blank.tileset = 1;
    blank.directmode = 1;
    blank.roomname.clear();

    return &blank;
}

#define FOREACH_PROP(NAME, TYPE) \
void customlevelclass::setroom##NAME(const int rx, const int ry, const TYPE NAME) \
{ \
    const int idx = getroompropidx(rx, ry); \
    \
    if (!INBOUNDS_ARR(idx, roomproperties)) \
    { \
        return; \
    } \
    \
    roomproperties[idx].NAME = NAME; \
}

ROOM_PROPERTIES

#undef FOREACH_PROP


int customlevelclass::absfree( int x, int y )
{
    //Returns 0 if tile is not a block, 1 otherwise, abs on grid
    if(x>=0 && y>=0 && x<mapwidth*40 && y<mapheight*30)
    {
        if(getabstile(x, y)==0)
        {
            return 0;
        }
        else
        {
            if(getabstile(x, y)>=2 && getabstile(x, y)<80)
            {
                return 0;
            }
            if(getabstile(x, y)>=680)
            {
                return 0;
            }
        }
    }
    return 1;
}

void customlevelclass::findstartpoint(void)
{
    //Ok! Scan the room for the closest checkpoint
    int testeditor=-1;
    //First up; is there a start point on this screen?
    for(size_t i=0; i<customentities.size(); i++)
    {
        //if() on screen
        if(customentities[i].t==16 && testeditor==-1)
        {
            testeditor=i;
        }
    }

    if(testeditor==-1)
    {
        game.edsavex = 160;
        game.edsavey = 120;
        game.edsaverx = 100;
        game.edsavery = 100;
        game.edsavegc = 0;
        game.edsavey--;
        game.edsavedir=1;
    }
    else
    {
        //Start point spawn
        game.edsavex = (customentities[testeditor].x * 8) - 4;
        game.edsavey = customentities[testeditor].y * 8;
        game.edsaverx = 100 + customentities[testeditor].rx;
        game.edsavery = 100 + customentities[testeditor].ry;
        game.edsavegc = 0;
        game.edsavey++;
        game.edsavedir=1-customentities[testeditor].p1;
    }
}

int customlevelclass::findtrinket(int t)
{
    int ttrinket=0;
    for(int i=0; i<(int)customentities.size(); i++)
    {
        if(i==t) return ttrinket;
        if(customentities[i].t==9) ttrinket++;
    }
    return 0;
}

int customlevelclass::findcrewmate(int t)
{
    int ttrinket=0;
    for(int i=0; i<(int)customentities.size(); i++)
    {
        if(i==t) return ttrinket;
        if(customentities[i].t==15) ttrinket++;
    }
    return 0;
}

int customlevelclass::findwarptoken(int t)
{
    int ttrinket=0;
    for(int i=0; i<(int)customentities.size(); i++)
    {
        if(i==t) return ttrinket;
        if(customentities[i].t==13) ttrinket++;
    }
    return 0;
}


bool customlevelclass::load(std::string _path)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;

    reset();
    ed.reset();

    static const char *levelDir = "levels/";
    if (_path.compare(0, SDL_strlen(levelDir), levelDir) != 0)
    {
        _path = levelDir + _path;
    }

    FILESYSTEM_unmountAssets();
    if (game.cliplaytest && game.playassets != "")
    {
        MAYBE_FAIL(FILESYSTEM_mountAssets(game.playassets.c_str()));
    }
    else
    {
        MAYBE_FAIL(FILESYSTEM_mountAssets(_path.c_str()));
    }

    if (!FILESYSTEM_loadTiXml2Document(_path.c_str(), doc))
    {
        FILESYSTEM_setLevelDirError(
            loc::gettext("Level {path} not found"),
            "path:str",
            _path.c_str()
        );
        goto fail;
    }

    if (doc.Error())
    {
        FILESYSTEM_setLevelDirError(
            loc::gettext("Error parsing {path}: {error}"),
            "path:str, error:str",
            _path.c_str(),
            doc.ErrorStr()
        );
        goto fail;
    }

    ed.loaded_filepath = _path;

    version = 0;
    level_font_name = "font";
    rtl = false;

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

        if (SDL_strcmp(pKey, "MetaData") == 0)
        {

            for( tinyxml2::XMLElement* subElem = pElem->FirstChildElement(); subElem; subElem= subElem->NextSiblingElement())
            {
                const char* pKey_ = subElem->Value();
                const char* pText_ = subElem->GetText() ;
                if(pText_ == NULL)
                {
                    pText_ = "";
                }

                if(SDL_strcmp(pKey_, "Creator") == 0)
                {
                    creator = pText_;
                }

                if(SDL_strcmp(pKey_, "Title") == 0)
                {
                    title = pText_;
                }

                if(SDL_strcmp(pKey_, "Desc1") == 0)
                {
                    Desc1 = pText_;
                }

                if(SDL_strcmp(pKey_, "Desc2") == 0)
                {
                    Desc2 = pText_;
                }

                if(SDL_strcmp(pKey_, "Desc3") == 0)
                {
                    Desc3 = pText_;
                }

                if(SDL_strcmp(pKey_, "website") == 0)
                {
                    website = pText_;
                }

                if(SDL_strcmp(pKey_, "onewaycol_override") == 0)
                {
                    onewaycol_override = help.Int(pText_);
                }

                if(SDL_strcmp(pKey_, "font") == 0)
                {
                    level_font_name = pText_;
                }

                if(SDL_strcmp(pKey_, "rtl") == 0)
                {
                    rtl = help.Int(pText_);
                }
            }
        }

        if (SDL_strcmp(pKey, "mapwidth") == 0)
        {
            mapwidth = help.Int(pText);
        }
        if (SDL_strcmp(pKey, "mapheight") == 0)
        {
            mapheight = help.Int(pText);
        }
        if (SDL_strcmp(pKey, "levmusic") == 0)
        {
            levmusic = help.Int(pText);
        }


        if (SDL_strcmp(pKey, "contents") == 0 && pText[0] != '\0')
        {
            int x = 0;
            int y = 0;

            char buffer[16];
            size_t start = 0;

            while (next_split_s(buffer, sizeof(buffer), &start, pText, ','))
            {
                const int idx = x + maxwidth*40*y;

                if (INBOUNDS_ARR(idx, contents))
                {
                    contents[idx] = help.Int(buffer);
                }

                ++x;

                if (x == mapwidth*40)
                {
                    x = 0;
                    ++y;
                }
            }
        }


        if (SDL_strcmp(pKey, "edEntities") == 0)
        {
            for( tinyxml2::XMLElement* edEntityEl = pElem->FirstChildElement(); edEntityEl; edEntityEl=edEntityEl->NextSiblingElement())
            {
                CustomEntity entity = CustomEntity();
                const char* text = edEntityEl->GetText();
                int global_x = 0;
                int global_y = 0;

                if (text != NULL)
                {
                    size_t len = SDL_strlen(text);

                    // And now we come to the part where we have to deal with
                    // the terrible decisions of the past.
                    //
                    // For some reason, the closing tag of edentities generated
                    // by 2.2 and below has not only been put on a separate
                    // line, but also indented to match with the opening tag as
                    // well. Like this:
                    //
                    //    <edentity ...>contents
                    //    </edentity>
                    //
                    // Instead of doing <edentity ...>contents</edentity>.
                    //
                    // This is COMPLETELY terrible. This requires the XML to be
                    // parsed in an extremely specific and quirky way, which
                    // TinyXML-1 just happened to do.
                    //
                    // TinyXML-2 by default interprets the newline and the next
                    // indentation of whitespace literally, so you end up with
                    // tag contents that has a linefeed plus a bunch of extra
                    // spaces. You can't fix this by setting the whitespace
                    // mode to COLLAPSE_WHITESPACE, that does way more than
                    // TinyXML-1 ever did - it removes the leading whitespace
                    // from things like <edentity ...> this</edentity>, and
                    // collapses XML-encoded whitespace like <edentity ...>
                    // &#32; &#32;this</edentity>, which TinyXML-1 never did.
                    //
                    // Best solution here is to specifically hardcode removing
                    // the linefeed + the extremely specific amount of
                    // whitespace at the end of the contents.

                    if (endsWith(text, "\n            ")) // linefeed + exactly 12 spaces
                    {
                        // 12 spaces + 1 linefeed = 13 chars
                        len -= 13;
                    }

                    entity.scriptname = std::string(text, len);
                }
                edEntityEl->QueryIntAttribute("x", &global_x);
                edEntityEl->QueryIntAttribute("y", &global_y);
                entity.rx = global_x / SCREEN_WIDTH_TILES;
                entity.x = global_x % SCREEN_WIDTH_TILES;
                entity.ry = global_y / SCREEN_HEIGHT_TILES;
                entity.y = global_y % SCREEN_HEIGHT_TILES;
                edEntityEl->QueryIntAttribute("t", &entity.t);

                edEntityEl->QueryIntAttribute("p1", &entity.p1);
                edEntityEl->QueryIntAttribute("p2", &entity.p2);
                edEntityEl->QueryIntAttribute("p3", &entity.p3);
                edEntityEl->QueryIntAttribute("p4", &entity.p4);
                edEntityEl->QueryIntAttribute("p5", &entity.p5);
                edEntityEl->QueryIntAttribute("p6", &entity.p6);

                customentities.push_back(entity);
            }
        }

        if (SDL_strcmp(pKey, "levelMetaData") == 0)
        {
            int i = 0;
            for( tinyxml2::XMLElement* edLevelClassElement = pElem->FirstChildElement(); edLevelClassElement; edLevelClassElement=edLevelClassElement->NextSiblingElement())
            {
                if (!INBOUNDS_ARR(i, roomproperties))
                {
                    continue;
                }

                if(edLevelClassElement->GetText() != NULL)
                {
                    roomproperties[i].roomname = std::string(edLevelClassElement->GetText()) ;
                }

                edLevelClassElement->QueryIntAttribute("tileset", &roomproperties[i].tileset);
                edLevelClassElement->QueryIntAttribute("tilecol", &roomproperties[i].tilecol);
                edLevelClassElement->QueryIntAttribute("platx1", &roomproperties[i].platx1);
                edLevelClassElement->QueryIntAttribute("platy1", &roomproperties[i].platy1);
                edLevelClassElement->QueryIntAttribute("platx2", &roomproperties[i].platx2);
                edLevelClassElement->QueryIntAttribute("platy2", &roomproperties[i].platy2);
                edLevelClassElement->QueryIntAttribute("platv", &roomproperties[i].platv);
                edLevelClassElement->QueryIntAttribute("enemyx1", &roomproperties[i].enemyx1);
                edLevelClassElement->QueryIntAttribute("enemyy1", &roomproperties[i].enemyy1);
                edLevelClassElement->QueryIntAttribute("enemyx2", &roomproperties[i].enemyx2);
                edLevelClassElement->QueryIntAttribute("enemyy2", &roomproperties[i].enemyy2);
                edLevelClassElement->QueryIntAttribute("enemytype", &roomproperties[i].enemytype);
                edLevelClassElement->QueryIntAttribute("enemyv", &roomproperties[i].enemyv);
                edLevelClassElement->QueryIntAttribute("directmode", &roomproperties[i].directmode);

                edLevelClassElement->QueryIntAttribute("warpdir", &roomproperties[i].warpdir);

                i++;

            }
        }

        if (SDL_strcmp(pKey, "script") == 0 && pText[0] != '\0')
        {
            Script script_;
            bool headerfound = false;

            size_t start = 0;
            size_t len = 0;
            size_t prev_start = 0;

            while (next_split(&start, &len, &pText[start], '|'))
            {
                if (len > 0 && pText[prev_start + len - 1] == ':')
                {
                    if (headerfound)
                    {
                        script.customscripts.push_back(script_);
                    }

                    script_.name = std::string(&pText[prev_start], len - 1);
                    script_.contents.clear();
                    headerfound = true;

                    goto next;
                }

                if (headerfound)
                {
                    script_.contents.push_back(std::string(&pText[prev_start], len));
                }

next:
                prev_start = start;
            }

            /* Add the last script */
            if (headerfound)
            {
                script.customscripts.push_back(script_);
            }
        }

        if (SDL_strcmp(pKey, "TextboxColours") == 0)
        {
            for (tinyxml2::XMLElement* textColourElement = pElem->FirstChildElement(); textColourElement; textColourElement = textColourElement->NextSiblingElement())
            {
                if (SDL_strcmp(textColourElement->Value(), "colour") == 0)
                {
                    int r = 255;
                    int g = 255;
                    int b = 255;

                    textColourElement->QueryIntAttribute("r", &r);
                    textColourElement->QueryIntAttribute("g", &g);
                    textColourElement->QueryIntAttribute("b", &b);

                    const char* name = textColourElement->Attribute("name");

                    if (name != NULL)
                    {
                        SDL_Colour colour;
                        colour.r = r;
                        colour.g = g;
                        colour.b = b;

                        script.textbox_colours[name] = colour;
                    }
                }
            }
        }

        if (SDL_strcmp(pKey, "SpecialRoomnames") == 0)
        {
            for (tinyxml2::XMLElement* roomnameElement = pElem->FirstChildElement(); roomnameElement; roomnameElement = roomnameElement->NextSiblingElement())
            {
                const char* roomnameType = roomnameElement->Value();
                Roomname name;
                name.x = 0;
                name.y = 0;
                name.flag = -1;
                name.loop = false;
                name.type = RoomnameType_STATIC;
                name.progress = 0;
                name.delay = 0;
                if (SDL_strcmp(roomnameType, "transform") == 0)
                {
                    name.type = RoomnameType_TRANSFORM;
                    name.delay = 2;
                }
                else if (SDL_strcmp(roomnameType, "glitch") == 0)
                {
                    name.type = RoomnameType_GLITCH;
                    name.progress = 1;
                    name.delay = -1;
                }

                name.text.clear();

                roomnameElement->QueryIntAttribute("x", &name.x);
                roomnameElement->QueryIntAttribute("y", &name.y);
                roomnameElement->QueryIntAttribute("flag", &name.flag);

                roomnameElement->QueryBoolAttribute("loop", &name.loop);

                // Rooms start at (100, 100) instead of (0, 0), so offset the coordinates
                name.x += 100;
                name.y += 100;

                if (name.type == RoomnameType_STATIC)
                {
                    const char* text = roomnameElement->GetText();
                    if (text != NULL)
                    {
                        name.text.push_back(std::string(text));
                    }
                }
                else
                {
                    // Does it have children?
                    if (roomnameElement->FirstChildElement() == NULL)
                    {
                        continue;
                    }
                    for (tinyxml2::XMLElement* textElement = roomnameElement->FirstChildElement(); textElement; textElement = textElement->NextSiblingElement())
                    {
                        if (SDL_strcmp(textElement->Value(), "text") == 0)
                        {
                            const char* text = textElement->GetText();
                            if (text != NULL)
                            {
                                name.text.push_back(std::string(text));
                            }
                        }
                    }
                }

                map.specialroomnames.push_back(name);
            }
        }
    }

    if (mapwidth < maxwidth)
    {
        /* Unscramble platv, since it was stored incorrectly
         * in 2.2 and previous... */
        size_t i;
        int x = 0;
        int y = 0;
        int temp_platv[numrooms];

        for (i = 0; i < numrooms; ++i)
        {
            temp_platv[i] = roomproperties[i].platv;
        }

        for (i = 0; i < numrooms; ++i)
        {
            if (x < mapwidth)
            {
                const int platv_idx = x + y * mapwidth;
                if (INBOUNDS_ARR(platv_idx, temp_platv))
                {
                    roomproperties[i].platv = temp_platv[platv_idx];
                }
            }
            else
            {
                roomproperties[i].platv = 4; /* default */
            }

            ++x;

            if (x >= maxwidth)
            {
                x = 0;
                ++y;
            }
        }
    }

    loc::loadtext_custom(_path.c_str());
    font::load_custom(level_font_name.c_str());

    version=2;

    return true;

fail:
    return false;
}

bool customlevelclass::save(const std::string& _path)
{
    tinyxml2::XMLDocument doc;

    std::string newpath("levels/" + _path);

    // Try to preserve the XML of the currently-loaded one
    bool already_exists = !ed.loaded_filepath.empty()
    && FILESYSTEM_loadTiXml2Document(ed.loaded_filepath.c_str(), doc);
    if (!already_exists && !ed.loaded_filepath.empty())
    {
        vlog_error("Currently-loaded %s not found", ed.loaded_filepath.c_str());
    }

    ed.loaded_filepath = newpath;

    tinyxml2::XMLElement* msg;

    xml::update_declaration(doc);

    tinyxml2::XMLElement * root = xml::update_element(doc, "MapData");
    root->SetAttribute("version",version);

    xml::update_comment(root, " Save file ");

    tinyxml2::XMLElement * data = xml::update_element(root, "Data");

    msg = xml::update_element(data, "MetaData");

    //getUser
    xml::update_tag(msg, "Creator", creator.c_str());

    xml::update_tag(msg, "Title", title.c_str());

    xml::update_tag(msg, "Created", version);

    xml::update_tag(msg, "Modified", modifier.c_str());

    xml::update_tag(msg, "Modifiers", version);

    xml::update_tag(msg, "Desc1", Desc1.c_str());

    xml::update_tag(msg, "Desc2", Desc2.c_str());

    xml::update_tag(msg, "Desc3", Desc3.c_str());

    xml::update_tag(msg, "website", website.c_str());

    if (onewaycol_override)
    {
        xml::update_tag(msg, "onewaycol_override", onewaycol_override);
    }
    else
    {
        // Delete the element. I could just delete one, but just to be sure,
        // I will delete all of them if there are more than one
        tinyxml2::XMLElement* element;
        while ((element = msg->FirstChildElement("onewaycol_override"))
        != NULL)
        {
            doc.DeleteNode(element);
        }
    }

    if (level_font_name != "" && level_font_name != "font")
    {
        xml::update_tag(msg, "font", level_font_name.c_str());
    }
    else
    {
        // Get rid of it completely, same as <onewaycol_override>
        tinyxml2::XMLElement* element;
        while ((element = msg->FirstChildElement("font")) != NULL)
        {
            doc.DeleteNode(element);
        }
    }

    if (rtl)
    {
        xml::update_tag(msg, "rtl", rtl);
    }
    else
    {
        // Also get rid of this one...
        tinyxml2::XMLElement* element;
        while ((element = msg->FirstChildElement("rtl")) != NULL)
        {
            doc.DeleteNode(element);
        }
    }

    xml::update_tag(data, "mapwidth", mapwidth);

    xml::update_tag(data, "mapheight", mapheight);

    xml::update_tag(data, "levmusic", levmusic);

    //New save format
    std::string contentsString="";
    for(int y = 0; y < mapheight*30; y++ )
    {
        for(int x = 0; x < mapwidth*40; x++ )
        {
            contentsString += help.String(getabstile(x, y)) + ",";
        }
    }
    xml::update_tag(data, "contents", contentsString.c_str());


    msg = xml::update_element_delete_contents(data, "edEntities");
    for(size_t i = 0; i < customentities.size(); i++)
    {
        tinyxml2::XMLElement *edentityElement = doc.NewElement( "edentity" );
        const int global_x = customentities[i].rx * SCREEN_WIDTH_TILES + customentities[i].x;
        const int global_y = customentities[i].ry * SCREEN_HEIGHT_TILES + customentities[i].y;
        edentityElement->SetAttribute("x", global_x);
        edentityElement->SetAttribute("y", global_y);
        edentityElement->SetAttribute(  "t", customentities[i].t);
        edentityElement->SetAttribute(  "p1", customentities[i].p1);
        edentityElement->SetAttribute(  "p2", customentities[i].p2);
        edentityElement->SetAttribute(  "p3", customentities[i].p3);
        edentityElement->SetAttribute( "p4", customentities[i].p4);
        edentityElement->SetAttribute( "p5", customentities[i].p5);
        edentityElement->SetAttribute(  "p6", customentities[i].p6);
        edentityElement->LinkEndChild( doc.NewText( customentities[i].scriptname.c_str() )) ;
        msg->LinkEndChild( edentityElement );
    }

    msg = xml::update_element_delete_contents(data, "levelMetaData");

    int temp_platv[numrooms];
    for (size_t i = 0; i < SDL_arraysize(temp_platv); ++i)
    {
        temp_platv[i] = 4; /* default */
    }

    if (mapwidth < maxwidth)
    {
        /* Re-scramble platv, since it was stored incorrectly
         * in 2.2 and previous... */
        size_t i;
        int x = 0;
        int y = 0;
        for (i = 0; i < numrooms; ++i)
        {
            if (x < mapwidth)
            {
                const int platv_idx = x + y * mapwidth;
                if (INBOUNDS_ARR(platv_idx, temp_platv))
                {
                    temp_platv[platv_idx] = roomproperties[i].platv;
                }
            }

            ++x;

            if (x >= mapwidth)
            {
                /* Skip to next actual row. */
                i += maxwidth - mapwidth;
                x = 0;
                ++y;
            }
        }
    }

    for(size_t i = 0; i < SDL_arraysize(roomproperties); i++)
    {
        tinyxml2::XMLElement *roompropertyElement = doc.NewElement( "edLevelClass" );
        roompropertyElement->SetAttribute( "tileset", roomproperties[i].tileset);
        roompropertyElement->SetAttribute(  "tilecol", roomproperties[i].tilecol);
        roompropertyElement->SetAttribute(  "platx1", roomproperties[i].platx1);
        roompropertyElement->SetAttribute(  "platy1", roomproperties[i].platy1);
        roompropertyElement->SetAttribute(  "platx2", roomproperties[i].platx2);
        roompropertyElement->SetAttribute( "platy2", roomproperties[i].platy2);
        roompropertyElement->SetAttribute( "platv", temp_platv[i]);
        roompropertyElement->SetAttribute(  "enemyx1", roomproperties[i].enemyx1);
        roompropertyElement->SetAttribute(  "enemyy1", roomproperties[i].enemyy1);
        roompropertyElement->SetAttribute(  "enemyx2", roomproperties[i].enemyx2);
        roompropertyElement->SetAttribute(  "enemyy2", roomproperties[i].enemyy2);
        roompropertyElement->SetAttribute(  "enemytype", roomproperties[i].enemytype);
        roompropertyElement->SetAttribute( "enemyv", roomproperties[i].enemyv);
        roompropertyElement->SetAttribute(  "directmode", roomproperties[i].directmode);
        roompropertyElement->SetAttribute(  "warpdir", roomproperties[i].warpdir);

        roompropertyElement->LinkEndChild( doc.NewText( roomproperties[i].roomname.c_str() )) ;
        msg->LinkEndChild( roompropertyElement );
    }

    std::string scriptString;
    for(size_t i = 0; i < script.customscripts.size(); i++)
    {
        Script& script_ = script.customscripts[i];

        scriptString += script_.name + ":|";
        for (size_t ii = 0; ii < script_.contents.size(); ++ii)
        {
            scriptString += script_.contents[ii];

            // Inserts a space if the line ends with a :
            if (script_.contents[ii].length() && *script_.contents[ii].rbegin() == ':')
            {
                scriptString += " ";
            }

            scriptString += "|";
        }
    }
    xml::update_tag(data, "script", scriptString.c_str());

    return FILESYSTEM_saveTiXml2Document(newpath.c_str(), doc);
}

void customlevelclass::generatecustomminimap(void)
{
    map.customzoom = 1;
    if (mapwidth <= 10 && mapheight <= 10)
    {
        map.customzoom = 2;
    }
    if (mapwidth <= 5 && mapheight <= 5)
    {
        map.customzoom = 4;
    }

    // Set minimap offsets
    switch (map.customzoom)
    {
    case 4:
        map.custommmxoff = 24 * (5 - mapwidth);
        map.custommmyoff = 18 * (5 - mapheight);
        break;
    case 2:
        map.custommmxoff = 12 * (10 - mapwidth);
        map.custommmyoff = 9 * (10 - mapheight);
        break;
    default:
        map.custommmxoff = 6 * (20 - mapwidth);
        map.custommmyoff = int(4.5 * (20 - mapheight));
        break;
    }

    map.custommmxsize = 240 - (map.custommmxoff * 2);
    map.custommmysize = 180 - (map.custommmyoff * 2);

    // Start drawing the minimap

    SDL_Texture* target = SDL_GetRenderTarget(gameScreen.m_renderer);
    graphics.set_render_target(graphics.images[IMAGE_CUSTOMMINIMAP]);
    graphics.clear();

    // Scan over the map size
    for (int j2 = 0; j2 < mapheight; j2++)
    {
        for (int i2 = 0; i2 < mapwidth; i2++)
        {
            std::vector<SDL_Point> dark_points;
            std::vector<SDL_Point> light_points;

            bool dark = getroomprop(i2, j2)->tileset == 1;

            // Ok, now scan over each square
            for (int j = 0; j < 9 * map.customzoom; j++)
            {
                for (int i = 0; i < 12 * map.customzoom; i++)
                {
                    int tile;
                    switch (map.customzoom)
                    {
                    case 4:
                        tile = absfree(
                            int(i * 0.83) + (i2 * 40),
                            int(j * 0.83) + (j2 * 30)
                        );
                        break;
                    case 2:
                        tile = absfree(
                            int(i * 1.6) + (i2 * 40),
                            int(j * 1.6) + (j2 * 30)
                        );
                        break;
                    default:
                        tile = absfree(
                            3 + (i * 3) + (i2 * 40),
                            (j * 3) + (j2 * 30)
                        );
                        break;
                    }

                    if (tile >= 1)
                    {
                        // Add this pixel
                        SDL_Point point = { (i2 * 12 * map.customzoom) + i, (j2 * 9 * map.customzoom) + j };
                        if (dark)
                        {
                            dark_points.push_back(point);
                        }
                        else
                        {
                            light_points.push_back(point);
                        }
                    }
                }
            }
            // Draw them all at once
            if (!dark_points.empty())
            {
                graphics.draw_points(dark_points.data(), dark_points.size(), 96, 96, 96);
            }
            if (!light_points.empty())
            {
                graphics.draw_points(light_points.data(), light_points.size(), 196, 196, 196);
            }
        }
    }

    graphics.set_render_target(target);
}

// Return a graphics-ready color based off of the given tileset and tilecol
// Much kudos to Dav999 for saving me a lot of work, because I stole these colors from const.lua in Ved! -Info Teddy
SDL_Color customlevelclass::getonewaycol(const int rx, const int ry)
{
    const RoomProperty* const room = getroomprop(rx, ry);
    switch (room->tileset) {

    case 0: // Space Station
        switch (room->tilecol) {
        case -1:
            return graphics.getRGB(109, 109, 109);
        case 0:
            return graphics.getRGB(131, 141, 235);
        case 1:
            return graphics.getRGB(227, 140, 227);
        case 2:
            return graphics.getRGB(242, 126, 151);
        case 3:
            return graphics.getRGB(229, 235, 133);
        case 4:
            return graphics.getRGB(148, 238, 130);
        case 5:
            return graphics.getRGB(140, 165, 227);
        case 6:
            return graphics.getRGB(227, 140, 148);
        case 7:
            return graphics.getRGB(140, 173, 228);
        case 8:
            return graphics.getRGB(142, 235, 137);
        case 9:
            return graphics.getRGB(137, 235, 206);
        case 10:
            return graphics.getRGB(235, 139, 223);
        case 11:
            return graphics.getRGB(238, 130, 138);
        case 12:
            return graphics.getRGB(137, 235, 178);
        case 13:
            return graphics.getRGB(125, 205, 247);
        case 14:
            return graphics.getRGB(190, 137, 235);
        case 15:
            return graphics.getRGB(235, 137, 206);
        case 16:
            return graphics.getRGB(229, 247, 127);
        case 17:
            return graphics.getRGB(127, 200, 247);
        case 18:
            return graphics.getRGB(197, 137, 235);
        case 19:
            return graphics.getRGB(235, 131, 175);
        case 20:
            return graphics.getRGB(242, 210, 123);
        case 21:
            return graphics.getRGB(131, 235, 158);
        case 22:
            return graphics.getRGB(242, 126, 151);
        case 23:
            return graphics.getRGB(219, 243, 123);
        case 24:
            return graphics.getRGB(131, 234, 145);
        case 25:
            return graphics.getRGB(131, 199, 234);
        case 26:
            return graphics.getRGB(141, 131, 234);
        case 27:
            return graphics.getRGB(226, 140, 144);
        case 28:
            return graphics.getRGB(129, 236, 144);
        case 29:
            return graphics.getRGB(235, 231, 131);
        case 30:
            return graphics.getRGB(153, 235, 131);
        case 31:
            return graphics.getRGB(207, 131, 235);
        }
        break;

    case 1: // Outside
        switch (room->tilecol) {
        case 0:
            return graphics.getRGB(57, 86, 140);
        case 1:
            return graphics.getRGB(156, 42, 42);
        case 2:
            return graphics.getRGB(42, 156, 155);
        case 3:
            return graphics.getRGB(125, 36, 162);
        case 4:
            return graphics.getRGB(191, 198, 0);
        case 5:
            return graphics.getRGB(0, 198, 126);
        case 6:
            return graphics.getRGB(224, 110, 177);
        case 7:
            return graphics.getRGB(255, 142, 87);
        }
        break;

    case 2: // Lab
        switch (room->tilecol) {
        case 0:
            return graphics.getRGB(0, 165, 206);
        case 1:
            return graphics.getRGB(206, 5, 0);
        case 2:
            return graphics.getRGB(222, 0, 173);
        case 3:
            return graphics.getRGB(27, 67, 255);
        case 4:
            return graphics.getRGB(194, 206, 0);
        case 5:
            return graphics.getRGB(0, 206, 39);
        case 6:
            return graphics.getRGB(0, 165, 206);
        }
        break;

    case 3: // Warp Zone
        switch (room->tilecol) {
        case 0:
            return graphics.getRGB(113, 178, 197);
        case 1:
            return graphics.getRGB(197, 113, 119);
        case 2:
            return graphics.getRGB(196, 113, 197);
        case 3:
            return graphics.getRGB(149, 113, 197);
        case 4:
            return graphics.getRGB(197, 182, 113);
        case 5:
            return graphics.getRGB(141, 197, 113);
        case 6:
            return graphics.getRGB(109, 109, 109);
        }
        break;

    case 4: // Ship
        switch (room->tilecol) {
        case 0:
            return graphics.getRGB(0, 206, 39);
        case 1:
            return graphics.getRGB(0, 165, 206);
        case 2:
            return graphics.getRGB(194, 206, 0);
        case 3:
            return graphics.getRGB(206, 0, 160);
        case 4:
            return graphics.getRGB(27, 67, 255);
        case 5:
            return graphics.getRGB(206, 5, 0);
        }
        break;

    }

    // Uh, I guess return solid white
    return graphics.getRGB(255, 255, 255);
}

// This version detects the room automatically
SDL_Color customlevelclass::getonewaycol(void)
{
    if (game.gamestate == EDITORMODE)
    {
        return getonewaycol(ed.levx, ed.levy);
    }
    else if (map.custommode)
    {
        return getonewaycol(game.roomx - 100, game.roomy - 100);
    }

    // Uh, I guess return solid white
    return graphics.getRGB(255, 255, 255);
}

static SDL_INLINE bool inbounds(const CustomEntity* entity)
{
    extern customlevelclass cl;
    return entity->x >= 0
    && entity->y >= 0
    && entity->x < cl.mapwidth * SCREEN_WIDTH_TILES
    && entity->y < cl.mapheight * SCREEN_HEIGHT_TILES;
}

int customlevelclass::numtrinkets(void)
{
    int temp = 0;
    for (size_t i = 0; i < customentities.size(); i++)
    {
        if (customentities[i].t == 9 && inbounds(&customentities[i]))
        {
            temp++;
        }
    }
    return temp;
}

int customlevelclass::numcrewmates(void)
{
    int temp = 0;
    for (size_t i = 0; i < customentities.size(); i++)
    {
        if (customentities[i].t == 15 && inbounds(&customentities[i]))
        {
            temp++;
        }
    }
    return temp;
}
