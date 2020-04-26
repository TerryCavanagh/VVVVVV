#if !defined(NO_CUSTOM_LEVELS)

#include "editor.h"

#include "Graphics.h"
#include "Entity.h"
#include "Music.h"
#include "KeyPoll.h"
#include "Map.h"
#include "Script.h"
#include "time.h"

#include "tinyxml.h"

#include "Enums.h"

#include "FileSystemUtils.h"

#include <string>
#include <utf8/unchecked.h>

edlevelclass::edlevelclass()
{
    tileset=0;
    tilecol=0;
    roomname="";
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
    directmode=0;
}

editorclass::editorclass()
{
    maxwidth=20;
    maxheight=20;

    //We create a blank map
    for (int j = 0; j < 30 * maxwidth; j++)
    {
        for (int i = 0; i < 40 * maxheight; i++)
        {
            contents.push_back(0);
        }
    }

    for (int j = 0; j < 30; j++)
    {
        for (int i = 0; i < 40; i++)
        {
            swapmap.push_back(0);
        }
    }

    for (int i = 0; i < 30 * maxheight; i++)
    {
        vmult.push_back(int(i * 40 * maxwidth));
    }

    reset();
}

// comparison, not case sensitive.
bool compare_nocase (std::string first, std::string second)
{
    unsigned int i=0;
    while ( (i<first.length()) && (i<second.length()) )
    {
        if (tolower(first[i])<tolower(second[i]))
            return true;
        else if (tolower(first[i])>tolower(second[i]))
            return false;
        ++i;
    }
    if (first.length()<second.length())
        return true;
    else
        return false;
}

void replace_all(std::string& str, const std::string& from, const std::string& to)
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

std::string find_tag(const std::string& buf, const std::string& start, const std::string& end)
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
        size_t end = value.find(';', start_pos);
        std::string number(value.substr(start_pos + 2, end - start_pos));

        if (!is_positive_num(number))
        {
            return "";
        }

        int character = atoi(number.c_str());
        int utf32[] = {character, 0};
        std::string utf8;
        utf8::unchecked::utf32to8(utf32, utf32 + 1, std::back_inserter(utf8));
        value.replace(start_pos, end - start_pos + 1, utf8);
    }

    return value;
}

#define TAG_FINDER(NAME, TAG) \
std::string NAME(const std::string& buf) \
{ \
    return find_tag(buf, "<" TAG ">", "</" TAG ">"); \
}

TAG_FINDER(find_metadata, "MetaData"); //only for checking that it exists

TAG_FINDER(find_creator, "Creator");
TAG_FINDER(find_title, "Title");
TAG_FINDER(find_desc1, "Desc1");
TAG_FINDER(find_desc2, "Desc2");
TAG_FINDER(find_desc3, "Desc3");
TAG_FINDER(find_website, "website");

#undef TAG_FINDER

void editorclass::getDirectoryData()
{

    ListOfMetaData.clear();
    directoryList.clear();

    directoryList = FILESYSTEM_getLevelDirFileNames();

    for(size_t i = 0; i < directoryList.size(); i++)
    {
        LevelMetaData temp;
        if (getLevelMetaData( directoryList[i], temp))
        {
            ListOfMetaData.push_back(temp);
        }
    }

    for(size_t i = 0; i < ListOfMetaData.size(); i++)
    {
        for(size_t k = 0; k < ListOfMetaData.size(); k++)
        {
            if(compare_nocase(ListOfMetaData[i].title, ListOfMetaData[k].title ))
            {
                std::swap(ListOfMetaData[i] , ListOfMetaData[k]);
                std::swap(directoryList[i], directoryList[k]);
            }
        }
    }

}
bool editorclass::getLevelMetaData(std::string& _path, LevelMetaData& _data )
{
    unsigned char *uMem = NULL;
    FILESYSTEM_loadFileToMemory(_path.c_str(), &uMem, NULL, true);

    if (uMem == NULL)
    {
        printf("Level %s not found :(\n", _path.c_str());
        return false;
    }

    std::string buf((char*) uMem);

    if (find_metadata(buf) == "")
    {
        printf("Couldn't load metadata for %s\n", _path.c_str());
        return false;
    }

    _data.creator = find_creator(buf);
    _data.title = find_title(buf);
    _data.Desc1 = find_desc1(buf);
    _data.Desc2 = find_desc2(buf);
    _data.Desc3 = find_desc3(buf);
    _data.website = find_desc3(buf);

    _data.filename = _path;
    return true;
}

void editorclass::reset()
{
    version=2; //New smaller format change is 2

    mapwidth=5;
    mapheight=5;

    EditorData::GetInstance().title="Untitled Level";
    EditorData::GetInstance().creator="Unknown";
    Desc1="";
    Desc2="";
    Desc3="";
    website="";

    roomnamehide=0;
    zmod=false;
    xmod=false;
    spacemod=false;
    spacemenu=0;
    shiftmenu=false;
    shiftkey=false;
    saveandquit=false;
    note="";
    notedelay=0;
    roomnamemod=false;
    textentry=false;
    savemod=false;
    loadmod=false;
    deletekeyheld=false;

    titlemod=false;
    creatormod=false;
    desc1mod=false;
    desc2mod=false;
    desc3mod=false;
    websitemod=false;
    settingsmod=false;
    warpmod=false; //Two step process
    warpent=-1;

    boundarymod=0;
    boundarytype=0;
    boundx1=0;
    boundx2=0;
    boundy1=0;
    boundy2=0;

    scripttextmod=false;
    textent=0;
    scripttexttype=0;

    drawmode=0;
    dmtile=0;
    dmtileeditor=0;
    entcol=0;

    tilex=0;
    tiley=0;
    levx=0;
    levy=0;
    keydelay=0;
    lclickdelay=0;
    savekey=false;
    loadkey=false;
    updatetiles=true;
    changeroom=true;
    levmusic=0;

    entframe=0;
    entframedelay=0;

    edentity.clear();
    levmusic=0;

    roomtextmod=false;

    for (int j = 0; j < maxheight; j++)
    {
        for (int i = 0; i < maxwidth; i++)
        {
            level[i+(j*maxwidth)].tileset=0;
            level[i+(j*maxwidth)].tilecol=(i+j)%32;
            level[i+(j*maxwidth)].roomname="";
            level[i+(j*maxwidth)].warpdir=0;
            level[i+(j*maxwidth)].platx1=0;
            level[i+(j*maxwidth)].platy1=0;
            level[i+(j*maxwidth)].platx2=320;
            level[i+(j*maxwidth)].platy2=240;
            level[i+(j*maxwidth)].platv=4;
            level[i+(j*maxwidth)].enemyx1=0;
            level[i+(j*maxwidth)].enemyy1=0;
            level[i+(j*maxwidth)].enemyx2=320;
            level[i+(j*maxwidth)].enemyy2=240;
            level[i+(j*maxwidth)].enemytype=0;
            level[i+(j*maxwidth)].directmode=0;
            kludgewarpdir[i+(j*maxwidth)]=0;
        }
    }

    for (int j = 0; j < 30 * maxheight; j++)
    {
        for (int i = 0; i < 40 * maxwidth; i++)
        {
            contents[i+(j*40*maxwidth)]=0;
        }
    }

    hooklist.clear();

    sb.clear();

    clearscriptbuffer();
    sbx=0;
    sby=0;
    pagey=0;
    scripteditmod=false;
    sbscript="null";
    scripthelppage=0;
    scripthelppagedelay=0;

    hookmenupage=0;
    hookmenu=0;
    script.customscript.clear();

    returneditoralpha = 0;
}

void editorclass::gethooks()
{
    //Scan through the script and create a hooks list based on it
    hooklist.clear();
    std::string tstring;
    std::string tstring2;
    for(size_t i=0; i<script.customscript.size(); i++)
    {
        tstring=script.customscript[i];
        if((int) tstring.length() - 1 >= 0) // FIXME: This is sketchy. -flibit
        {
            tstring=tstring[tstring.length()-1];
        }
        else
        {
            tstring="";
        }
        if(tstring==":")
        {
            tstring2="";
            tstring=script.customscript[i];
            for(size_t j=0; j<tstring.length()-1; j++)
            {
                tstring2+=tstring[j];
            }
            hooklist.push_back(tstring2);
        }
    }
}

void editorclass::loadhookineditor(std::string t)
{
    //Find hook t in the scriptclass, then load it into the editor
    clearscriptbuffer();

    std::string tstring;

    bool removemode=false;
    for(size_t i=0; i<script.customscript.size(); i++)
    {
        if(script.customscript[i]==t+":")
        {
            removemode=true;
        }
        else if(removemode)
        {
            tstring=script.customscript[i];
            if(tstring != "")
            {
                tstring = tstring[tstring.length()-1];
            }
            if(tstring==":")
            {
                //this is a hook
                removemode=false;
            }
            else
            {
                //load in this line
                sb.push_back(script.customscript[i]);
            }
        }
    }
    if(sb.empty())
    {
        //Always have one line or we'll have problems
        sb.resize(1);
    }
}

void editorclass::addhooktoscript(std::string t)
{
    //Adds hook+the scriptbuffer to the end of the scriptclass
    removehookfromscript(t);
    script.customscript.push_back(t+":");
    for(size_t i=0; i<sb.size(); i++)
    {
        script.customscript.push_back(sb[i]);
    }
}

void editorclass::removehookfromscript(std::string t)
{
    //Find hook t in the scriptclass, then removes it (and any other code with it)
    std::string tstring;
    bool removemode=false;
    for(size_t i=0; i<script.customscript.size(); i++)
    {
        if(script.customscript[i]==t+":")
        {
            removemode=true;
            //Remove this line
            for(size_t j=i; j<script.customscript.size()-1; j++)
            {
                script.customscript[j]=script.customscript[j+1];
            }
            script.customscript.pop_back();

            i--;
        }
        else if(removemode)
        {
            //If this line is not the start of a new hook, remove it!
            tstring=script.customscript[i];
            if (tstring.length() > 0) {
                tstring=tstring[tstring.length()-1];
            } else {
                tstring="";
            }
            if(tstring==":")
            {
                //this is a hook
                removemode=false;
            }
            else
            {
                //Remove this line
                for(size_t j=i; j<script.customscript.size()-1; j++)
                {
                    script.customscript[j]=script.customscript[j+1];
                }
                script.customscript.pop_back();

                i--;
            }
        }
    }
}

void editorclass::removehook(std::string t)
{
    //Check the hooklist for the hook t. If it's there, remove it from here and the script
    removehookfromscript(t);
    hooklist.erase(std::remove(hooklist.begin(), hooklist.end(), t), hooklist.end());
}

void editorclass::addhook(std::string t)
{
    //Add an empty function to the list in both editor and script
    removehook(t);
    hooklist.push_back(t);
    addhooktoscript(t);
}

bool editorclass::checkhook(std::string t)
{
    //returns true if hook t already is in the list
    for(size_t i=0; i<hooklist.size(); i++)
    {
        if(hooklist[i]==t) return true;
    }
    return false;
}


void editorclass::clearscriptbuffer()
{
    sb.clear();
}

void editorclass::removeline(int t)
{
    //Remove line t from the script
    if((int)sb.size()>1)
    {
        sb.erase(sb.begin() + t);
    }
}

void editorclass::insertline(int t)
{
    //insert a blank line into script at line t
    sb.insert(sb.begin() + t, "");
}

void editorclass::loadlevel( int rxi, int ryi )
{
    //Set up our buffer array to be picked up by mapclass
    rxi -= 100;
    ryi -= 100;
    if(rxi<0)rxi+=mapwidth;
    if(ryi<0)ryi+=mapheight;
    if(rxi>=mapwidth)rxi-=mapwidth;
    if(ryi>=mapheight)ryi-=mapheight;

    for (int j = 0; j < 30; j++)
    {
        for (int i = 0; i < 40; i++)
        {
            swapmap[i+(j*40)]=contents[i+(rxi*40)+vmult[j+(ryi*30)]];
        }
    }
}

int editorclass::getlevelcol(int t)
{
    if(level[t].tileset==0)  //Space Station
    {
        return level[t].tilecol;
    }
    else if(level[t].tileset==1)   //Outside
    {
        return 32+level[t].tilecol;
    }
    else if(level[t].tileset==2)   //Lab
    {
        return 40+level[t].tilecol;
    }
    else if(level[t].tileset==3)   //Warp Zone
    {
        return 46+level[t].tilecol;
    }
    else if(level[t].tileset==4)   //Ship
    {
        return 52+level[t].tilecol;
    }
    return 0;
}

int editorclass::getenemycol(int t)
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
    return 0;
}

int editorclass::getwarpbackground(int rx, int ry)
{
    int tmp=rx+(maxwidth*ry);
    switch(level[tmp].tileset)
    {
    case 0: //Space Station
        switch(level[tmp].tilecol)
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
        switch(level[tmp].tilecol)
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
        switch(level[tmp].tilecol)
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
        switch(level[tmp].tilecol)
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
        switch(level[tmp].tilecol)
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

int editorclass::getenemyframe(int t)
{
    switch(t)
    {
    case 0:
        return 78;
        break;
    case 1:
        return 88;
        break;
    case 2:
        return 36;
        break;
    case 3:
        return 164;
        break;
    case 4:
        return 68;
        break;
    case 5:
        return 48;
        break;
    case 6:
        return 176;
        break;
    case 7:
        return 168;
        break;
    case 8:
        return 112;
        break;
    case 9:
        return 114;
        break;
    default:
        return 78;
        break;
    }
    return 78;
}


void editorclass::placetile( int x, int y, int t )
{
    if(x>=0 && y>=0 && x<mapwidth*40 && y<mapheight*30)
    {
        contents[x+(levx*40)+vmult[y+(levy*30)]]=t;
    }
}

void editorclass::placetilelocal( int x, int y, int t )
{
    if(x>=0 && y>=0 && x<40 && y<30)
    {
        contents[x+(levx*40)+vmult[y+(levy*30)]]=t;
    }
    updatetiles=true;
}

int editorclass::base( int x, int y )
{
    //Return the base tile for the given tileset and colour
    int temp=x+(y*maxwidth);
    if(level[temp].tileset==0)  //Space Station
    {
        if(level[temp].tilecol>=22)
        {
            return 483 + ((level[temp].tilecol-22)*3);
        }
        else if(level[temp].tilecol>=11)
        {
            return 283 + ((level[temp].tilecol-11)*3);
        }
        else
        {
            return 83 + (level[temp].tilecol*3);
        }
    }
    else if(level[temp].tileset==1)   //Outside
    {
        return 480 + (level[temp].tilecol*3);
    }
    else if(level[temp].tileset==2)   //Lab
    {
        return 280 + (level[temp].tilecol*3);
    }
    else if(level[temp].tileset==3)   //Warp Zone/Intermission
    {
        return 80 + (level[temp].tilecol*3);
    }
    else if(level[temp].tileset==4)   //SHIP
    {
        return 101 + (level[temp].tilecol*3);
    }
    return 0;
}

int editorclass::backbase( int x, int y )
{
    //Return the base tile for the background of the given tileset and colour
    int temp=x+(y*maxwidth);
    if(level[temp].tileset==0)  //Space Station
    {
        //Pick depending on tilecol
        switch(level[temp].tilecol)
        {
        case 0:
        case 5:
        case 26:
            return 680; //Blue
            break;
        case 3:
        case 16:
        case 23:
            return 683; //Yellow
            break;
        case 9:
        case 12:
        case 21:
            return 686; //Greeny Cyan
            break;
        case 4:
        case 8:
        case 24:
        case 28:
        case 30:
            return 689; //Green
            break;
        case 20:
        case 29:
            return 692; //Orange
            break;
        case 2:
        case 6:
        case 11:
        case 22:
        case 27:
            return 695; //Red
            break;
        case 1:
        case 10:
        case 15:
        case 19:
        case 31:
            return 698; //Pink
            break;
        case 14:
        case 18:
            return 701; //Dark Blue
            break;
        case 7:
        case 13:
        case 17:
        case 25:
            return 704; //Cyan
            break;
        default:
            return 680;
            break;
        }

    }
    else if(level[temp].tileset==1)   //outside
    {
        return 680 + (level[temp].tilecol*3);
    }
    else if(level[temp].tileset==2)   //Lab
    {
        return 0;
    }
    else if(level[temp].tileset==3)   //Warp Zone/Intermission
    {
        return 120 + (level[temp].tilecol*3);
    }
    else if(level[temp].tileset==4)   //SHIP
    {
        return 741 + (level[temp].tilecol*3);
    }
    return 0;
}

int editorclass::at( int x, int y )
{
    if(x<0) return at(0,y);
    if(y<0) return at(x,0);
    if(x>=40) return at(39,y);
    if(y>=30) return at(x,29);

    if(x>=0 && y>=0 && x<40 && y<30)
    {
        return contents[x+(levx*40)+vmult[y+(levy*30)]];
    }
    return 0;
}


int editorclass::freewrap( int x, int y )
{
    if(x<0) return freewrap(x+(mapwidth*40),y);
    if(y<0) return freewrap(x,y+(mapheight*30));
    if(x>=(mapwidth*40)) return freewrap(x-(mapwidth*40),y);
    if(y>=(mapheight*30)) return freewrap(x,y-(mapheight*30));

    if(x>=0 && y>=0 && x<(mapwidth*40) && y<(mapheight*30))
    {
        if(contents[x+vmult[y]]==0)
        {
            return 0;
        }
        else
        {
            if(contents[x+vmult[y]]>=2 && contents[x+vmult[y]]<80)
            {
                return 0;
            }
            if(contents[x+vmult[y]]>=680)
            {
                return 0;
            }
        }
    }
    return 1;
}

int editorclass::backonlyfree( int x, int y )
{
    //Returns 1 if tile is a background tile, 0 otherwise
    if(x<0) return backonlyfree(0,y);
    if(y<0) return backonlyfree(x,0);
    if(x>=40) return backonlyfree(39,y);
    if(y>=30) return backonlyfree(x,29);

    if(x>=0 && y>=0 && x<40 && y<30)
    {
        if(contents[x+(levx*40)+vmult[y+(levy*30)]]>=680)
        {
            return 1;
        }
    }
    return 0;
}

int editorclass::backfree( int x, int y )
{
    //Returns 0 if tile is not a block or background tile, 1 otherwise
    if(x<0) return backfree(0,y);
    if(y<0) return backfree(x,0);
    if(x>=40) return backfree(39,y);
    if(y>=30) return backfree(x,29);

    if(x>=0 && y>=0 && x<40 && y<30)
    {
        if(contents[x+(levx*40)+vmult[y+(levy*30)]]==0)
        {
            return 0;
        }
    }
    return 1;
}

int editorclass::spikefree( int x, int y )
{
    //Returns 0 if tile is not a block or spike, 1 otherwise
    if(x==-1) return free(0,y);
    if(y==-1) return free(x,0);
    if(x==40) return free(39,y);
    if(y==30) return free(x,29);

    if(x>=0 && y>=0 && x<40 && y<30)
    {
        if(contents[x+(levx*40)+vmult[y+(levy*30)]]==0)
        {
            return 0;
        }
        else
        {
            if(contents[x+(levx*40)+vmult[y+(levy*30)]]>=680)
            {
                return 0;
            }
        }
    }
    return 1;
}

int editorclass::free( int x, int y )
{
    //Returns 0 if tile is not a block, 1 otherwise
    if(x==-1) return free(0,y);
    if(y==-1) return free(x,0);
    if(x==40) return free(39,y);
    if(y==30) return free(x,29);

    if(x>=0 && y>=0 && x<40 && y<30)
    {
        if(contents[x+(levx*40)+vmult[y+(levy*30)]]==0)
        {
            return 0;
        }
        else
        {
            if(contents[x+(levx*40)+vmult[y+(levy*30)]]>=2 && contents[x+(levx*40)+vmult[y+(levy*30)]]<80)
            {
                return 0;
            }
            if(contents[x+(levx*40)+vmult[y+(levy*30)]]>=680)
            {
                return 0;
            }
        }
    }
    return 1;
}

int editorclass::absfree( int x, int y )
{
    //Returns 0 if tile is not a block, 1 otherwise, abs on grid
    if(x>=0 && y>=0 && x<mapwidth*40 && y<mapheight*30)
    {
        if(contents[x+vmult[y]]==0)
        {
            return 0;
        }
        else
        {
            if(contents[x+vmult[y]]>=2 && contents[x+vmult[y]]<80)
            {
                return 0;
            }
            if(contents[x+vmult[y]]>=680)
            {
                return 0;
            }
        }
    }
    return 1;
}

int editorclass::match( int x, int y )
{
    if(free(x-1,y)==0 && free(x,y-1)==0 && free(x+1,y)==0 && free(x,y+1)==0) return 0;

    if(free(x-1,y)==0 && free(x,y-1)==0) return 10;
    if(free(x+1,y)==0 && free(x,y-1)==0) return 11;
    if(free(x-1,y)==0 && free(x,y+1)==0) return 12;
    if(free(x+1,y)==0 && free(x,y+1)==0) return 13;

    if(free(x,y-1)==0) return 1;
    if(free(x-1,y)==0) return 2;
    if(free(x,y+1)==0) return 3;
    if(free(x+1,y)==0) return 4;
    if(free(x-1,y-1)==0) return 5;
    if(free(x+1,y-1)==0) return 6;
    if(free(x-1,y+1)==0) return 7;
    if(free(x+1,y+1)==0) return 8;

    return 0;
}

int editorclass::warpzonematch( int x, int y )
{
    if(free(x-1,y)==0 && free(x,y-1)==0 && free(x+1,y)==0 && free(x,y+1)==0) return 0;

    if(free(x-1,y)==0 && free(x,y-1)==0) return 10;
    if(free(x+1,y)==0 && free(x,y-1)==0) return 11;
    if(free(x-1,y)==0 && free(x,y+1)==0) return 12;
    if(free(x+1,y)==0 && free(x,y+1)==0) return 13;

    if(free(x,y-1)==0) return 1;
    if(free(x-1,y)==0) return 2;
    if(free(x,y+1)==0) return 3;
    if(free(x+1,y)==0) return 4;
    if(free(x-1,y-1)==0) return 5;
    if(free(x+1,y-1)==0) return 6;
    if(free(x-1,y+1)==0) return 7;
    if(free(x+1,y+1)==0) return 8;

    return 0;
}

int editorclass::outsidematch( int x, int y )
{

    if(backonlyfree(x-1,y)==0 && backonlyfree(x+1,y)==0) return 2;
    if(backonlyfree(x,y-1)==0 && backonlyfree(x,y+1)==0) return 1;

    return 0;
}

int editorclass::backmatch( int x, int y )
{
    //Returns the first position match for a border
    // 5 1 6
    // 2 X 4
    // 7 3 8
    if(backfree(x-1,y)==0 && backfree(x,y-1)==0 && backfree(x+1,y)==0 && backfree(x,y+1)==0) return 0;

    if(backfree(x-1,y)==0 && backfree(x,y-1)==0) return 10;
    if(backfree(x+1,y)==0 && backfree(x,y-1)==0) return 11;
    if(backfree(x-1,y)==0 && backfree(x,y+1)==0) return 12;
    if(backfree(x+1,y)==0 && backfree(x,y+1)==0) return 13;

    if(backfree(x,y-1)==0) return 1;
    if(backfree(x-1,y)==0) return 2;
    if(backfree(x,y+1)==0) return 3;
    if(backfree(x+1,y)==0) return 4;
    if(backfree(x-1,y-1)==0) return 5;
    if(backfree(x+1,y-1)==0) return 6;
    if(backfree(x-1,y+1)==0) return 7;
    if(backfree(x+1,y+1)==0) return 8;

    return 0;
}

int editorclass::edgetile( int x, int y )
{
    switch(match(x,y))
    {
    case 14:
        return 0;
        break;
    case 10:
        return 80;
        break;
    case 11:
        return 82;
        break;
    case 12:
        return 160;
        break;
    case 13:
        return 162;
        break;
    case 1:
        return 81;
        break;
    case 2:
        return 120;
        break;
    case 3:
        return 161;
        break;
    case 4:
        return 122;
        break;
    case 5:
        return 42;
        break;
    case 6:
        return 41;
        break;
    case 7:
        return 2;
        break;
    case 8:
        return 1;
        break;
    case 0:
    default:
        return 0;
        break;
    }
    return 0;
}

int editorclass::warpzoneedgetile( int x, int y )
{
    switch(backmatch(x,y))
    {
    case 14:
        return 0;
        break;
    case 10:
        return 80;
        break;
    case 11:
        return 82;
        break;
    case 12:
        return 160;
        break;
    case 13:
        return 162;
        break;
    case 1:
        return 81;
        break;
    case 2:
        return 120;
        break;
    case 3:
        return 161;
        break;
    case 4:
        return 122;
        break;
    case 5:
        return 42;
        break;
    case 6:
        return 41;
        break;
    case 7:
        return 2;
        break;
    case 8:
        return 1;
        break;
    case 0:
    default:
        return 0;
        break;
    }
    return 0;
}

int editorclass::outsideedgetile( int x, int y )
{
    switch(outsidematch(x,y))
    {
    case 2:
        return 0;
        break;
    case 1:
        return 1;
        break;
    case 0:
    default:
        return 2;
        break;
    }
    return 2;
}


int editorclass::backedgetile( int x, int y )
{
    switch(backmatch(x,y))
    {
    case 14:
        return 0;
        break;
    case 10:
        return 80;
        break;
    case 11:
        return 82;
        break;
    case 12:
        return 160;
        break;
    case 13:
        return 162;
        break;
    case 1:
        return 81;
        break;
    case 2:
        return 120;
        break;
    case 3:
        return 161;
        break;
    case 4:
        return 122;
        break;
    case 5:
        return 42;
        break;
    case 6:
        return 41;
        break;
    case 7:
        return 2;
        break;
    case 8:
        return 1;
        break;
    case 0:
    default:
        return 0;
        break;
    }
    return 0;
}

int editorclass::labspikedir( int x, int y, int t )
{
    // a slightly more tricky case
    if(free(x,y+1)==1) return 63 + (t*2);
    if(free(x,y-1)==1) return 64 + (t*2);
    if(free(x-1,y)==1) return 51 + (t*2);
    if(free(x+1,y)==1) return 52 + (t*2);
    return 63 + (t*2);
}

int editorclass::spikedir( int x, int y )
{
    if(free(x,y+1)==1) return 8;
    if(free(x,y-1)==1) return 9;
    if(free(x-1,y)==1) return 49;
    if(free(x+1,y)==1) return 50;
    return 8;
}

void editorclass::findstartpoint()
{
    //Ok! Scan the room for the closest checkpoint
    int testeditor=-1;
    //First up; is there a start point on this screen?
    for(size_t i=0; i<edentity.size(); i++)
    {
        //if() on screen
        if(edentity[i].t==16 && testeditor==-1)
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
        int tx=(edentity[testeditor].x-(edentity[testeditor].x%40))/40;
        int ty=(edentity[testeditor].y-(edentity[testeditor].y%30))/30;
        game.edsavex = ((edentity[testeditor].x%40)*8)-4;
        game.edsavey = (edentity[testeditor].y%30)*8;
        game.edsaverx = 100+tx;
        game.edsavery = 100+ty;
        game.edsavegc = 0;
        game.edsavey--;
        game.edsavedir=1-edentity[testeditor].p1;
    }
}

void editorclass::saveconvertor()
{
    //In the case of resizing breaking a level, this function can fix it
    maxwidth=20;
    maxheight=20;
    int oldwidth=10, oldheight=10;

    std::vector <int> tempcontents;
    for (int j = 0; j < 30 * oldwidth; j++)
    {
        for (int i = 0; i < 40 * oldheight; i++)
        {
            tempcontents.push_back(contents[i+(j*40*oldwidth)]);
        }
    }

    contents.clear();
    for (int j = 0; j < 30 * maxheight; j++)
    {
        for (int i = 0; i < 40 * maxwidth; i++)
        {
            contents.push_back(0);
        }
    }

    for (int j = 0; j < 30 * oldheight; j++)
    {
        for (int i = 0; i < 40 * oldwidth; i++)
        {
            contents[i+(j*40*oldwidth)]=tempcontents[i+(j*40*oldwidth)];
        }
    }

    tempcontents.clear();

    for (int i = 0; i < 30 * maxheight; i++)
    {
        vmult.push_back(int(i * 40 * maxwidth));
    }

    for (int j = 0; j < maxheight; j++)
    {
        for (int i = 0; i < maxwidth; i++)
        {
            level[i+(j*maxwidth)].tilecol=(i+j)%6;
        }
    }
    contents.clear();

}

int editorclass::findtrinket(int t)
{
    int ttrinket=0;
    for(int i=0; i<(int)edentity.size(); i++)
    {
        if(i==t) return ttrinket;
        if(edentity[i].t==9) ttrinket++;
    }
    return 0;
}

int editorclass::findcrewmate(int t)
{
    int ttrinket=0;
    for(int i=0; i<(int)edentity.size(); i++)
    {
        if(i==t) return ttrinket;
        if(edentity[i].t==15) ttrinket++;
    }
    return 0;
}

int editorclass::findwarptoken(int t)
{
    int ttrinket=0;
    for(int i=0; i<(int)edentity.size(); i++)
    {
        if(i==t) return ttrinket;
        if(edentity[i].t==13) ttrinket++;
    }
    return 0;
}

void editorclass::load(std::string& _path)
{
    reset();

    static const char *levelDir = "levels/";
    if (_path.compare(0, strlen(levelDir), levelDir) != 0)
    {
        _path = levelDir + _path;
    }

    TiXmlDocument doc;
    if (!FILESYSTEM_loadTiXmlDocument(_path.c_str(), &doc))
    {
        printf("No level %s to load :(\n", _path.c_str());
        return;
    }


    TiXmlHandle hDoc(&doc);
    TiXmlElement* pElem;
    TiXmlHandle hRoot(0);
    version = 0;

    {
        pElem=hDoc.FirstChildElement().Element();
        // should always have a valid root but handle gracefully if it does
        if (!pElem)
        {
            printf("No valid root! Corrupt level file?\n");
        }

        pElem->QueryIntAttribute("version", &version);
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

        if (pKey == "MetaData")
        {

            for( TiXmlElement* subElem = pElem->FirstChildElement(); subElem; subElem= subElem->NextSiblingElement())
            {
                std::string pKey(subElem->Value());
                const char* pText = subElem->GetText() ;
                if(pText == NULL)
                {
                    pText = "";
                }

                if(pKey == "Creator")
                {
                    EditorData::GetInstance().creator = pText;
                }

                if(pKey == "Title")
                {
                    EditorData::GetInstance().title = pText;
                }

                if(pKey == "Desc1")
                {
                    Desc1 = pText;
                }

                if(pKey == "Desc2")
                {
                    Desc2 = pText;
                }

                if(pKey == "Desc3")
                {
                    Desc3 = pText;
                }

                if(pKey == "website")
                {
                    website = pText;
                }
            }
        }

        if (pKey == "mapwidth")
        {
            mapwidth = atoi(pText);
        }
        if (pKey == "mapheight")
        {
            mapheight = atoi(pText);
        }
        if (pKey == "levmusic")
        {
            levmusic = atoi(pText);
        }


        if (pKey == "contents")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,',');
                //contents.clear();
                for(size_t i = 0; i < contents.size(); i++)
                {
                    contents[i] =0;
                }
                int x =0;
                int y =0;
                for(size_t i = 0; i < values.size(); i++)
                {
                    contents[x + (maxwidth*40*y)] = atoi(values[i].c_str());
                    x++;
                    if(x == mapwidth*40)
                    {
                        x=0;
                        y++;
                    }

                }
            }
        }

        /*else if(version==1){
          if (pKey == "contents")
          {
            std::string TextString = (pText);
            if(TextString.length())
            {
              std::vector<std::string> values = split(TextString,',');
              contents.clear();
              for(int i = 0; i < values.size(); i++)
              {
                contents.push_back(atoi(values[i].c_str()));
              }
            }
          }
        //}
        */


        if (pKey == "edEntities")
        {
            for( TiXmlElement* edEntityEl = pElem->FirstChildElement(); edEntityEl; edEntityEl=edEntityEl->NextSiblingElement())
            {
                edentities entity;

                std::string pKey(edEntityEl->Value());
                if (edEntityEl->GetText() != NULL)
                {
                    entity.scriptname = std::string(edEntityEl->GetText());
                }
                edEntityEl->QueryIntAttribute("x", &entity.x);
                edEntityEl->QueryIntAttribute("y", &entity.y);
                edEntityEl->QueryIntAttribute("t", &entity.t);

                edEntityEl->QueryIntAttribute("p1", &entity.p1);
                edEntityEl->QueryIntAttribute("p2", &entity.p2);
                edEntityEl->QueryIntAttribute("p3", &entity.p3);
                edEntityEl->QueryIntAttribute("p4", &entity.p4);
                edEntityEl->QueryIntAttribute("p5", &entity.p5);
                edEntityEl->QueryIntAttribute("p6", &entity.p6);

                edentity.push_back(entity);
            }
        }

        if (pKey == "levelMetaData")
        {
            int i = 0;
            for( TiXmlElement* edLevelClassElement = pElem->FirstChildElement(); edLevelClassElement; edLevelClassElement=edLevelClassElement->NextSiblingElement())
            {
                std::string pKey(edLevelClassElement->Value());
                if(edLevelClassElement->GetText() != NULL)
                {
                    level[i].roomname = std::string(edLevelClassElement->GetText()) ;
                }

                edLevelClassElement->QueryIntAttribute("tileset", &level[i].tileset);
                edLevelClassElement->QueryIntAttribute("tilecol", &level[i].tilecol);
                edLevelClassElement->QueryIntAttribute("platx1", &level[i].platx1);
                edLevelClassElement->QueryIntAttribute("platy1", &level[i].platy1);
                edLevelClassElement->QueryIntAttribute("platx2", &level[i].platx2);
                edLevelClassElement->QueryIntAttribute("platy2", &level[i].platy2);
                edLevelClassElement->QueryIntAttribute("platv", &level[i].platv);
                edLevelClassElement->QueryIntAttribute("enemyx1", &level[i].enemyx1);
                edLevelClassElement->QueryIntAttribute("enemyy1", &level[i].enemyy1);
                edLevelClassElement->QueryIntAttribute("enemyx2", &level[i].enemyx2);
                edLevelClassElement->QueryIntAttribute("enemyy2", &level[i].enemyy2);
                edLevelClassElement->QueryIntAttribute("enemytype", &level[i].enemytype);
                edLevelClassElement->QueryIntAttribute("directmode", &level[i].directmode);

                edLevelClassElement->QueryIntAttribute("warpdir", &level[i].warpdir);

                i++;

            }
        }

        if (pKey == "script")
        {
            std::string TextString = (pText);
            if(TextString.length())
            {
                std::vector<std::string> values = split(TextString,'|');
                script.clearcustom();
                for(size_t i = 0; i < values.size(); i++)
                {
                    script.customscript.push_back(values[i]);
                }

            }
        }

    }

    gethooks();
    version=2;
}

void editorclass::save(std::string& _path)
{
    TiXmlDocument doc;
    TiXmlElement* msg;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );

    TiXmlElement * root = new TiXmlElement( "MapData" );
    root->SetAttribute("version",version);
    doc.LinkEndChild( root );

    TiXmlComment * comment = new TiXmlComment();
    comment->SetValue(" Save file " );
    root->LinkEndChild( comment );

    TiXmlElement * data = new TiXmlElement( "Data" );
    root->LinkEndChild( data );

    msg = new TiXmlElement( "MetaData" );

    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    std::string timeAndDate = asctime (timeinfo);

    EditorData::GetInstance().timeModified =  timeAndDate;
    if(EditorData::GetInstance().timeModified == "")
    {
        EditorData::GetInstance().timeCreated =  timeAndDate;
    }

    //getUser
    TiXmlElement* meta = new TiXmlElement( "Creator" );
    meta->LinkEndChild( new TiXmlText( EditorData::GetInstance().creator.c_str() ));
    msg->LinkEndChild( meta );

    meta = new TiXmlElement( "Title" );
    meta->LinkEndChild( new TiXmlText( EditorData::GetInstance().title.c_str() ));
    msg->LinkEndChild( meta );

    meta = new TiXmlElement( "Created" );
    meta->LinkEndChild( new TiXmlText( help.String(version).c_str() ));
    msg->LinkEndChild( meta );

    meta = new TiXmlElement( "Modified" );
    meta->LinkEndChild( new TiXmlText( EditorData::GetInstance().modifier.c_str() ) );
    msg->LinkEndChild( meta );

    meta = new TiXmlElement( "Modifiers" );
    meta->LinkEndChild( new TiXmlText( help.String(version).c_str() ));
    msg->LinkEndChild( meta );

    meta = new TiXmlElement( "Desc1" );
    meta->LinkEndChild( new TiXmlText( Desc1.c_str() ));
    msg->LinkEndChild( meta );

    meta = new TiXmlElement( "Desc2" );
    meta->LinkEndChild( new TiXmlText( Desc2.c_str() ));
    msg->LinkEndChild( meta );

    meta = new TiXmlElement( "Desc3" );
    meta->LinkEndChild( new TiXmlText( Desc3.c_str() ));
    msg->LinkEndChild( meta );

    meta = new TiXmlElement( "website" );
    meta->LinkEndChild( new TiXmlText( website.c_str() ));
    msg->LinkEndChild( meta );

    data->LinkEndChild( msg );

    msg = new TiXmlElement( "mapwidth" );
    msg->LinkEndChild( new TiXmlText( help.String(mapwidth).c_str() ));
    data->LinkEndChild( msg );

    msg = new TiXmlElement( "mapheight" );
    msg->LinkEndChild( new TiXmlText( help.String(mapheight).c_str() ));
    data->LinkEndChild( msg );

    msg = new TiXmlElement( "levmusic" );
    msg->LinkEndChild( new TiXmlText( help.String(levmusic).c_str() ));
    data->LinkEndChild( msg );

    //New save format
    std::string contentsString="";
    for(int y = 0; y < mapheight*30; y++ )
    {
        for(int x = 0; x < mapwidth*40; x++ )
        {
            contentsString += help.String(contents[x + (maxwidth*40*y)]) + ",";
        }
    }
    msg = new TiXmlElement( "contents" );
    msg->LinkEndChild( new TiXmlText( contentsString.c_str() ));
    data->LinkEndChild( msg );


    msg = new TiXmlElement( "edEntities" );
    for(size_t i = 0; i < edentity.size(); i++)
    {
        TiXmlElement *edentityElement = new TiXmlElement( "edentity" );
        edentityElement->SetAttribute( "x", edentity[i].x);
        edentityElement->SetAttribute(  "y", edentity[i].y);
        edentityElement->SetAttribute(  "t", edentity[i].t);
        edentityElement->SetAttribute(  "p1", edentity[i].p1);
        edentityElement->SetAttribute(  "p2", edentity[i].p2);
        edentityElement->SetAttribute(  "p3", edentity[i].p3);
        edentityElement->SetAttribute( "p4", edentity[i].p4);
        edentityElement->SetAttribute( "p5", edentity[i].p5);
        edentityElement->SetAttribute(  "p6", edentity[i].p6);
        edentityElement->LinkEndChild( new TiXmlText( edentity[i].scriptname.c_str() )) ;
        msg->LinkEndChild( edentityElement );
    }

    data->LinkEndChild( msg );

    msg = new TiXmlElement( "levelMetaData" );
    for(int i = 0; i < 400; i++)
    {
        TiXmlElement *edlevelclassElement = new TiXmlElement( "edLevelClass" );
        edlevelclassElement->SetAttribute( "tileset", level[i].tileset);
        edlevelclassElement->SetAttribute(  "tilecol", level[i].tilecol);
        edlevelclassElement->SetAttribute(  "platx1", level[i].platx1);
        edlevelclassElement->SetAttribute(  "platy1", level[i].platy1);
        edlevelclassElement->SetAttribute(  "platx2", level[i].platx2);
        edlevelclassElement->SetAttribute( "platy2", level[i].platy2);
        edlevelclassElement->SetAttribute( "platv", level[i].platv);
        edlevelclassElement->SetAttribute(  "enemyx1", level[i].enemyx1);
        edlevelclassElement->SetAttribute(  "enemyy1", level[i].enemyy1);
        edlevelclassElement->SetAttribute(  "enemyx2", level[i].enemyx2);
        edlevelclassElement->SetAttribute(  "enemyy2", level[i].enemyy2);
        edlevelclassElement->SetAttribute(  "enemytype", level[i].enemytype);
        edlevelclassElement->SetAttribute(  "directmode", level[i].directmode);
        edlevelclassElement->SetAttribute(  "warpdir", level[i].warpdir);

        edlevelclassElement->LinkEndChild( new TiXmlText( level[i].roomname.c_str() )) ;
        msg->LinkEndChild( edlevelclassElement );
    }
    data->LinkEndChild( msg );

    std::string scriptString;
    for(size_t i = 0; i < script.customscript.size(); i++ )
    {
        scriptString += script.customscript[i] + "|";
    }
    msg = new TiXmlElement( "script" );
    msg->LinkEndChild( new TiXmlText( scriptString.c_str() ));
    data->LinkEndChild( msg );

    FILESYSTEM_saveTiXmlDocument(("levels/" + _path).c_str(), &doc);
}


void addedentity( int xp, int yp, int tp, int p1/*=0*/, int p2/*=0*/, int p3/*=0*/, int p4/*=0*/, int p5/*=320*/, int p6/*=240*/)
{
    edentities entity;

    entity.x=xp;
    entity.y=yp;
    entity.t=tp;
    entity.p1=p1;
    entity.p2=p2;
    entity.p3=p3;
    entity.p4=p4;
    entity.p5=p5;
    entity.p6=p6;
    entity.scriptname="";

    edentity.push_back(entity);
}

void removeedentity( int t )
{
    edentity.erase(edentity.begin() + t);
}

int edentat( int xp, int yp )
{
    for(size_t i=0; i<edentity.size(); i++)
    {
        if(edentity[i].x==xp && edentity[i].y==yp) return i;
    }
    return -1;
}

bool edentclear( int xp, int yp )
{
    for(size_t i=0; i<edentity.size(); i++)
    {
        if(edentity[i].x==xp && edentity[i].y==yp) return false;
    }
    return true;
}

void fillbox( int x, int y, int x2, int y2, int c )
{
    FillRect(graphics.backBuffer, x, y, x2-x, 1, c);
    FillRect(graphics.backBuffer, x, y2-1, x2-x, 1, c);
    FillRect(graphics.backBuffer, x, y, 1, y2-y, c);
    FillRect(graphics.backBuffer, x2-1, y, 1, y2-y, c);
}

void fillboxabs( int x, int y, int x2, int y2, int c )
{
    FillRect(graphics.backBuffer, x, y, x2, 1, c);
    FillRect(graphics.backBuffer, x, y+y2-1, x2, 1, c);
    FillRect(graphics.backBuffer, x, y, 1, y2, c);
    FillRect(graphics.backBuffer, x+x2-1, y, 1, y2, c);
}


extern editorclass ed;

extern scriptclass script;

void editorclass::generatecustomminimap()
{
    map.customwidth=mapwidth;
    map.customheight=mapheight;

    map.customzoom=1;
    if(map.customwidth<=10 && map.customheight<=10) map.customzoom=2;
    if(map.customwidth<=5 && map.customheight<=5) map.customzoom=4;

    //Set minimap offsets
    if(map.customzoom==4)
    {
        map.custommmxoff=24*(5-map.customwidth);
        map.custommmxsize=240-(map.custommmxoff*2);

        map.custommmyoff=18*(5-map.customheight);
        map.custommmysize=180-(map.custommmyoff*2);
    }
    else if(map.customzoom==2)
    {
        map.custommmxoff=12*(10-map.customwidth);
        map.custommmxsize=240-(map.custommmxoff*2);

        map.custommmyoff=9*(10-map.customheight);
        map.custommmysize=180-(map.custommmyoff*2);
    }
    else
    {
        map.custommmxoff=6*(20-map.customwidth);
        map.custommmxsize=240-(map.custommmxoff*2);

        map.custommmyoff=int(4.5*(20-map.customheight));
        map.custommmysize=180-(map.custommmyoff*2);
    }

    FillRect(graphics.images[12], graphics.getRGB(0,0,0));

    int tm=0;
    int temp=0;
    //Scan over the map size
    if(ed.mapheight<=5 && ed.mapwidth<=5)
    {
        //4x map
        for(int j2=0; j2<ed.mapheight; j2++)
        {
            for(int i2=0; i2<ed.mapwidth; i2++)
            {
                //Ok, now scan over each square
                tm=196;
                if(ed.level[i2 + (j2*ed.maxwidth)].tileset==1) tm=96;

                for(int j=0; j<36; j++)
                {
                    for(int i=0; i<48; i++)
                    {
                        temp=ed.absfree(int(i*0.83) + (i2*40),int(j*0.83)+(j2*30));
                        if(temp>=1)
                        {
                            //Fill in this pixel
                            FillRect(graphics.images[12], (i2*48)+i, (j2*36)+j, 1, 1, graphics.getRGB(tm, tm, tm));
                        }
                    }
                }
            }
        }
    }
    else if(ed.mapheight<=10 && ed.mapwidth<=10)
    {
        //2x map
        for(int j2=0; j2<ed.mapheight; j2++)
        {
            for(int i2=0; i2<ed.mapwidth; i2++)
            {
                //Ok, now scan over each square
                tm=196;
                if(ed.level[i2 + (j2*ed.maxwidth)].tileset==1) tm=96;

                for(int j=0; j<18; j++)
                {
                    for(int i=0; i<24; i++)
                    {
                        temp=ed.absfree(int(i*1.6) + (i2*40),int(j*1.6)+(j2*30));
                        if(temp>=1)
                        {
                            //Fill in this pixel
                            FillRect(graphics.images[12], (i2*24)+i, (j2*18)+j, 1, 1, graphics.getRGB(tm, tm, tm));
                        }
                    }
                }
            }
        }
    }
    else
    {
        for(int j2=0; j2<ed.mapheight; j2++)
        {
            for(int i2=0; i2<ed.mapwidth; i2++)
            {
                //Ok, now scan over each square
                tm=196;
                if(ed.level[i2 + (j2*ed.maxwidth)].tileset==1) tm=96;

                for(int j=0; j<9; j++)
                {
                    for(int i=0; i<12; i++)
                    {
                        temp=ed.absfree(3+(i*3) + (i2*40),(j*3)+(j2*30));
                        if(temp>=1)
                        {
                            //Fill in this pixel
                            FillRect(graphics.images[12], (i2*12)+i, (j2*9)+j, 1, 1, graphics.getRGB(tm, tm, tm));
                        }
                    }
                }
            }
        }
    }
}

void editormenurender(int tr, int tg, int tb)
{
    switch (game.currentmenuname)
    {
    case Menu::ed_settings:
        graphics.bigprint( -1, 75, "Map Settings", tr, tg, tb, true);
        break;
    case Menu::ed_desc:
        if(ed.titlemod)
        {
            if(ed.entframe<2)
            {
                graphics.bigprint( -1, 35, key.keybuffer+"_", tr, tg, tb, true);
            }
            else
            {
                graphics.bigprint( -1, 35, key.keybuffer+" ", tr, tg, tb, true);
            }
        }
        else
        {
            graphics.bigprint( -1, 35, EditorData::GetInstance().title, tr, tg, tb, true);
        }
        if(ed.creatormod)
        {
            if(ed.entframe<2)
            {
                graphics.Print( -1, 60, "by " + key.keybuffer+ "_", tr, tg, tb, true);
            }
            else
            {
                graphics.Print( -1, 60, "by " + key.keybuffer+ " ", tr, tg, tb, true);
            }
        }
        else
        {
            graphics.Print( -1, 60, "by " + EditorData::GetInstance().creator, tr, tg, tb, true);
        }
        if(ed.websitemod)
        {
            if(ed.entframe<2)
            {
                graphics.Print( -1, 70, key.keybuffer+"_", tr, tg, tb, true);
            }
            else
            {
                graphics.Print( -1, 70, key.keybuffer+" ", tr, tg, tb, true);
            }
        }
        else
        {
            graphics.Print( -1, 70, ed.website, tr, tg, tb, true);
        }
        if(ed.desc1mod)
        {
            if(ed.entframe<2)
            {
                graphics.Print( -1, 90, key.keybuffer+"_", tr, tg, tb, true);
            }
            else
            {
                graphics.Print( -1, 90, key.keybuffer+" ", tr, tg, tb, true);
            }
        }
        else
        {
            graphics.Print( -1, 90, ed.Desc1, tr, tg, tb, true);
        }
        if(ed.desc2mod)
        {
            if(ed.entframe<2)
            {
                graphics.Print( -1, 100, key.keybuffer+"_", tr, tg, tb, true);
            }
            else
            {
                graphics.Print( -1, 100, key.keybuffer+" ", tr, tg, tb, true);
            }
        }
        else
        {
            graphics.Print( -1, 100, ed.Desc2, tr, tg, tb, true);
        }
        if(ed.desc3mod)
        {
            if(ed.entframe<2)
            {
                graphics.Print( -1, 110, key.keybuffer+"_", tr, tg, tb, true);
            }
            else
            {
                graphics.Print( -1, 110, key.keybuffer+" ", tr, tg, tb, true);
            }
        }
        else
        {
            graphics.Print( -1, 110, ed.Desc3, tr, tg, tb, true);
        }
        break;
    case Menu::ed_music:
        graphics.bigprint( -1, 65, "Map Music", tr, tg, tb, true);

        graphics.Print( -1, 85, "Current map music:", tr, tg, tb, true);
        switch(ed.levmusic)
        {
        case 0:
            graphics.Print( -1, 120, "No background music", tr, tg, tb, true);
            break;
        case 1:
            graphics.Print( -1, 120, "1: Pushing Onwards", tr, tg, tb, true);
            break;
        case 2:
            graphics.Print( -1, 120, "2: Positive Force", tr, tg, tb, true);
            break;
        case 3:
            graphics.Print( -1, 120, "3: Potential for Anything", tr, tg, tb, true);
            break;
        case 4:
            graphics.Print( -1, 120, "4: Passion for Exploring", tr, tg, tb, true);
            break;
        case 6:
            graphics.Print( -1, 120, "5: Presenting VVVVVV", tr, tg, tb, true);
            break;
        case 8:
            graphics.Print( -1, 120, "6: Predestined Fate", tr, tg, tb, true);
            break;
        case 10:
            graphics.Print( -1, 120, "7: Popular Potpourri", tr, tg, tb, true);
            break;
        case 11:
            graphics.Print( -1, 120, "8: Pipe Dream", tr, tg, tb, true);
            break;
        case 12:
            graphics.Print( -1, 120, "9: Pressure Cooker", tr, tg, tb, true);
            break;
        case 13:
            graphics.Print( -1, 120, "10: Paced Energy", tr, tg, tb, true);
            break;
        case 14:
            graphics.Print( -1, 120, "11: Piercing the Sky", tr, tg, tb, true);
            break;
        default:
            graphics.Print( -1, 120, "?: something else", tr, tg, tb, true);
            break;
        }
        break;
    case Menu::ed_quit:
        graphics.bigprint( -1, 90, "Save before", tr, tg, tb, true);
        graphics.bigprint( -1, 110, "quitting?", tr, tg, tb, true);
        break;
    default:
        break;
    }
}

void editorrender()
{
    //Draw grid

    FillRect(graphics.backBuffer, 0, 0, 320,240, graphics.getRGB(0,0,0));
    for(int j=0; j<30; j++)
    {
        for(int i=0; i<40; i++)
        {
            fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(8,8,8)); //a simple grid
            if(i%4==0) fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(16,16,16));
            if(j%4==0) fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(16,16,16));

            //Minor guides
            if(i==9) fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(24,24,24));
            if(i==30) fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(24,24,24));
            if(j==6 || j==7) fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(24,24,24));
            if(j==21 || j==22) fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(24,24,24));

            //Major guides
            if(i==20 || i==19) fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(32,32,32));
            if(j==14) fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(32,32,32));
        }
    }

    //Or draw background
    if(!ed.settingsmod)
    {
        switch(ed.level[ed.levx+(ed.levy*ed.maxwidth)].warpdir)
        {
        case 1:
            graphics.rcol=ed.getwarpbackground(ed.levx, ed.levy);
            graphics.drawbackground(3);
            break;
        case 2:
            graphics.rcol=ed.getwarpbackground(ed.levx, ed.levy);
            graphics.drawbackground(4);
            break;
        case 3:
            graphics.rcol=ed.getwarpbackground(ed.levx, ed.levy);
            graphics.drawbackground(5);
            break;
        default:
            break;
        }
    }

    //Draw map, in function
    int temp;
    if(ed.level[ed.levx+(ed.maxwidth*ed.levy)].tileset==0 || ed.level[ed.levx+(ed.maxwidth*ed.levy)].tileset==10)
    {
        for (int j = 0; j < 30; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                temp=ed.contents[i + (ed.levx*40) + ed.vmult[j+(ed.levy*30)]];
                if(temp>0) graphics.drawtile(i*8,j*8,temp);
            }
        }
    }
    else
    {
        for (int j = 0; j < 30; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                temp=ed.contents[i + (ed.levx*40) + ed.vmult[j+(ed.levy*30)]];
                if(temp>0) graphics.drawtile2(i*8,j*8,temp);
            }
        }
    }

    //Edge tile fix

    //Buffer the sides of the new room with tiles from other rooms, to ensure no gap problems.
    for(int j=0; j<30; j++)
    {
        //left edge
        if(ed.freewrap((ed.levx*40)-1,j+(ed.levy*30))==1)
        {
            FillRect(graphics.backBuffer, 0,j*8, 2,8, graphics.getRGB(255,255,255-help.glow));
        }
        //right edge
        if(ed.freewrap((ed.levx*40)+40,j+(ed.levy*30))==1)
        {
            FillRect(graphics.backBuffer, 318,j*8, 2,8, graphics.getRGB(255,255,255-help.glow));
        }
    }

    for(int i=0; i<40; i++)
    {
        if(ed.freewrap((ed.levx*40)+i,(ed.levy*30)-1)==1)
        {
            FillRect(graphics.backBuffer, i*8,0, 8,2, graphics.getRGB(255,255,255-help.glow));
        }

        if(ed.freewrap((ed.levx*40)+i,30+(ed.levy*30))==1)
        {
            FillRect(graphics.backBuffer, i*8,238, 8,2, graphics.getRGB(255,255,255-help.glow));
        }
    }

    //Draw entities
    game.customcol=ed.getlevelcol(ed.levx+(ed.levy*ed.maxwidth))+1;
    ed.entcol=ed.getenemycol(game.customcol);
    obj.customplatformtile=game.customcol*12;

    ed.temp=edentat(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30));

    // Draw entities backward to remain accurate with ingame
    for (int i = edentity.size() - 1; i >= 0; i--)
    {
        //if() on screen
        int tx=(edentity[i].x-(edentity[i].x%40))/40;
        int ty=(edentity[i].y-(edentity[i].y%30))/30;

        point tpoint;
        SDL_Rect drawRect;

        if(tx==ed.levx && ty==ed.levy)
        {
            switch(edentity[i].t)
            {
            case 1: //Entities
                graphics.drawspritesetcol((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),ed.getenemyframe(ed.level[ed.levx+(ed.levy*ed.maxwidth)].enemytype),ed.entcol);
                if(edentity[i].p1==0) graphics.Print((edentity[i].x*8)- (ed.levx*40*8)+4,(edentity[i].y*8)- (ed.levy*30*8)+4, "V", 255, 255, 255 - help.glow, false);
                if(edentity[i].p1==1) graphics.Print((edentity[i].x*8)- (ed.levx*40*8)+4,(edentity[i].y*8)- (ed.levy*30*8)+4, "^", 255, 255, 255 - help.glow, false);
                if(edentity[i].p1==2) graphics.Print((edentity[i].x*8)- (ed.levx*40*8)+4,(edentity[i].y*8)- (ed.levy*30*8)+4, "<", 255, 255, 255 - help.glow, false);
                if(edentity[i].p1==3) graphics.Print((edentity[i].x*8)- (ed.levx*40*8)+4,(edentity[i].y*8)- (ed.levy*30*8)+4, ">", 255, 255, 255 - help.glow, false);
                fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),16,16,graphics.getBGR(255,164,255));
                break;
            case 2: //Threadmills & platforms
                tpoint.x = (edentity[i].x*8)- (ed.levx*40*8);
                tpoint.y = (edentity[i].y*8)- (ed.levy*30*8);
                drawRect = graphics.tiles_rect;
                drawRect.x += tpoint.x;
                drawRect.y += tpoint.y;
                BlitSurfaceStandard(graphics.entcolours[obj.customplatformtile],NULL, graphics.backBuffer, &drawRect);
                drawRect.x += 8;
                BlitSurfaceStandard(graphics.entcolours[obj.customplatformtile],NULL, graphics.backBuffer, &drawRect);
                drawRect.x += 8;
                BlitSurfaceStandard(graphics.entcolours[obj.customplatformtile],NULL, graphics.backBuffer, &drawRect);
                drawRect.x += 8;
                BlitSurfaceStandard(graphics.entcolours[obj.customplatformtile],NULL,graphics.backBuffer, &drawRect);

                if(edentity[i].p1<=4)
                {
                    if(edentity[i].p1==0) graphics.Print((edentity[i].x*8)- (ed.levx*40*8)+12,(edentity[i].y*8)- (ed.levy*30*8), "V", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                    if(edentity[i].p1==1) graphics.Print((edentity[i].x*8)- (ed.levx*40*8)+12,(edentity[i].y*8)- (ed.levy*30*8), "^", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                    if(edentity[i].p1==2) graphics.Print((edentity[i].x*8)- (ed.levx*40*8)+12,(edentity[i].y*8)- (ed.levy*30*8), "<", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                    if(edentity[i].p1==3) graphics.Print((edentity[i].x*8)- (ed.levx*40*8)+12,(edentity[i].y*8)- (ed.levy*30*8), ">", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                    fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),32,8,graphics.getBGR(255,255,255));
                }

                if(edentity[i].p1==5)
                {
                    graphics.Print((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8), ">>>>", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                    fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),32,8,graphics.getBGR(255,255,255));
                }
                else if(edentity[i].p1==6)
                {
                    graphics.Print((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8), "<<<<", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                    fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),32,8,graphics.getBGR(255,255,255));
                }

                if(edentity[i].p1>=7)
                {
                    tpoint.x = (edentity[i].x*8)- (ed.levx*40*8)+32;
                    tpoint.y = (edentity[i].y*8)- (ed.levy*30*8);
                    drawRect = graphics.tiles_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceStandard(graphics.entcolours[obj.customplatformtile],NULL, graphics.backBuffer, &drawRect);
                    drawRect.x += 8;
                    BlitSurfaceStandard(graphics.entcolours[obj.customplatformtile],NULL, graphics.backBuffer, &drawRect);
                    drawRect.x += 8;
                    BlitSurfaceStandard(graphics.entcolours[obj.customplatformtile],NULL, graphics.backBuffer, &drawRect);
                    drawRect.x += 8;
                    BlitSurfaceStandard(graphics.entcolours[obj.customplatformtile],NULL,graphics.backBuffer, &drawRect);

                }

                if(edentity[i].p1==7)
                {
                    graphics.Print((edentity[i].x*8)- (ed.levx*40*8)+4,(edentity[i].y*8)- (ed.levy*30*8), "> > > > ", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                    fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),64,8,graphics.getBGR(255,255,255));
                }
                else if(edentity[i].p1==8)
                {
                    graphics.Print((edentity[i].x*8)- (ed.levx*40*8)+4,(edentity[i].y*8)- (ed.levy*30*8), "< < < < ", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                    fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),64,8,graphics.getBGR(255,255,255));
                }
                break;
            case 3: //Disappearing Platform
                tpoint.x = (edentity[i].x*8)- (ed.levx*40*8);
                tpoint.y = (edentity[i].y*8)- (ed.levy*30*8);
                drawRect = graphics.tiles_rect;
                drawRect.x += tpoint.x;
                drawRect.y += tpoint.y;
                BlitSurfaceStandard(graphics.entcolours[obj.customplatformtile],NULL, graphics.backBuffer, &drawRect);
                drawRect.x += 8;
                BlitSurfaceStandard(graphics.entcolours[obj.customplatformtile],NULL, graphics.backBuffer, &drawRect);
                drawRect.x += 8;
                BlitSurfaceStandard(graphics.entcolours[obj.customplatformtile],NULL, graphics.backBuffer, &drawRect);
                drawRect.x += 8;
                BlitSurfaceStandard(graphics.entcolours[obj.customplatformtile],NULL,graphics.backBuffer, &drawRect);

                graphics.Print((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8), "////", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),32,8,graphics.getBGR(255,255,255));
                break;
            case 9: //Shiny Trinket
                graphics.drawsprite((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),22,196,196,196);
                fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),16,16,graphics.getRGB(164,164,255));
                break;
            case 10: //Checkpoints
                if(edentity[i].p1==0)  //From roof
                {
                    graphics.drawsprite((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),20,196,196,196);
                }
                else if(edentity[i].p1==1)   //From floor
                {
                    graphics.drawsprite((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),21,196,196,196);
                }
                fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),16,16,graphics.getRGB(164,164,255));
                break;
            case 11: //Gravity lines
                if(edentity[i].p1==0)  //Horizontal
                {
                    int tx=edentity[i].x-(ed.levx*40);
                    int tx2=edentity[i].x-(ed.levx*40);
                    int ty=edentity[i].y-(ed.levy*30);
                    while(ed.spikefree(tx,ty)==0) tx--;
                    while(ed.spikefree(tx2,ty)==0) tx2++;
                    tx++;
                    FillRect(graphics.backBuffer, (tx*8),(ty*8)+4, (tx2-tx)*8,1, graphics.getRGB(194,194,194));
                    fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),8,8,graphics.getRGB(164,255,164));
                    edentity[i].p2=tx;
                    edentity[i].p3=(tx2-tx)*8;
                }
                else  //Vertical
                {
                    int tx=edentity[i].x-(ed.levx*40);
                    int ty=edentity[i].y-(ed.levy*30);
                    int ty2=edentity[i].y-(ed.levy*30);
                    while(ed.spikefree(tx,ty)==0) ty--;
                    while(ed.spikefree(tx,ty2)==0) ty2++;
                    ty++;
                    FillRect(graphics.backBuffer, (tx*8)+3,(ty*8), 1,(ty2-ty)*8, graphics.getRGB(194,194,194));
                    fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),8,8,graphics.getRGB(164,255,164));
                    edentity[i].p2=ty;
                    edentity[i].p3=(ty2-ty)*8;
                }
                break;
            case 13://Warp tokens
                graphics.drawsprite((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),18+(ed.entframe%2),196,196,196);
                fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),16,16,graphics.getRGB(164,164,255));
                if(ed.temp==i)
                {
                    graphics.bprint((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8)-8,
                                "("+help.String(((edentity[i].p1-int(edentity[i].p1%40))/40)+1)+","+help.String(((edentity[i].p2-int(edentity[i].p2%30))/30)+1)+")",210,210,255);
                }
                else
                {
                    graphics.bprint((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8)-8,help.String(ed.findwarptoken(i)),210,210,255);
                }
                break;
            case 15: //Crewmates
                graphics.drawspritesetcol((edentity[i].x*8)- (ed.levx*40*8)-4,(edentity[i].y*8)- (ed.levy*30*8),144,obj.crewcolour(edentity[i].p1));
                fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),16,24,graphics.getRGB(164,164,164));
                break;
            case 16: //Start
                if(edentity[i].p1==0)  //Left
                {
                    graphics.drawspritesetcol((edentity[i].x*8)- (ed.levx*40*8)-4,(edentity[i].y*8)- (ed.levy*30*8),0,obj.crewcolour(0));
                }
                else if(edentity[i].p1==1)
                {
                    graphics.drawspritesetcol((edentity[i].x*8)- (ed.levx*40*8)-4,(edentity[i].y*8)- (ed.levy*30*8),3,obj.crewcolour(0));
                }
                fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),16,24,graphics.getRGB(164,255,255));
                if(ed.entframe<2)
                {
                    graphics.bprint((edentity[i].x*8)- (ed.levx*40*8)-12,(edentity[i].y*8)- (ed.levy*30*8)-8,"START",255,255,255);
                }
                else
                {
                    graphics.bprint((edentity[i].x*8)- (ed.levx*40*8)-12,(edentity[i].y*8)- (ed.levy*30*8)-8,"START",196,196,196);
                }
                break;
            case 17: //Roomtext
                if(edentity[i].scriptname.length()<1)
                {
                    fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),8,8,graphics.getRGB(96,96,96));
                }
                else
                {
                    int length = utf8::unchecked::distance(edentity[i].scriptname.begin(), edentity[i].scriptname.end());
                    fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),length*8,8,graphics.getRGB(96,96,96));
                }
                graphics.Print((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8), edentity[i].scriptname, 196, 196, 255 - help.glow);
                break;
            case 18: //Terminals
                graphics.drawsprite((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8)+8,17,96,96,96);
                fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),16,24,graphics.getRGB(164,164,164));
                if(ed.temp==i)
                {
                    graphics.bprint((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8)-8,edentity[i].scriptname,210,210,255);
                }
                break;
            case 19: //Script Triggers
                fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),edentity[i].p1*8,edentity[i].p2*8,graphics.getRGB(255,164,255));
                fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),8,8,graphics.getRGB(255,255,255));
                if(ed.temp==i)
                {
                    graphics.bprint((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8)-8,edentity[i].scriptname,210,210,255);
                }
                break;
            case 50: //Warp lines
                if(edentity[i].p1>=2)  //Horizontal
                {
                    int tx=edentity[i].x-(ed.levx*40);
                    int tx2=edentity[i].x-(ed.levx*40);
                    int ty=edentity[i].y-(ed.levy*30);
                    while(ed.free(tx,ty)==0) tx--;
                    while(ed.free(tx2,ty)==0) tx2++;
                    tx++;
                    fillboxabs((tx*8),(ty*8)+1, (tx2-tx)*8,6, graphics.getRGB(255,255,194));
                    fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),8,8,graphics.getRGB(255,255,164));
                    edentity[i].p2=tx;
                    edentity[i].p3=(tx2-tx)*8;
                }
                else  //Vertical
                {
                    int tx=edentity[i].x-(ed.levx*40);
                    int ty=edentity[i].y-(ed.levy*30);
                    int ty2=edentity[i].y-(ed.levy*30);
                    while(ed.free(tx,ty)==0) ty--;
                    while(ed.free(tx,ty2)==0) ty2++;
                    ty++;
                    fillboxabs((tx*8)+1,(ty*8), 6,(ty2-ty)*8, graphics.getRGB(255,255,194));
                    fillboxabs((edentity[i].x*8)- (ed.levx*40*8),(edentity[i].y*8)- (ed.levy*30*8),8,8,graphics.getRGB(255,255,164));
                    edentity[i].p2=ty;
                    edentity[i].p3=(ty2-ty)*8;
                }
                break;
            }
        }

        //Need to also check warp point destinations
        if(edentity[i].t==13 && ed.warpent!=i)
        {
            tx=(edentity[i].p1-(edentity[i].p1%40))/40;
            ty=(edentity[i].p2-(edentity[i].p2%30))/30;
            if(tx==ed.levx && ty==ed.levy)
            {
                graphics.drawsprite((edentity[i].p1*8)- (ed.levx*40*8),(edentity[i].p2*8)- (ed.levy*30*8),18+(ed.entframe%2),64,64,64);
                fillboxabs((edentity[i].p1*8)- (ed.levx*40*8),(edentity[i].p2*8)- (ed.levy*30*8),16,16,graphics.getRGB(64,64,96));
                if(ed.tilex+(ed.levx*40)==edentity[i].p1 && ed.tiley+(ed.levy*30)==edentity[i].p2)
                {
                    graphics.bprint((edentity[i].p1*8)- (ed.levx*40*8),(edentity[i].p2*8)- (ed.levy*30*8)-8,
                                "("+help.String(((edentity[i].x-int(edentity[i].x%40))/40)+1)+","+help.String(((edentity[i].y-int(edentity[i].y%30))/30)+1)+")",190,190,225);
                }
                else
                {
                    graphics.bprint((edentity[i].p1*8)- (ed.levx*40*8),(edentity[i].p2*8)- (ed.levy*30*8)-8,help.String(ed.findwarptoken(i)),190,190,225);
                }
            }
        }
    }

    if(ed.boundarymod>0)
    {
        if(ed.boundarymod==1)
        {
            fillboxabs(ed.tilex*8, ed.tiley*8, 8,8,graphics.getRGB(255-(help.glow/2),191+(help.glow),210+(help.glow/2)));
            fillboxabs((ed.tilex*8)+2, (ed.tiley*8)+2, 4,4,graphics.getRGB(128-(help.glow/4),100+(help.glow/2),105+(help.glow/4)));
        }
        else if(ed.boundarymod==2)
        {
            if((ed.tilex*8)+8<=ed.boundx1 || (ed.tiley*8)+8<=ed.boundy1)
            {
                fillboxabs(ed.boundx1, ed.boundy1, 8, 8,graphics.getRGB(255-(help.glow/2),191+(help.glow),210+(help.glow/2)));
                fillboxabs(ed.boundx1+2, ed.boundy1+2, 4, 4,graphics.getRGB(128-(help.glow/4),100+(help.glow/2),105+(help.glow/4)));
            }
            else
            {
                fillboxabs(ed.boundx1, ed.boundy1, (ed.tilex*8)+8-ed.boundx1,(ed.tiley*8)+8-ed.boundy1,graphics.getRGB(255-(help.glow/2),191+(help.glow),210+(help.glow/2)));
                fillboxabs(ed.boundx1+2, ed.boundy1+2, (ed.tilex*8)+8-ed.boundx1-4,(ed.tiley*8)+8-ed.boundy1-4,graphics.getRGB(128-(help.glow/4),100+(help.glow/2),105+(help.glow/4)));
            }
        }
    }
    else
    {
        //Draw boundaries
        int tmp=ed.levx+(ed.levy*ed.maxwidth);
        if(ed.level[tmp].enemyx1!=0 && ed.level[tmp].enemyy1!=0
                && ed.level[tmp].enemyx2!=320 && ed.level[tmp].enemyy2!=240)
        {
            fillboxabs( ed.level[tmp].enemyx1, ed.level[tmp].enemyy1,
                       ed.level[tmp].enemyx2-ed.level[tmp].enemyx1,
                       ed.level[tmp].enemyy2-ed.level[tmp].enemyy1,
                       graphics.getBGR(255-(help.glow/2),64,64));
        }

        if(ed.level[tmp].platx1!=0 && ed.level[tmp].platy1!=0
                && ed.level[tmp].platx2!=320 && ed.level[tmp].platy2!=240)
        {
            fillboxabs( ed.level[tmp].platx1, ed.level[tmp].platy1,
                       ed.level[tmp].platx2-ed.level[tmp].platx1,
                       ed.level[tmp].platy2-ed.level[tmp].platy1,
                       graphics.getBGR(64,64,255-(help.glow/2)));
        }
    }

    //Draw Cursor
    switch(ed.drawmode)
    {
    case 0:
    case 1:
    case 2:
    case 9:
    case 10:
    case 12: //Single point
        fillboxabs((ed.tilex*8),(ed.tiley*8),8,8, graphics.getRGB(200,32,32));
        break;
    case 3:
    case 4:
    case 8:
    case 13://2x2
        fillboxabs((ed.tilex*8),(ed.tiley*8),16,16, graphics.getRGB(200,32,32));
        break;
    case 5:
    case 6:
    case 7://Platform
        fillboxabs((ed.tilex*8),(ed.tiley*8),32,8, graphics.getRGB(200,32,32));
        break;
    case 14: //X if not on edge
        if(ed.tilex==0 || ed.tilex==39 || ed.tiley==0 || ed.tiley==29)
        {
            fillboxabs((ed.tilex*8),(ed.tiley*8),8,8, graphics.getRGB(200,32,32));
        }
        else
        {
            graphics.Print((ed.tilex*8),(ed.tiley*8),"X",255,0,0);
        }
        break;
    case 11:
    case 15:
    case 16: //2x3
        fillboxabs((ed.tilex*8),(ed.tiley*8),16,24, graphics.getRGB(200,32,32));
        break;
    }

    if(ed.drawmode<3)
    {
        if(ed.zmod && ed.drawmode<2)
        {
            fillboxabs((ed.tilex*8)-8,(ed.tiley*8)-8,24,24, graphics.getRGB(200,32,32));
        }
        else if(ed.xmod && ed.drawmode<2)
        {
            fillboxabs((ed.tilex*8)-16,(ed.tiley*8)-16,24+16,24+16, graphics.getRGB(200,32,32));
        }
    }

    //If in directmode, show current directmode tile
    if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].directmode==1)
    {
        //Tile box for direct mode
        int t2=0;
        if(ed.dmtileeditor>0)
        {
            ed.dmtileeditor--;
            if(ed.dmtileeditor<=4)
            {
                t2=(4-ed.dmtileeditor)*12;
            }

            //Draw five lines of the editor
            temp=ed.dmtile-(ed.dmtile%40);
            temp-=80;
            FillRect(graphics.backBuffer, 0,-t2,320,40, graphics.getRGB(0,0,0));
            FillRect(graphics.backBuffer, 0,-t2+40,320,2, graphics.getRGB(255,255,255));
            if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].tileset==0)
            {
                for(int i=0; i<40; i++)
                {
                    graphics.drawtile(i*8,0-t2,(temp+1200+i)%1200);
                    graphics.drawtile(i*8,8-t2,(temp+1200+40+i)%1200);
                    graphics.drawtile(i*8,16-t2,(temp+1200+80+i)%1200);
                    graphics.drawtile(i*8,24-t2,(temp+1200+120+i)%1200);
                    graphics.drawtile(i*8,32-t2,(temp+1200+160+i)%1200);
                }
            }
            else
            {
                for(int i=0; i<40; i++)
                {
                    graphics.drawtile2(i*8,0-t2,(temp+1200+i)%1200);
                    graphics.drawtile2(i*8,8-t2,(temp+1200+40+i)%1200);
                    graphics.drawtile2(i*8,16-t2,(temp+1200+80+i)%1200);
                    graphics.drawtile2(i*8,24-t2,(temp+1200+120+i)%1200);
                    graphics.drawtile2(i*8,32-t2,(temp+1200+160+i)%1200);
                }
            }
            //Highlight our little block
            fillboxabs(((ed.dmtile%40)*8)-2,16-2,12,12,graphics.getRGB(196, 196, 255 - help.glow));
            fillboxabs(((ed.dmtile%40)*8)-1,16-1,10,10,graphics.getRGB(0,0,0));
        }

        if(ed.dmtileeditor>0 && t2<=30)
        {
            graphics.bprint(2, 45-t2, "Tile:", 196, 196, 255 - help.glow, false);
            graphics.bprint(58, 45-t2, help.String(ed.dmtile), 196, 196, 255 - help.glow, false);
            FillRect(graphics.backBuffer, 44,44-t2,10,10, graphics.getRGB(196, 196, 255 - help.glow));
            FillRect(graphics.backBuffer, 45,45-t2,8,8, graphics.getRGB(0,0,0));

            if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].tileset==0)
            {
                graphics.drawtile(45,45-t2,ed.dmtile);
            }
            else
            {
                graphics.drawtile2(45,45-t2,ed.dmtile);
            }
        }
        else
        {
            graphics.bprint(2, 12, "Tile:", 196, 196, 255 - help.glow, false);
            graphics.bprint(58, 12, help.String(ed.dmtile), 196, 196, 255 - help.glow, false);
            FillRect(graphics.backBuffer, 44,11,10,10, graphics.getRGB(196, 196, 255 - help.glow));
            FillRect(graphics.backBuffer, 45,12,8,8, graphics.getRGB(0,0,0));

            if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].tileset==0)
            {
                graphics.drawtile(45,12,ed.dmtile);
            }
            else
            {
                graphics.drawtile2(45,12,ed.dmtile);
            }
        }
    }




    //Draw GUI
    if(ed.boundarymod>0)
    {
        if(ed.boundarymod==1)
        {
            FillRect(graphics.backBuffer, 0,230,320,240, graphics.getRGB(32,32,32));
            FillRect(graphics.backBuffer, 0,231,320,240, graphics.getRGB(0,0,0));
            switch(ed.boundarytype)
            {
            case 0:
                graphics.Print(4, 232, "SCRIPT BOX: Click on top left", 255,255,255, false);
                break;
            case 1:
                graphics.Print(4, 232, "ENEMY BOUNDS: Click on top left", 255,255,255, false);
                break;
            case 2:
                graphics.Print(4, 232, "PLATFORM BOUNDS: Click on top left", 255,255,255, false);
                break;
            case 3:
                graphics.Print(4, 232, "COPY TILES: Click on top left", 255,255,255, false);
                break;
            default:
                graphics.Print(4, 232, "Click on top left", 255,255,255, false);
                break;
            }
        }
        else if(ed.boundarymod==2)
        {
            FillRect(graphics.backBuffer, 0,230,320,240, graphics.getRGB(32,32,32));
            FillRect(graphics.backBuffer, 0,231,320,240, graphics.getRGB(0,0,0));
            switch(ed.boundarytype)
            {
            case 0:
                graphics.Print(4, 232, "SCRIPT BOX: Click on bottom right", 255,255,255, false);
                break;
            case 1:
                graphics.Print(4, 232, "ENEMY BOUNDS: Click on bottom right", 255,255,255, false);
                break;
            case 2:
                graphics.Print(4, 232, "PLATFORM BOUNDS: Click on bottom right", 255,255,255, false);
                break;
            case 3:
                graphics.Print(4, 232, "COPY TILES: Click on bottom right", 255,255,255, false);
                break;
            default:
                graphics.Print(4, 232, "Click on bottom right", 255,255,255, false);
                break;
            }
        }
    }
    else if(ed.scripteditmod)
    {
        //Elaborate C64 BASIC menu goes here!
        FillRect(graphics.backBuffer, 0,0,320,240, graphics.getBGR(123, 111, 218));
        FillRect(graphics.backBuffer, 14,16,292,208, graphics.getRGB(162,48,61));
        switch(ed.scripthelppage)
        {
        case 0:
            graphics.Print(16,28,"**** VVVVVV SCRIPT EDITOR ****", 123, 111, 218, true);
            graphics.Print(16,44,"PRESS ESC TO RETURN TO MENU", 123, 111, 218, true);

            if(!ed.hooklist.empty())
            {
                for(int i=0; i<9; i++)
                {
                    if(ed.hookmenupage+i<(int)ed.hooklist.size())
                    {
                        if(ed.hookmenupage+i==ed.hookmenu)
                        {
                            std::string tstring="> " + ed.hooklist[(ed.hooklist.size()-1)-(ed.hookmenupage+i)] + " <";
                            std::transform(tstring.begin(), tstring.end(),tstring.begin(), ::toupper);
                            graphics.Print(16,68+(i*16),tstring,123, 111, 218, true);
                        }
                        else
                        {
                            graphics.Print(16,68+(i*16),ed.hooklist[(ed.hooklist.size()-1)-(ed.hookmenupage+i)],123, 111, 218, true);
                        }
                    }
                }
            }
            else
            {
                graphics.Print(16,110,"NO SCRIPT IDS FOUND", 123, 111, 218, true);
                graphics.Print(16,130,"CREATE A SCRIPT WITH EITHER", 123, 111, 218, true);
                graphics.Print(16,140,"THE TERMINAL OR SCRIPT BOX TOOLS", 123, 111, 218, true);
            }
            break;
        case 1:
            //Current scriptname
            FillRect(graphics.backBuffer, 14,226,292,12, graphics.getRGB(162,48,61));
            graphics.Print(16,228,"CURRENT SCRIPT: " + ed.sbscript, 123, 111, 218, true);
            //Draw text
            for(int i=0; i<25; i++)
            {
                if(i+ed.pagey<(int)ed.sb.size())
                {
                    graphics.Print(16,20+(i*8),ed.sb[i+ed.pagey], 123, 111, 218, false);
                }
            }
            //Draw cursor
            if(ed.entframe<2)
            {
                graphics.Print(16+(ed.sbx*8),20+(ed.sby*8),"_",123, 111, 218, false);
            }
            break;
        }
    }
    else if(ed.settingsmod)
    {
        if(!game.colourblindmode) graphics.drawtowerbackgroundsolo();

        int tr = map.r - (help.glow / 4) - int(fRandom() * 4);
        int tg = map.g - (help.glow / 4) - int(fRandom() * 4);
        int tb = map.b - (help.glow / 4) - int(fRandom() * 4);
        if (tr < 0) tr = 0;
        if(tr>255) tr=255;
        if (tg < 0) tg = 0;
        if(tg>255) tg=255;
        if (tb < 0) tb = 0;
        if(tb>255) tb=255;
        editormenurender(tr, tg, tb);

        graphics.drawmenu(tr, tg, tb, 15);
    }
    else if(ed.scripttextmod)
    {
        FillRect(graphics.backBuffer, 0,221,320,240, graphics.getRGB(32,32,32));
        FillRect(graphics.backBuffer, 0,222,320,240, graphics.getRGB(0,0,0));
        graphics.Print(4, 224, "Enter script id name:", 255,255,255, false);
        if(ed.entframe<2)
        {
            graphics.Print(4, 232, edentity[ed.textent].scriptname+"_", 196, 196, 255 - help.glow, true);
        }
        else
        {
            graphics.Print(4, 232, edentity[ed.textent].scriptname+" ", 196, 196, 255 - help.glow, true);
        }
    }
    else if(ed.savemod)
    {
        FillRect(graphics.backBuffer, 0,221,320,240, graphics.getRGB(32,32,32));
        FillRect(graphics.backBuffer, 0,222,320,240, graphics.getRGB(0,0,0));
        graphics.Print(4, 224, "Enter filename to save map as:", 255,255,255, false);
        if(ed.entframe<2)
        {
            graphics.Print(4, 232, ed.filename+"_", 196, 196, 255 - help.glow, true);
        }
        else
        {
            graphics.Print(4, 232, ed.filename+" ", 196, 196, 255 - help.glow, true);
        }
    }
    else if(ed.loadmod)
    {
        FillRect(graphics.backBuffer, 0,221,320,240, graphics.getRGB(32,32,32));
        FillRect(graphics.backBuffer, 0,222,320,240, graphics.getRGB(0,0,0));
        graphics.Print(4, 224, "Enter map filename to load:", 255,255,255, false);
        if(ed.entframe<2)
        {
            graphics.Print(4, 232, ed.filename+"_", 196, 196, 255 - help.glow, true);
        }
        else
        {
            graphics.Print(4, 232, ed.filename+" ", 196, 196, 255 - help.glow, true);
        }
    }
    else if(ed.roomnamemod)
    {
        FillRect(graphics.backBuffer, 0,221,320,240, graphics.getRGB(32,32,32));
        FillRect(graphics.backBuffer, 0,222,320,240, graphics.getRGB(0,0,0));
        graphics.Print(4, 224, "Enter new room name:", 255,255,255, false);
        if(ed.entframe<2)
        {
            graphics.Print(4, 232, ed.level[ed.levx+(ed.levy*ed.maxwidth)].roomname+"_", 196, 196, 255 - help.glow, true);
        }
        else
        {
            graphics.Print(4, 232, ed.level[ed.levx+(ed.levy*ed.maxwidth)].roomname+" ", 196, 196, 255 - help.glow, true);
        }
    }
    else if(ed.roomtextmod)
    {
        FillRect(graphics.backBuffer, 0,221,320,240, graphics.getRGB(32,32,32));
        FillRect(graphics.backBuffer, 0,222,320,240, graphics.getRGB(0,0,0));
        graphics.Print(4, 224, "Enter text string:", 255,255,255, false);
        if(ed.entframe<2)
        {
            graphics.Print(4, 232, edentity[ed.textent].scriptname+"_", 196, 196, 255 - help.glow, true);
        }
        else
        {
            graphics.Print(4, 232, edentity[ed.textent].scriptname+" ", 196, 196, 255 - help.glow, true);
        }
    }
    else if(ed.warpmod)
    {
        //placing warp token
        FillRect(graphics.backBuffer, 0,221,320,240, graphics.getRGB(32,32,32));
        FillRect(graphics.backBuffer, 0,222,320,240, graphics.getRGB(0,0,0));
        graphics.Print(4, 224, "Left click to place warp destination", 196, 196, 255 - help.glow, false);
        graphics.Print(4, 232, "Right click to cancel", 196, 196, 255 - help.glow, false);
    }
    else
    {
        if(ed.spacemod)
        {
            FillRect(graphics.backBuffer, 0,208,320,240, graphics.getRGB(32,32,32));
            FillRect(graphics.backBuffer, 0,209,320,240, graphics.getRGB(0,0,0));

            //Draw little icons for each thingy
            int tx=6, ty=211, tg=32;

            if(ed.spacemenu==0)
            {
                for(int i=0; i<10; i++)
                {
                    FillRect(graphics.backBuffer, 4+(i*tg), 209,20,20,graphics.getRGB(32,32,32));
                }
                FillRect(graphics.backBuffer, 4+(ed.drawmode*tg), 209,20,20,graphics.getRGB(64,64,64));
                //0:
                graphics.drawtile(tx,ty,83);
                graphics.drawtile(tx+8,ty,83);
                graphics.drawtile(tx,ty+8,83);
                graphics.drawtile(tx+8,ty+8,83);
                //1:
                tx+=tg;
                graphics.drawtile(tx,ty,680);
                graphics.drawtile(tx+8,ty,680);
                graphics.drawtile(tx,ty+8,680);
                graphics.drawtile(tx+8,ty+8,680);
                //2:
                tx+=tg;
                graphics.drawtile(tx+4,ty+4,8);
                //3:
                tx+=tg;
                graphics.drawsprite(tx,ty,22,196,196,196);
                //4:
                tx+=tg;
                graphics.drawsprite(tx,ty,21,196,196,196);
                //5:
                tx+=tg;
                graphics.drawtile(tx,ty+4,3);
                graphics.drawtile(tx+8,ty+4,4);
                //6:
                tx+=tg;
                graphics.drawtile(tx,ty+4,24);
                graphics.drawtile(tx+8,ty+4,24);
                //7:
                tx+=tg;
                graphics.drawtile(tx,ty+4,1);
                graphics.drawtile(tx+8,ty+4,1);
                //8:
                tx+=tg;
                graphics.drawsprite(tx,ty,78+ed.entframe,196,196,196);
                //9:
                tx+=tg;
                FillRect(graphics.backBuffer, tx+2,ty+8,12,1,graphics.getRGB(255,255,255));

                for(int i=0; i<9; i++)
                {
                    fillboxabs(4+(i*tg), 209,20,20,graphics.getRGB(96,96,96));
                    graphics.Print(22+(i*tg)-4, 225-4,help.String(i+1),164,164,164,false);
                }

                if(ed.drawmode==9)graphics.Print(22+(ed.drawmode*tg)-4, 225-4,"0",255,255,255,false);

                fillboxabs(4+(9*tg), 209,20,20,graphics.getRGB(96,96,96));
                graphics.Print(22+(9*tg)-4, 225-4, "0",164,164,164,false);

                fillboxabs(4+(ed.drawmode*tg), 209,20,20,graphics.getRGB(200,200,200));
                if(ed.drawmode<9)
                {
                    graphics.Print(22+(ed.drawmode*tg)-4, 225-4,help.String(ed.drawmode+1),255,255,255,false);
                }

                graphics.Print(4, 232, "1/2", 196, 196, 255 - help.glow, false);
            }
            else
            {
                for(int i=0; i<7; i++)
                {
                    FillRect(graphics.backBuffer, 4+(i*tg), 209,20,20,graphics.getRGB(32,32,32));
                }
                FillRect(graphics.backBuffer, 4+((ed.drawmode-10)*tg), 209,20,20,graphics.getRGB(64,64,64));
                //10:
                graphics.Print(tx,ty,"A",196, 196, 255 - help.glow, false);
                graphics.Print(tx+8,ty,"B",196, 196, 255 - help.glow, false);
                graphics.Print(tx,ty+8,"C",196, 196, 255 - help.glow, false);
                graphics.Print(tx+8,ty+8,"D",196, 196, 255 - help.glow, false);
                //11:
                tx+=tg;
                graphics.drawsprite(tx,ty,17,196,196,196);
                //12:
                tx+=tg;
                fillboxabs(tx+4,ty+4,8,8,graphics.getRGB(96,96,96));
                //13:
                tx+=tg;
                graphics.drawsprite(tx,ty,18+(ed.entframe%2),196,196,196);
                //14:
                tx+=tg;
                FillRect(graphics.backBuffer, tx+6,ty+2,4,12,graphics.getRGB(255,255,255));
                //15:
                tx+=tg;
                graphics.drawsprite(tx,ty,186,75, 75, 255- help.glow/4 - (fRandom()*20));
                //16:
                tx+=tg;
                graphics.drawsprite(tx,ty,184,160- help.glow/2 - (fRandom()*20), 200- help.glow/2, 220 - help.glow);

                if(ed.drawmode==10)graphics.Print(22+((ed.drawmode-10)*tg)-4, 225-4,"R",255,255,255,false);
                if(ed.drawmode==11)graphics.Print(22+((ed.drawmode-10)*tg)-4, 225-4,"T",255,255,255,false);
                if(ed.drawmode==12)graphics.Print(22+((ed.drawmode-10)*tg)-4, 225-4,"Y",255,255,255,false);
                if(ed.drawmode==13)graphics.Print(22+((ed.drawmode-10)*tg)-4, 225-4,"U",255,255,255,false);
                if(ed.drawmode==14)graphics.Print(22+((ed.drawmode-10)*tg)-4, 225-4,"I",255,255,255,false);
                if(ed.drawmode==15)graphics.Print(22+((ed.drawmode-10)*tg)-4, 225-4,"O",255,255,255,false);
                if(ed.drawmode==16)graphics.Print(22+((ed.drawmode-10)*tg)-4, 225-4,"P",255,255,255,false);

                fillboxabs(4+(0*tg), 209,20,20,graphics.getRGB(96,96,96));
                graphics.Print(22+(0*tg)-4, 225-4, "R",164,164,164,false);
                fillboxabs(4+(1*tg), 209,20,20,graphics.getRGB(96,96,96));
                graphics.Print(22+(1*tg)-4, 225-4, "T",164,164,164,false);
                fillboxabs(4+(2*tg), 209,20,20,graphics.getRGB(96,96,96));
                graphics.Print(22+(2*tg)-4, 225-4, "Y",164,164,164,false);
                fillboxabs(4+(3*tg), 209,20,20,graphics.getRGB(96,96,96));
                graphics.Print(22+(3*tg)-4, 225-4, "U",164,164,164,false);
                fillboxabs(4+(4*tg), 209,20,20,graphics.getRGB(96,96,96));
                graphics.Print(22+(4*tg)-4, 225-4, "I",164,164,164,false);
                fillboxabs(4+(5*tg), 209,20,20,graphics.getRGB(96,96,96));
                graphics.Print(22+(5*tg)-4, 225-4, "O",164,164,164,false);
                fillboxabs(4+(6*tg), 209,20,20,graphics.getRGB(96,96,96));
                graphics.Print(22+(6*tg)-4, 225-4, "P",164,164,164,false);

                graphics.Print(4, 232, "2/2", 196, 196, 255 - help.glow, false);
            }

            graphics.Print(128, 232, "< and > keys change tool", 196, 196, 255 - help.glow, false);

            FillRect(graphics.backBuffer, 0,198,120,10, graphics.getRGB(32,32,32));
            FillRect(graphics.backBuffer, 0,199,119,9, graphics.getRGB(0,0,0));
            switch(ed.drawmode)
            {
            case 0:
                graphics.bprint(2,199, "1: Walls",196, 196, 255 - help.glow);
                break;
            case 1:
                graphics.bprint(2,199, "2: Backing",196, 196, 255 - help.glow);
                break;
            case 2:
                graphics.bprint(2,199, "3: Spikes",196, 196, 255 - help.glow);
                break;
            case 3:
                graphics.bprint(2,199, "4: Trinkets",196, 196, 255 - help.glow);
                break;
            case 4:
                graphics.bprint(2,199, "5: Checkpoint",196, 196, 255 - help.glow);
                break;
            case 5:
                graphics.bprint(2,199, "6: Disappear",196, 196, 255 - help.glow);
                break;
            case 6:
                graphics.bprint(2,199, "7: Conveyors",196, 196, 255 - help.glow);
                break;
            case 7:
                graphics.bprint(2,199, "8: Moving",196, 196, 255 - help.glow);
                break;
            case 8:
                graphics.bprint(2,199, "9: Enemies",196, 196, 255 - help.glow);
                break;
            case 9:
                graphics.bprint(2,199, "0: Grav Line",196, 196, 255 - help.glow);
                break;
            case 10:
                graphics.bprint(2,199, "R: Roomtext",196, 196, 255 - help.glow);
                break;
            case 11:
                graphics.bprint(2,199, "T: Terminal",196, 196, 255 - help.glow);
                break;
            case 12:
                graphics.bprint(2,199, "Y: Script Box",196, 196, 255 - help.glow);
                break;
            case 13:
                graphics.bprint(2,199, "U: Warp Token",196, 196, 255 - help.glow);
                break;
            case 14:
                graphics.bprint(2,199, "I: Warp Lines",196, 196, 255 - help.glow);
                break;
            case 15:
                graphics.bprint(2,199, "O: Crewmate",196, 196, 255 - help.glow);
                break;
            case 16:
                graphics.bprint(2,199, "P: Start Point",196, 196, 255 - help.glow);
                break;
            }

            FillRect(graphics.backBuffer, 260,198,80,10, graphics.getRGB(32,32,32));
            FillRect(graphics.backBuffer, 261,199,80,9, graphics.getRGB(0,0,0));
            graphics.bprint(268,199, "("+help.String(ed.levx+1)+","+help.String(ed.levy+1)+")",196, 196, 255 - help.glow, false);

        }
        else
        {
            //FillRect(graphics.backBuffer, 0,230,72,240, graphics.RGB(32,32,32));
            //FillRect(graphics.backBuffer, 0,231,71,240, graphics.RGB(0,0,0));
            if(ed.level[ed.levx+(ed.maxwidth*ed.levy)].roomname!="")
            {
                if(ed.tiley<28)
                {
                    if(ed.roomnamehide>0) ed.roomnamehide--;
                }
                else
                {
                    if(ed.roomnamehide<12) ed.roomnamehide++;
                }
                if (graphics.translucentroomname)
                {
                    graphics.footerrect.y = 230+ed.roomnamehide;
                    SDL_BlitSurface(graphics.footerbuffer, NULL, graphics.backBuffer, &graphics.footerrect);
                }
                else
                {
                    FillRect(graphics.backBuffer, 0,230+ed.roomnamehide,320,10, graphics.getRGB(0,0,0));
                }
                graphics.bprint(5,231+ed.roomnamehide,ed.level[ed.levx+(ed.maxwidth*ed.levy)].roomname, 196, 196, 255 - help.glow, true);
                graphics.bprint(4, 222, "SPACE ^  SHIFT ^", 196, 196, 255 - help.glow, false);
                graphics.bprint(268,222, "("+help.String(ed.levx+1)+","+help.String(ed.levy+1)+")",196, 196, 255 - help.glow, false);
            }
            else
            {
                graphics.bprint(4, 232, "SPACE ^  SHIFT ^", 196, 196, 255 - help.glow, false);
                graphics.bprint(268,232, "("+help.String(ed.levx+1)+","+help.String(ed.levy+1)+")",196, 196, 255 - help.glow, false);
            }
        }

        if(ed.shiftmenu)
        {
            fillboxabs(0, 127,161+8,140,graphics.getRGB(64,64,64));
            FillRect(graphics.backBuffer, 0,128,160+8,140, graphics.getRGB(0,0,0));
            graphics.Print(4, 130, "F1: Change Tileset",164,164,164,false);
            graphics.Print(4, 140, "F2: Change Colour",164,164,164,false);
            graphics.Print(4, 150, "F3: Change Enemies",164,164,164,false);
            graphics.Print(4, 160, "F4: Enemy Bounds",164,164,164,false);
            graphics.Print(4, 170, "F5: Platform Bounds",164,164,164,false);

            graphics.Print(4, 190, "F10: Direct Mode",164,164,164,false);

            graphics.Print(4, 210, "W: Change Warp Dir",164,164,164,false);
            graphics.Print(4, 220, "E: Change Roomname",164,164,164,false);

            fillboxabs(220, 207,100,60,graphics.getRGB(64,64,64));
            FillRect(graphics.backBuffer, 221,208,160,60, graphics.getRGB(0,0,0));
            graphics.Print(224, 210, "S: Save Map",164,164,164,false);
            graphics.Print(224, 220, "L: Load Map",164,164,164,false);
        }
    }


    if(!ed.settingsmod && !ed.scripteditmod)
    {
        //Same as above, without borders
        switch(ed.drawmode)
        {
        case 0:
            graphics.bprint(2,2, "1: Walls",196, 196, 255 - help.glow);
            break;
        case 1:
            graphics.bprint(2,2, "2: Backing",196, 196, 255 - help.glow);
            break;
        case 2:
            graphics.bprint(2,2, "3: Spikes",196, 196, 255 - help.glow);
            break;
        case 3:
            graphics.bprint(2,2, "4: Trinkets",196, 196, 255 - help.glow);
            break;
        case 4:
            graphics.bprint(2,2, "5: Checkpoint",196, 196, 255 - help.glow);
            break;
        case 5:
            graphics.bprint(2,2, "6: Disappear",196, 196, 255 - help.glow);
            break;
        case 6:
            graphics.bprint(2,2, "7: Conveyors",196, 196, 255 - help.glow);
            break;
        case 7:
            graphics.bprint(2,2, "8: Moving",196, 196, 255 - help.glow);
            break;
        case 8:
            graphics.bprint(2,2, "9: Enemies",196, 196, 255 - help.glow);
            break;
        case 9:
            graphics.bprint(2,2, "0: Grav Line",196, 196, 255 - help.glow);
            break;
        case 10:
            graphics.bprint(2,2, "R: Roomtext",196, 196, 255 - help.glow);
            break;
        case 11:
            graphics.bprint(2,2, "T: Terminal",196, 196, 255 - help.glow);
            break;
        case 12:
            graphics.bprint(2,2, "Y: Script Box",196, 196, 255 - help.glow);
            break;
        case 13:
            graphics.bprint(2,2, "U: Warp Token",196, 196, 255 - help.glow);
            break;
        case 14:
            graphics.bprint(2,2, "I: Warp Lines",196, 196, 255 - help.glow);
            break;
        case 15:
            graphics.bprint(2,2, "O: Crewmate",196, 196, 255 - help.glow);
            break;
        case 16:
            graphics.bprint(2,2, "P: Start Point",196, 196, 255 - help.glow);
            break;
        }
    }

    if(ed.notedelay>0)
    {
        FillRect(graphics.backBuffer, 0,115,320,18, graphics.getRGB(92,92,92));
        FillRect(graphics.backBuffer, 0,116,320,16, graphics.getRGB(0,0,0));
        graphics.Print(0,121, ed.note,196-((45-ed.notedelay)*4), 196-((45-ed.notedelay)*4), 196-((45-ed.notedelay)*4), true);
    }

    graphics.drawfade();

    if (game.flashlight > 0 && !game.noflashingmode)
    {
        game.flashlight--;
        graphics.flashlight();
    }

    if (game.screenshake > 0  && !game.noflashingmode)
    {
        game.screenshake--;
        graphics.screenshake();
    }
    else
    {
        graphics.render();
    }
}

void editorlogic()
{
    //Misc
    help.updateglow();

    map.bypos -= 2;
    map.bscroll = -2;

    ed.entframedelay--;
    if(ed.entframedelay<=0)
    {
        ed.entframe=(ed.entframe+1)%4;
        ed.entframedelay=8;
    }

    if(ed.notedelay>0)
    {
        ed.notedelay--;
    }

    if (graphics.fademode == 1)
    {
        //Return to game
        map.nexttowercolour();
        map.colstate = 10;
        game.gamestate = TITLEMODE;
        graphics.fademode = 4;
        music.haltdasmusik();
        music.play(6);
        map.nexttowercolour();
        ed.settingsmod=false;
        graphics.backgrounddrawn=false;
        game.returntomenu(Menu::playerworlds);
    }
}


void editormenuactionpress()
{
    switch (game.currentmenuname)
    {
    case Menu::ed_desc:
        switch (game.currentmenuoption)
        {
        case 0:
            ed.textentry=true;
            ed.titlemod=true;
            key.enabletextentry();
            key.keybuffer=EditorData::GetInstance().title;
            break;
        case 1:
            ed.textentry=true;
            ed.creatormod=true;
            key.enabletextentry();
            key.keybuffer=EditorData::GetInstance().creator;
            break;
        case 2:
            ed.textentry=true;
            ed.desc1mod=true;
            key.enabletextentry();
            key.keybuffer=ed.Desc1;
            break;
        case 3:
            ed.textentry=true;
            ed.websitemod=true;
            key.enabletextentry();
            key.keybuffer=ed.website;
            break;
        case 4:
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::ed_settings:
        switch (game.currentmenuoption)
        {
        case 0:
            //Change level description stuff
            music.playef(11);
            game.createmenu(Menu::ed_desc);
            map.nexttowercolour();
            break;
        case 1:
            //Enter script editormode
            music.playef(11);
            ed.scripteditmod=true;
            ed.clearscriptbuffer();
            key.enabletextentry();
            key.keybuffer="";
            ed.hookmenupage=0;
            ed.hookmenu=0;
            ed.scripthelppage=0;
            ed.scripthelppagedelay=0;
            ed.sby=0;
            ed.sbx=0, ed.pagey=0;
            break;
        case 2:
            music.playef(11);
            game.createmenu(Menu::ed_music);
            map.nexttowercolour();
            if(ed.levmusic>0) music.play(ed.levmusic);
            break;
        case 3:
            //Load level
            ed.settingsmod=false;
            graphics.backgrounddrawn=false;
            map.nexttowercolour();

            ed.loadmod=true;
            ed.textentry=true;
            key.enabletextentry();
            key.keybuffer=ed.filename;
            ed.keydelay=6;
            game.mapheld=true;
            graphics.backgrounddrawn=false;
            break;
        case 4:
            //Save level
            ed.settingsmod=false;
            graphics.backgrounddrawn=false;
            map.nexttowercolour();

            ed.savemod=true;
            ed.textentry=true;
            key.enabletextentry();
            key.keybuffer=ed.filename;
            ed.keydelay=6;
            game.mapheld=true;
            graphics.backgrounddrawn=false;
            break;
        case 5:
            music.playef(11);
            game.createmenu(Menu::ed_quit);
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::ed_music:
        switch (game.currentmenuoption)
        {
        case 0:
            ed.levmusic++;
            if(ed.levmusic==5) ed.levmusic=6;
            if(ed.levmusic==7) ed.levmusic=8;
            if(ed.levmusic==9) ed.levmusic=10;
            if(ed.levmusic==15) ed.levmusic=0;
            if(ed.levmusic>0)
            {
                music.play(ed.levmusic);
            }
            else
            {
                music.haltdasmusik();
            }
            music.playef(11);
            break;
        case 1:
            music.playef(11);
            music.fadeout();
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::ed_quit:
        switch (game.currentmenuoption)
        {
        case 0:
            //Saving and quit
            ed.saveandquit=true;

            ed.settingsmod=false;
            graphics.backgrounddrawn=false;
            map.nexttowercolour();

            ed.savemod=true;
            ed.textentry=true;
            key.enabletextentry();
            key.keybuffer=ed.filename;
            ed.keydelay=6;
            game.mapheld=true;
            graphics.backgrounddrawn=false;
            break;
        case 1:
            //Quit without saving
            music.playef(11);
            music.fadeout();
            graphics.fademode = 2;
            break;
        case 2:
            //Go back to editor
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    default:
        break;
    }
}

void editorinput()
{
    game.mx = (float) key.mx;
    game.my = (float) key.my;
    ed.tilex=(game.mx - (game.mx%8))/8;
    ed.tiley=(game.my - (game.my%8))/8;
    if (game.stretchMode == 1) {
        // In this mode specifically, we have to fix the mouse coordinates
        int winwidth, winheight;
        graphics.screenbuffer->GetWindowSize(&winwidth, &winheight);
        ed.tilex = ed.tilex * 320 / winwidth;
        ed.tiley = ed.tiley * 240 / winheight;
    }

    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;

    if (key.isDown(KEYBOARD_LEFT) || key.isDown(KEYBOARD_a))
    {
        game.press_left = true;
    }
    if (key.isDown(KEYBOARD_RIGHT) || key.isDown(KEYBOARD_d))
    {
        game.press_right = true;
    }
    if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v))
    {
        game.press_action = true;
    };

    if (key.isDown(KEYBOARD_ENTER)) game.press_map = true;
    if (key.isDown(27) && !ed.settingskey)
    {
        ed.settingskey=true;
        if(ed.textentry)
        {
            key.disabletextentry();
            ed.roomnamemod=false;
            ed.loadmod=false;
            ed.savemod=false;
            ed.textentry=false;
            ed.titlemod=false;
            ed.desc1mod=false;
            ed.desc2mod=false;
            ed.desc3mod=false;
            ed.websitemod=false;
            ed.creatormod=false;
            if(ed.scripttextmod || ed.roomtextmod)
            {
                if (ed.oldenttext == "")
                {
                    removeedentity(ed.textent);
                }
                else
                {
                    edentity[ed.textent].scriptname = ed.oldenttext;
                }
            }

            ed.scripttextmod=false;
            ed.roomtextmod=false;
            ed.shiftmenu=false;
            ed.shiftkey=false;
        }
        else if(ed.boundarymod>0)
        {
            ed.boundarymod=0;
        }
        else
        {

            ed.settingsmod=!ed.settingsmod;
            graphics.backgrounddrawn=false;

            game.createmenu(Menu::ed_settings);
            map.nexttowercolour();
        }
    }

    if (!key.isDown(27))
    {
        ed.settingskey=false;
    }

    if(key.keymap[SDLK_LCTRL] || key.keymap[SDLK_RCTRL])
    {
        if(key.leftbutton) key.rightbutton=true;
    }

    if(ed.scripteditmod)
    {
        if(ed.scripthelppage==0)
        {
            //hook select menu
            if(ed.keydelay>0) ed.keydelay--;

            if(key.keymap[SDLK_UP] && ed.keydelay<=0)
            {
                ed.keydelay=6;
                ed.hookmenu--;
            }

            if(key.keymap[SDLK_DOWN] && ed.keydelay<=0)
            {
                ed.keydelay=6;
                ed.hookmenu++;
            }

            if(ed.hookmenu>=(int)ed.hooklist.size())
            {
                ed.hookmenu=ed.hooklist.size()-1;
            }
            if(ed.hookmenu<0) ed.hookmenu=0;

            if(ed.hookmenu<ed.hookmenupage)
            {
                ed.hookmenupage=ed.hookmenu;
            }

            if(ed.hookmenu>=ed.hookmenupage+9)
            {
                ed.hookmenupage=ed.hookmenu+8;
            }

            if(!key.keymap[SDLK_BACKSPACE]) ed.deletekeyheld=0;

            if(key.keymap[SDLK_BACKSPACE] && ed.deletekeyheld==0 && !ed.hooklist.empty())
            {
                ed.deletekeyheld=1;
                music.playef(2);
                ed.removehook(ed.hooklist[(ed.hooklist.size()-1)-ed.hookmenu]);
            }

            if (!game.press_action && !game.press_left && !game.press_right
                    && !key.keymap[SDLK_UP] && !key.keymap[SDLK_DOWN] && !key.isDown(27)) game.jumpheld = false;
            if (!game.jumpheld)
            {
                if (game.press_action || game.press_left || game.press_right || game.press_map
                        || key.keymap[SDLK_UP] || key.keymap[SDLK_DOWN] || key.isDown(27))
                {
                    game.jumpheld = true;
                }
                if ((game.press_action || game.press_map) && !ed.hooklist.empty())
                {
                    game.mapheld=true;
                    ed.scripthelppage=1;
                    key.keybuffer="";
                    ed.sbscript=ed.hooklist[(ed.hooklist.size()-1)-ed.hookmenu];
                    ed.loadhookineditor(ed.sbscript);

                    ed.sby=ed.sb.size()-1;
                    ed.pagey=0;
                    while(ed.sby>=20)
                    {
                        ed.pagey++;
                        ed.sby--;
                    }
                    key.keybuffer=ed.sb[ed.pagey+ed.sby];
                    ed.sbx = utf8::unchecked::distance(ed.sb[ed.pagey+ed.sby].begin(), ed.sb[ed.pagey+ed.sby].end());
                }

                if (key.isDown(27))
                {
                    ed.scripteditmod=false;
                    ed.settingsmod=false;
                }
            }
        }
        else if(ed.scripthelppage==1)
        {
            //Script editor!
            if (key.isDown(27))
            {
                ed.scripthelppage=0;
                game.jumpheld = true;
                //save the script for use again!
                ed.addhook(ed.sbscript);
            }

            if(ed.keydelay>0) ed.keydelay--;

            if(key.keymap[SDLK_UP] && ed.keydelay<=0)
            {
                ed.keydelay=6;
                ed.sby--;
                if(ed.sby<=5)
                {
                    if(ed.pagey>0)
                    {
                        ed.pagey--;
                        ed.sby++;
                    }
                    else
                    {
                        if(ed.sby<0) ed.sby=0;
                    }
                }
                key.keybuffer=ed.sb[ed.pagey+ed.sby];
            }

            if(key.keymap[SDLK_DOWN] && ed.keydelay<=0)
            {
                ed.keydelay=6;
                if(ed.sby+ed.pagey<(int)ed.sb.size()-1)
                {
                    ed.sby++;
                    if(ed.sby>=20)
                    {
                        ed.pagey++;
                        ed.sby--;
                    }
                }
                key.keybuffer=ed.sb[ed.pagey+ed.sby];
            }

            if(key.linealreadyemptykludge)
            {
                ed.keydelay=6;
                key.linealreadyemptykludge=false;
            }

            if(key.pressedbackspace && ed.sb[ed.pagey+ed.sby]=="" && ed.keydelay<=0)
            {
                //Remove this line completely
                ed.removeline(ed.pagey+ed.sby);
                ed.sby--;
                if(ed.sby<=5)
                {
                    if(ed.pagey>0)
                    {
                        ed.pagey--;
                        ed.sby++;
                    }
                    else
                    {
                        if(ed.sby<0) ed.sby=0;
                    }
                }
                key.keybuffer=ed.sb[ed.pagey+ed.sby];
                ed.keydelay=6;
            }

            ed.sb[ed.pagey+ed.sby]=key.keybuffer;
            ed.sbx = utf8::unchecked::distance(ed.sb[ed.pagey+ed.sby].begin(), ed.sb[ed.pagey+ed.sby].end());

            if(!game.press_map && !key.isDown(27)) game.mapheld=false;
            if (!game.mapheld)
            {
                if(game.press_map)
                {
                    game.mapheld=true;
                    //Continue to next line
                    if(ed.sby+ed.pagey>=(int)ed.sb.size()) //we're on the last line
                    {
                        ed.sby++;
                        if(ed.sby>=20)
                        {
                            ed.pagey++;
                            ed.sby--;
                        }
                        key.keybuffer=ed.sb[ed.pagey+ed.sby];
                        ed.sbx = utf8::unchecked::distance(ed.sb[ed.pagey+ed.sby].begin(), ed.sb[ed.pagey+ed.sby].end());
                    }
                    else
                    {
                        //We're not, insert a line instead
                        ed.sby++;
                        if(ed.sby>=20)
                        {
                            ed.pagey++;
                            ed.sby--;
                        }
                        ed.insertline(ed.sby+ed.pagey);
                        key.keybuffer="";
                        ed.sbx = 0;
                    }
                }
            }
        }
    }
    else if(ed.textentry)
    {
        if(ed.roomnamemod)
        {
            ed.level[ed.levx+(ed.levy*ed.maxwidth)].roomname=key.keybuffer;
        }
        else if(ed.savemod)
        {
            ed.filename=key.keybuffer;
        }
        else if(ed.loadmod)
        {
            ed.filename=key.keybuffer;
        }
        else if(ed.roomtextmod)
        {
            edentity[ed.textent].scriptname=key.keybuffer;
        }
        else if(ed.scripttextmod)
        {
            edentity[ed.textent].scriptname=key.keybuffer;
        }
        else if(ed.titlemod)
        {
            EditorData::GetInstance().title=key.keybuffer;
        }
        else if(ed.creatormod)
        {
            EditorData::GetInstance().creator=key.keybuffer;
        }
        else if(ed.websitemod)
        {
            ed.website=key.keybuffer;
        }
        else if(ed.desc1mod)
        {
            ed.Desc1=key.keybuffer;
        }
        else if(ed.desc2mod)
        {
            ed.Desc2=key.keybuffer;
        }
        else if(ed.desc3mod)
        {
            ed.Desc3=key.keybuffer;
        }

        if(!game.press_map && !key.isDown(27)) game.mapheld=false;
        if (!game.mapheld)
        {
            if(game.press_map)
            {
                game.mapheld=true;
                if(ed.roomnamemod)
                {
                    ed.level[ed.levx+(ed.levy*ed.maxwidth)].roomname=key.keybuffer;
                    ed.roomnamemod=false;
                }
                else if(ed.savemod)
                {
                    std::string savestring=ed.filename+".vvvvvv";
                    ed.save(savestring);
                    ed.note="[ Saved map: " + ed.filename+ ".vvvvvv]";
                    ed.notedelay=45;
                    ed.savemod=false;

                    ed.shiftmenu=false;
                    ed.shiftkey=false;

                    if(ed.saveandquit)
                    {
                        //quit editor
                        graphics.fademode = 2;
                    }
                }
                else if(ed.loadmod)
                {
                    std::string loadstring=ed.filename+".vvvvvv";
                    ed.load(loadstring);
                    ed.note="[ Loaded map: " + ed.filename+ ".vvvvvv]";
                    ed.notedelay=45;
                    ed.loadmod=false;

                    ed.shiftmenu=false;
                    ed.shiftkey=false;
                }
                else if(ed.roomtextmod)
                {
                    edentity[ed.textent].scriptname=key.keybuffer;
                    ed.roomtextmod=false;

                    ed.shiftmenu=false;
                    ed.shiftkey=false;
                }
                else if(ed.scripttextmod)
                {
                    edentity[ed.textent].scriptname=key.keybuffer;
                    ed.scripttextmod=false;
                    ed.clearscriptbuffer();
                    if(!ed.checkhook(edentity[ed.textent].scriptname))
                    {
                        ed.addhook(edentity[ed.textent].scriptname);
                    }
                }
                else if(ed.titlemod)
                {
                    EditorData::GetInstance().title=key.keybuffer;
                    ed.titlemod=false;
                }
                else if(ed.creatormod)
                {
                    EditorData::GetInstance().creator=key.keybuffer;
                    ed.creatormod=false;
                }
                else if(ed.websitemod)
                {
                    ed.website=key.keybuffer;
                    ed.websitemod=false;
                }
                else if(ed.desc1mod)
                {
                    ed.Desc1=key.keybuffer;
                }
                else if(ed.desc2mod)
                {
                    ed.Desc2=key.keybuffer;
                }
                else if(ed.desc3mod)
                {
                    ed.Desc3=key.keybuffer;
                    ed.desc3mod=false;
                }
                key.disabletextentry();
                ed.textentry=false;

                if(ed.desc1mod)
                {
                    ed.desc1mod=false;

                    ed.textentry=true;
                    ed.desc2mod=true;
                    key.enabletextentry();
                    key.keybuffer=ed.Desc2;
                }
                else if(ed.desc2mod)
                {
                    ed.desc2mod=false;

                    ed.textentry=true;
                    ed.desc3mod=true;
                    key.enabletextentry();
                    key.keybuffer=ed.Desc3;
                }
            }
        }
    }
    else
    {
        if(ed.settingsmod)
        {
            if (!game.press_action && !game.press_left && !game.press_right
                    && !key.keymap[SDLK_UP] && !key.keymap[SDLK_DOWN]) game.jumpheld = false;
            if (!game.jumpheld)
            {
                if (game.press_action || game.press_left || game.press_right || game.press_map
                        || key.keymap[SDLK_UP] || key.keymap[SDLK_DOWN])
                {
                    game.jumpheld = true;
                }

                if(game.menustart)
                {
                    if (game.press_left || key.keymap[SDLK_UP])
                    {
                        game.currentmenuoption--;
                    }
                    else if (game.press_right || key.keymap[SDLK_DOWN])
                    {
                        game.currentmenuoption++;
                    }
                }

                if (game.currentmenuoption < 0) game.currentmenuoption = game.menuoptions.size()-1;
                if (game.currentmenuoption >= (int) game.menuoptions.size() ) game.currentmenuoption = 0;

                if (game.press_action)
                {
                    editormenuactionpress();
                }
            }
        }
        else
        {
            //Shortcut keys
            //TO DO: make more user friendly
            if(key.keymap[SDLK_F1] && ed.keydelay==0)
            {
                ed.level[ed.levx+(ed.levy*ed.maxwidth)].tileset++;
                graphics.backgrounddrawn=false;
                if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].tileset>=5) ed.level[ed.levx+(ed.levy*ed.maxwidth)].tileset=0;
                if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].tileset==0)
                {
                    if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].tilecol>=32) ed.level[ed.levx+(ed.levy*ed.maxwidth)].tilecol=0;
                }
                else if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].tileset==1)
                {
                    if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].tilecol>=8) ed.level[ed.levx+(ed.levy*ed.maxwidth)].tilecol=0;
                }
                else
                {
                    if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].tilecol>=6) ed.level[ed.levx+(ed.levy*ed.maxwidth)].tilecol=0;
                }
                ed.notedelay=45;
                switch(ed.level[ed.levx+(ed.levy*ed.maxwidth)].tileset)
                {
                case 0:
                    ed.note="Now using Space Station Tileset";
                    break;
                case 1:
                    ed.note="Now using Outside Tileset";
                    break;
                case 2:
                    ed.note="Now using Lab Tileset";
                    break;
                case 3:
                    ed.note="Now using Warp Zone Tileset";
                    break;
                case 4:
                    ed.note="Now using Ship Tileset";
                    break;
                case 5:
                    ed.note="Now using Tower Tileset";
                    break;
                default:
                    ed.note="Tileset Changed";
                    break;
                }
                ed.updatetiles=true;
                ed.keydelay=6;
            }
            if(key.keymap[SDLK_F2] && ed.keydelay==0)
            {
                ed.level[ed.levx+(ed.levy*ed.maxwidth)].tilecol++;
                graphics.backgrounddrawn=false;
                if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].tileset==0)
                {
                    if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].tilecol>=32) ed.level[ed.levx+(ed.levy*ed.maxwidth)].tilecol=0;
                }
                else if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].tileset==1)
                {
                    if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].tilecol>=8) ed.level[ed.levx+(ed.levy*ed.maxwidth)].tilecol=0;
                }
                else
                {
                    if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].tilecol>=6) ed.level[ed.levx+(ed.levy*ed.maxwidth)].tilecol=0;
                }
                ed.updatetiles=true;
                ed.keydelay=6;
                ed.notedelay=45;
                ed.note="Tileset Colour Changed";
            }
            if(key.keymap[SDLK_F3] && ed.keydelay==0)
            {
                ed.level[ed.levx+(ed.levy*ed.maxwidth)].enemytype=(ed.level[ed.levx+(ed.levy*ed.maxwidth)].enemytype+1)%10;
                ed.keydelay=6;
                ed.notedelay=45;
                ed.note="Enemy Type Changed";
            }
            if(key.keymap[SDLK_F4] && ed.keydelay==0)
            {
                ed.keydelay=6;
                ed.boundarytype=1;
                ed.boundarymod=1;
            }
            if(key.keymap[SDLK_F5] && ed.keydelay==0)
            {
                ed.keydelay=6;
                ed.boundarytype=2;
                ed.boundarymod=1;
            }
            if(key.keymap[SDLK_F10] && ed.keydelay==0)
            {
                if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].directmode==1)
                {
                    ed.level[ed.levx+(ed.levy*ed.maxwidth)].directmode=0;
                    ed.note="Direct Mode Disabled";
                }
                else
                {
                    ed.level[ed.levx+(ed.levy*ed.maxwidth)].directmode=1;
                    ed.note="Direct Mode Enabled";
                }
                graphics.backgrounddrawn=false;

                ed.notedelay=45;
                ed.updatetiles=true;
                ed.keydelay=6;
            }
            if(key.keymap[SDLK_1]) ed.drawmode=0;
            if(key.keymap[SDLK_2]) ed.drawmode=1;
            if(key.keymap[SDLK_3]) ed.drawmode=2;
            if(key.keymap[SDLK_4]) ed.drawmode=3;
            if(key.keymap[SDLK_5]) ed.drawmode=4;
            if(key.keymap[SDLK_6]) ed.drawmode=5;
            if(key.keymap[SDLK_7]) ed.drawmode=6;
            if(key.keymap[SDLK_8]) ed.drawmode=7;
            if(key.keymap[SDLK_9]) ed.drawmode=8;
            if(key.keymap[SDLK_0]) ed.drawmode=9;
            if(key.keymap[SDLK_r]) ed.drawmode=10;
            if(key.keymap[SDLK_t]) ed.drawmode=11;
            if(key.keymap[SDLK_y]) ed.drawmode=12;
            if(key.keymap[SDLK_u]) ed.drawmode=13;
            if(key.keymap[SDLK_i]) ed.drawmode=14;
            if(key.keymap[SDLK_o]) ed.drawmode=15;
            if(key.keymap[SDLK_p]) ed.drawmode=16;

            if(key.keymap[SDLK_w] && ed.keydelay==0)
            {
                int j=0, tx=0, ty=0;
                for(size_t i=0; i<edentity.size(); i++)
                {
                    if(edentity[i].t==50)
                    {
                        tx=(edentity[i].p1-(edentity[i].p1%40))/40;
                        ty=(edentity[i].p2-(edentity[i].p2%30))/30;
                        if(tx==ed.levx && ty==ed.levy)
                        {
                            j++;
                        }
                    }
                }
                if(j>0)
                {
                    ed.note="ERROR: Cannot have both warp types";
                    ed.notedelay=45;
                }
                else
                {
                    ed.level[ed.levx+(ed.levy*ed.maxwidth)].warpdir=(ed.level[ed.levx+(ed.levy*ed.maxwidth)].warpdir+1)%4;
                    if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].warpdir==0)
                    {
                        ed.note="Room warping disabled";
                        ed.notedelay=45;
                        graphics.backgrounddrawn=false;
                    }
                    else if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].warpdir==1)
                    {
                        ed.note="Room warps horizontally";
                        ed.notedelay=45;
                        graphics.backgrounddrawn=false;
                    }
                    else if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].warpdir==2)
                    {
                        ed.note="Room warps vertically";
                        ed.notedelay=45;
                        graphics.backgrounddrawn=false;
                    }
                    else if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].warpdir==3)
                    {
                        ed.note="Room warps in all directions";
                        ed.notedelay=45;
                        graphics.backgrounddrawn=false;
                    }
                }
                ed.keydelay=6;
            }
            if(key.keymap[SDLK_e] && ed.keydelay==0)
            {
                ed.roomnamemod=true;
                ed.textentry=true;
                key.enabletextentry();
                key.keybuffer=ed.level[ed.levx+(ed.levy*ed.maxwidth)].roomname;
                ed.keydelay=6;
                game.mapheld=true;
            }

            //Save and load
            if(key.keymap[SDLK_s] && ed.keydelay==0)
            {
                ed.savemod=true;
                ed.textentry=true;
                key.enabletextentry();
                key.keybuffer=ed.filename;
                ed.keydelay=6;
                game.mapheld=true;
                graphics.backgrounddrawn=false;
            }

            if(key.keymap[SDLK_l] && ed.keydelay==0)
            {
                ed.loadmod=true;
                ed.textentry=true;
                key.enabletextentry();
                key.keybuffer=ed.filename;
                ed.keydelay=6;
                game.mapheld=true;
                graphics.backgrounddrawn=false;
            }

            if(!game.press_map) game.mapheld=false;
            if (!game.mapheld)
            {
                if(game.press_map)
                {
                    game.mapheld=true;

                    //Ok! Scan the room for the closest checkpoint
                    int testeditor=-1;
                    int startpoint=0;
                    //First up; is there a start point on this screen?
                    for(size_t i=0; i<edentity.size(); i++)
                    {
                        //if() on screen
                        if(edentity[i].t==16 && testeditor==-1)
                        {
                            int tx=(edentity[i].x-(edentity[i].x%40))/40;
                            int ty=(edentity[i].y-(edentity[i].y%30))/30;
                            if(tx==ed.levx && ty==ed.levy)
                            {
                                testeditor=i;
                                startpoint=1;
                            }
                        }
                    }
                    if(testeditor==-1)
                    {
                        //Ok, settle for a check point
                        for(size_t i=0; i<edentity.size(); i++)
                        {
                            //if() on screen
                            if(edentity[i].t==10 && testeditor==-1)
                            {
                                int tx=(edentity[i].x-(edentity[i].x%40))/40;
                                int ty=(edentity[i].y-(edentity[i].y%30))/30;
                                if(tx==ed.levx && ty==ed.levy)
                                {
                                    testeditor=i;
                                }
                            }
                        }
                    }

                    if(testeditor==-1)
                    {
                        ed.note="ERROR: No checkpoint to spawn at";
                        ed.notedelay=45;
                    }
                    else
                    {
                        if(startpoint==0)
                        {
                            //Checkpoint spawn
                            int tx=(edentity[testeditor].x-(edentity[testeditor].x%40))/40;
                            int ty=(edentity[testeditor].y-(edentity[testeditor].y%30))/30;
                            game.edsavex = (edentity[testeditor].x%40)*8;
                            game.edsavey = (edentity[testeditor].y%30)*8;
                            game.edsaverx = 100+tx;
                            game.edsavery = 100+ty;
                            game.edsavegc = edentity[testeditor].p1;
                            if(game.edsavegc==0)
                            {
                                game.edsavey--;
                            }
                            else
                            {
                                game.edsavey-=8;
                            }
                            game.edsavedir = 0;
                        }
                        else
                        {
                            //Start point spawn
                            int tx=(edentity[testeditor].x-(edentity[testeditor].x%40))/40;
                            int ty=(edentity[testeditor].y-(edentity[testeditor].y%30))/30;
                            game.edsavex = ((edentity[testeditor].x%40)*8)-4;
                            game.edsavey = (edentity[testeditor].y%30)*8;
                            game.edsaverx = 100+tx;
                            game.edsavery = 100+ty;
                            game.edsavegc = 0;
                            game.edsavey--;
                            game.edsavedir=1-edentity[testeditor].p1;
                        }

                        music.haltdasmusik();
                        graphics.backgrounddrawn=false;
                        ed.returneditoralpha = 1000; // Let's start it higher than 255 since it gets clamped
                        script.startgamemode(21);
                    }
                }
            }

            if(key.keymap[SDLK_x])
            {
                ed.xmod=true;
            }
            else
            {
                ed.xmod=false;
            }


            if(key.keymap[SDLK_z])
            {
                ed.zmod=true;
            }
            else
            {
                ed.zmod=false;
            }

            //Keyboard shortcuts
            if(ed.keydelay>0)
            {
                ed.keydelay--;
            }
            else
            {
                if(key.keymap[SDLK_LSHIFT] || key.keymap[SDLK_RSHIFT])
                {
                    if(key.keymap[SDLK_UP])
                    {
                        ed.keydelay=6;
                        ed.mapheight--;
                    }
                    else if(key.keymap[SDLK_DOWN])
                    {
                        ed.keydelay=6;
                        ed.mapheight++;
                    }

                    if(key.keymap[SDLK_LEFT])
                    {
                        ed.keydelay=6;
                        ed.mapwidth--;
                    }
                    else if(key.keymap[SDLK_RIGHT])
                    {
                        ed.keydelay=6;
                        ed.mapwidth++;
                    }
                    if(ed.keydelay==6)
                    {
                        if(ed.mapwidth<1) ed.mapwidth=1;
                        if(ed.mapheight<1) ed.mapheight=1;
                        if(ed.mapwidth>=ed.maxwidth) ed.mapwidth=ed.maxwidth;
                        if(ed.mapheight>=ed.maxheight) ed.mapheight=ed.maxheight;
                        ed.note = "Mapsize is now [" + help.String(ed.mapwidth) + "," + help.String(ed.mapheight) + "]";
                        ed.notedelay=45;
                    }
                }
                else
                {
                    if(key.keymap[SDLK_COMMA])
                    {
                        ed.drawmode--;
                        ed.keydelay=6;
                    }
                    else if(key.keymap[SDLK_PERIOD])
                    {
                        ed.drawmode++;
                        ed.keydelay=6;
                    }

                    if(ed.drawmode<0)
                    {
                        ed.drawmode=16;
                        if(ed.spacemod) ed.spacemenu=0;
                    }
                    if(ed.drawmode>16) ed.drawmode=0;
                    if(ed.drawmode>9)
                    {
                        if(ed.spacemod) ed.spacemenu=1;
                    }
                    else
                    {
                        if(ed.spacemod) ed.spacemenu=0;
                    }

                    if(key.keymap[SDLK_LCTRL] || key.keymap[SDLK_RCTRL])
                    {
                        ed.dmtileeditor=10;
                        if(key.keymap[SDLK_LEFT])
                        {
                            ed.dmtile--;
                            ed.keydelay=3;
                            if(ed.dmtile<0) ed.dmtile+=1200;
                        }
                        else if(key.keymap[SDLK_RIGHT])
                        {
                            ed.dmtile++;
                            ed.keydelay=3;

                            if(ed.dmtile>=1200) ed.dmtile-=1200;
                        }
                        if(key.keymap[SDLK_UP])
                        {
                            ed.dmtile-=40;
                            ed.keydelay=3;
                            if(ed.dmtile<0) ed.dmtile+=1200;
                        }
                        else if(key.keymap[SDLK_DOWN])
                        {
                            ed.dmtile+=40;
                            ed.keydelay=3;

                            if(ed.dmtile>=1200) ed.dmtile-=1200;
                        }
                    }
                    else
                    {
                        if(key.keymap[SDLK_UP])
                        {
                            ed.keydelay=6;
                            graphics.backgrounddrawn=false;
                            ed.levy--;
                            ed.updatetiles=true;
                            ed.changeroom=true;
                        }
                        else if(key.keymap[SDLK_DOWN])
                        {
                            ed.keydelay=6;
                            graphics.backgrounddrawn=false;
                            ed.levy++;
                            ed.updatetiles=true;
                            ed.changeroom=true;
                        }
                        else if(key.keymap[SDLK_LEFT])
                        {
                            ed.keydelay=6;
                            graphics.backgrounddrawn=false;
                            ed.levx--;
                            ed.updatetiles=true;
                            ed.changeroom=true;
                        }
                        else if(key.keymap[SDLK_RIGHT])
                        {
                            ed.keydelay=6;
                            graphics.backgrounddrawn=false;
                            ed.levx++;
                            ed.updatetiles=true;
                            ed.changeroom=true;
                        }
                    }

                    if(ed.levx<0) ed.levx+=ed.mapwidth;
                    if(ed.levx>= ed.mapwidth) ed.levx-=ed.mapwidth;
                    if(ed.levy<0) ed.levy+=ed.mapheight;
                    if(ed.levy>=ed.mapheight) ed.levy-=ed.mapheight;
                }
                if(key.keymap[SDLK_SPACE])
                {
                    ed.spacemod = !ed.spacemod;
                    ed.keydelay=6;
                }

                if(key.keymap[SDLK_LSHIFT] || key.keymap[SDLK_RSHIFT])
                {
                    if(!ed.shiftkey)
                    {
                        if(ed.shiftmenu)
                        {
                            ed.shiftmenu=false;
                        }
                        else
                        {
                            ed.shiftmenu=true;
                        }
                    }
                    ed.shiftkey=true;
                }
                else
                {
                    ed.shiftkey=false;
                }
            }
        }

        if(!ed.settingsmod)
        {
            if(ed.boundarymod>0)
            {
                if(key.leftbutton)
                {
                    if(ed.lclickdelay==0)
                    {
                        if(ed.boundarymod==1)
                        {
                            ed.lclickdelay=1;
                            ed.boundx1=(ed.tilex*8);
                            ed.boundy1=(ed.tiley*8);
                            ed.boundarymod=2;
                        }
                        else if(ed.boundarymod==2)
                        {
                            if((ed.tilex*8)+8>=ed.boundx1 || (ed.tiley*8)+8>=ed.boundy1)
                            {
                                ed.boundx2=(ed.tilex*8)+8;
                                ed.boundy2=(ed.tiley*8)+8;
                            }
                            else
                            {
                                ed.boundx2=ed.boundx1+8;
                                ed.boundy2=ed.boundy1+8;
                            }
                            if(ed.boundarytype==0)
                            {
                                //Script trigger
                                ed.scripttextmod=true;
                                ed.oldenttext="";
                                ed.textent=edentity.size();
                                addedentity((ed.boundx1/8)+(ed.levx*40),(ed.boundy1/8)+ (ed.levy*30),19,
                                            (ed.boundx2-ed.boundx1)/8, (ed.boundy2-ed.boundy1)/8);
                                ed.lclickdelay=1;

                                ed.textentry=true;
                                key.enabletextentry();
                                key.keybuffer="";
                                ed.lclickdelay=1;
                            }
                            else if(ed.boundarytype==1)
                            {
                                //Enemy bounds
                                int tmp=ed.levx+(ed.levy*ed.maxwidth);
                                ed.level[tmp].enemyx1=ed.boundx1;
                                ed.level[tmp].enemyy1=ed.boundy1;
                                ed.level[tmp].enemyx2=ed.boundx2;
                                ed.level[tmp].enemyy2=ed.boundy2;
                            }
                            else if(ed.boundarytype==2)
                            {
                                //Platform bounds
                                int tmp=ed.levx+(ed.levy*ed.maxwidth);
                                ed.level[tmp].platx1=ed.boundx1;
                                ed.level[tmp].platy1=ed.boundy1;
                                ed.level[tmp].platx2=ed.boundx2;
                                ed.level[tmp].platy2=ed.boundy2;
                            }
                            else if(ed.boundarytype==3)
                            {
                                //Copy
                            }
                            ed.boundarymod=0;
                            ed.lclickdelay=1;
                        }
                    }
                }
                else
                {
                    ed.lclickdelay=0;
                }
                if(key.rightbutton)
                {
                    ed.boundarymod=0;
                }
            }
            else if(ed.warpmod)
            {
                //Placing warp token
                if(key.leftbutton)
                {
                    if(ed.lclickdelay==0)
                    {
                        if(ed.free(ed.tilex, ed.tiley)==0)
                        {
                            edentity[ed.warpent].p1=ed.tilex+(ed.levx*40);
                            edentity[ed.warpent].p2=ed.tiley+(ed.levy*30);
                            ed.warpmod=false;
                            ed.warpent=-1;
                            ed.lclickdelay=1;
                        }
                    }
                }
                else
                {
                    ed.lclickdelay=0;
                }
                if(key.rightbutton)
                {
                    removeedentity(ed.warpent);
                    ed.warpmod=false;
                    ed.warpent=-1;
                }
            }
            else
            {
                //Mouse input
                if(key.leftbutton)
                {
                    if(ed.lclickdelay==0)
                    {
                        //Depending on current mode, place something
                        if(ed.drawmode==0)
                        {
                            //place tiles
                            //Are we in direct mode?
                            if(ed.level[ed.levx+(ed.levy*ed.maxwidth)].directmode>=1)
                            {
                                if(ed.xmod)
                                {
                                    for(int j=-2; j<3; j++)
                                    {
                                        for(int i=-2; i<3; i++)
                                        {
                                            ed.placetilelocal(ed.tilex+i, ed.tiley+j, ed.dmtile);
                                        }
                                    }
                                }
                                else if(ed.zmod)
                                {
                                    for(int j=-1; j<2; j++)
                                    {
                                        for(int i=-1; i<2; i++)
                                        {
                                            ed.placetilelocal(ed.tilex+i, ed.tiley+j, ed.dmtile);
                                        }
                                    }
                                }
                                else
                                {
                                    ed.placetilelocal(ed.tilex, ed.tiley, ed.dmtile);
                                }
                            }
                            else
                            {
                                if(ed.xmod)
                                {
                                    for(int j=-2; j<3; j++)
                                    {
                                        for(int i=-2; i<3; i++)
                                        {
                                            ed.placetilelocal(ed.tilex+i, ed.tiley+j, 80);
                                        }
                                    }
                                }
                                else if(ed.zmod)
                                {
                                    for(int j=-1; j<2; j++)
                                    {
                                        for(int i=-1; i<2; i++)
                                        {
                                            ed.placetilelocal(ed.tilex+i, ed.tiley+j, 80);
                                        }
                                    }
                                }
                                else
                                {
                                    ed.placetilelocal(ed.tilex, ed.tiley, 80);
                                }
                            }
                        }
                        else if(ed.drawmode==1)
                        {
                            //place background tiles
                            if(ed.xmod)
                            {
                                for(int j=-2; j<3; j++)
                                {
                                    for(int i=-2; i<3; i++)
                                    {
                                        ed.placetilelocal(ed.tilex+i, ed.tiley+j, 2);
                                    }
                                }
                            }
                            else if(ed.zmod)
                            {
                                for(int j=-1; j<2; j++)
                                {
                                    for(int i=-1; i<2; i++)
                                    {
                                        ed.placetilelocal(ed.tilex+i, ed.tiley+j, 2);
                                    }
                                }
                            }
                            else
                            {
                                ed.placetilelocal(ed.tilex, ed.tiley, 2);
                            }
                        }
                        else if(ed.drawmode==2)
                        {
                            //place spikes
                            ed.placetilelocal(ed.tilex, ed.tiley, 8);
                        }

                        int tmp=edentat(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30));
                        if(tmp==-1)
                        {
                            //Room text and script triggers can be placed in walls
                            if(ed.drawmode==10)
                            {
                                ed.roomtextmod=true;
                                ed.oldenttext="";
                                ed.textent=edentity.size();
                                ed.textentry=true;
                                key.enabletextentry();
                                key.keybuffer="";
                                graphics.backgrounddrawn=false;
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),17);
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==12)   //Script Trigger
                            {
                                ed.boundarytype=0;
                                ed.boundx1=ed.tilex*8;
                                ed.boundy1=ed.tiley*8;
                                ed.boundarymod=2;
                                ed.lclickdelay=1;
                            }
                        }
                        if(tmp==-1 && ed.free(ed.tilex,ed.tiley)==0)
                        {
                            if(ed.drawmode==3)
                            {
                                if(ed.numtrinkets()<100)
                                {
                                    addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),9);
                                    ed.lclickdelay=1;
                                }
                                else
                                {
                                    ed.note="ERROR: Max number of trinkets is 100";
                                    ed.notedelay=45;
                                }
                            }
                            else if(ed.drawmode==4)
                            {
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),10, 1);
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==5)
                            {
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),3);
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==6)
                            {
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),2,5);
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==7)
                            {
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),2,0);
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==8)
                            {
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),1,0);
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==9)
                            {
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),11,0);
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==11)
                            {
                                ed.scripttextmod=true;
                                ed.oldenttext="";
                                ed.textent=edentity.size();
                                ed.textentry=true;
                                key.enabletextentry();

                                addedentity(ed.tilex+(ed.levx*40),ed.tiley+ (ed.levy*30),18,0);
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==13)
                            {
                                ed.warpmod=true;
                                ed.warpent=edentity.size();
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),13);
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==14)
                            {
                                //Warp lines
                                if(ed.level[ed.levx+(ed.maxwidth*ed.levy)].warpdir==0)
                                {
                                    if(ed.tilex==0)
                                    {
                                        addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),50,0);
                                    }
                                    else if(ed.tilex==39)
                                    {
                                        addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),50,1);
                                    }
                                    else if(ed.tiley==0)
                                    {
                                        addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),50,2);
                                    }
                                    else if(ed.tiley==29)
                                    {
                                        addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),50,3);
                                    }
                                    else
                                    {
                                        ed.note="ERROR: Warp lines must be on edges";
                                        ed.notedelay=45;
                                    }
                                }
                                else
                                {
                                    ed.note="ERROR: Cannot have both warp types";
                                    ed.notedelay=45;
                                }
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==15)  //Crewmate
                            {
                                if(ed.numcrewmates()<100)
                                {
                                    addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),15,1 + int(fRandom() * 5));
                                    ed.lclickdelay=1;
                                }
                                else
                                {
                                    ed.note="ERROR: Max number of crewmates is 100";
                                    ed.notedelay=45;
                                }
                            }
                            else if(ed.drawmode==16)  //Start Point
                            {
                                //If there is another start point, destroy it
                                for(size_t i=0; i<edentity.size(); i++)
                                {
                                    if(edentity[i].t==16)
                                    {
                                        removeedentity(i);
                                        i--;
                                    }
                                }
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),16,0);
                                ed.lclickdelay=1;
                            }
                        }
                        else if(tmp == -1)
                        {
                            //Important! Do nothing, or else Undefined Behavior will happen
                        }
                        else if(edentity[tmp].t==1)
                        {
                            edentity[tmp].p1=(edentity[tmp].p1+1)%4;
                            ed.lclickdelay=1;
                        }
                        else if(edentity[tmp].t==2)
                        {
                            if(edentity[tmp].p1>=5)
                            {
                                edentity[tmp].p1=(edentity[tmp].p1+1)%9;
                                if(edentity[tmp].p1<5) edentity[tmp].p1=5;
                            }
                            else
                            {
                                edentity[tmp].p1=(edentity[tmp].p1+1)%4;
                            }
                            ed.lclickdelay=1;
                        }
                        else if(edentity[tmp].t==10)
                        {
                            edentity[tmp].p1=(edentity[tmp].p1+1)%2;
                            ed.lclickdelay=1;
                        }
                        else if(edentity[tmp].t==11)
                        {
                            edentity[tmp].p1=(edentity[tmp].p1+1)%2;
                            ed.lclickdelay=1;
                        }
                        else if(edentity[tmp].t==15)
                        {
                            edentity[tmp].p1=(edentity[tmp].p1+1)%6;
                            ed.lclickdelay=1;
                        }
                        else if(edentity[tmp].t==16)
                        {
                            edentity[tmp].p1=(edentity[tmp].p1+1)%2;
                            ed.lclickdelay=1;
                        }
                        else if(edentity[tmp].t==17)
                        {
                            ed.roomtextmod=true;
                            ed.oldenttext=edentity[tmp].scriptname;
                            ed.textent=tmp;
                            ed.textentry=true;
                            key.enabletextentry();
                            key.keybuffer=ed.oldenttext;
                            ed.lclickdelay=1;
                        }
                        else if(edentity[tmp].t==18 || edentity[tmp].t==19)
                        {
                            ed.scripttextmod=true;
                            ed.oldenttext=edentity[tmp].scriptname;
                            ed.textent=tmp;
                            ed.textentry=true;
                            key.enabletextentry();
                            key.keybuffer=ed.oldenttext;
                            ed.lclickdelay=1;
                        }
                    }
                }
                else
                {
                    ed.lclickdelay=0;
                }

                if(key.rightbutton)
                {
                    //place tiles
                    if(ed.xmod)
                    {
                        for(int j=-2; j<3; j++)
                        {
                            for(int i=-2; i<3; i++)
                            {
                                ed.placetilelocal(ed.tilex+i, ed.tiley+j, 0);
                            }
                        }
                    }
                    else if(ed.zmod)
                    {
                        for(int j=-1; j<2; j++)
                        {
                            for(int i=-1; i<2; i++)
                            {
                                ed.placetilelocal(ed.tilex+i, ed.tiley+j, 0);
                            }
                        }
                    }
                    else
                    {
                        ed.placetilelocal(ed.tilex, ed.tiley, 0);
                    }
                    for(size_t i=0; i<edentity.size(); i++)
                    {
                        if(edentity[i].x==ed.tilex + (ed.levx*40)&& edentity[i].y==ed.tiley+ (ed.levy*30))
                        {
                            removeedentity(i);
                        }
                    }
                }

                if(key.middlebutton)
                {
                    ed.dmtile=ed.contents[ed.tilex + (ed.levx*40) + ed.vmult[ed.tiley + (ed.levy*30)]];
                }
            }
        }
    }

    if(ed.updatetiles && ed.level[ed.levx + (ed.levy*ed.maxwidth)].directmode==0)
    {
        ed.updatetiles=false;
        //Correctly set the tiles in the current room
        switch(ed.level[ed.levx + (ed.levy*ed.maxwidth)].tileset)
        {
        case 0: //The Space Station
            for(int j=0; j<30; j++)
            {
                for(int i=0; i<40; i++)
                {
                    int temp=i+(ed.levx*40) + ed.vmult[j+(ed.levy*30)];
                    if(ed.contents[temp]>=3 && ed.contents[temp]<80)
                    {
                        //Fix spikes
                        ed.contents[temp]=ed.spikedir(i,j);
                    }
                    else if(ed.contents[temp]==2 || ed.contents[temp]>=680)
                    {
                        //Fix background
                        ed.contents[temp]=ed.backedgetile(i,j)+ed.backbase(ed.levx,ed.levy);
                    }
                    else if(ed.contents[temp]>0)
                    {
                        //Fix tiles
                        ed.contents[temp]=ed.edgetile(i,j)+ed.base(ed.levx,ed.levy);
                    }
                }
            }
            break;
        case 1: //Outside
            for(int j=0; j<30; j++)
            {
                for(int i=0; i<40; i++)
                {
                    int temp=i+(ed.levx*40) + ed.vmult[j+(ed.levy*30)];
                    if(ed.contents[temp]>=3 && ed.contents[temp]<80)
                    {
                        //Fix spikes
                        ed.contents[temp]=ed.spikedir(i,j);
                    }
                    else if(ed.contents[temp]==2 || ed.contents[temp]>=680)
                    {
                        //Fix background
                        ed.contents[temp]=ed.outsideedgetile(i,j)+ed.backbase(ed.levx,ed.levy);
                    }
                    else if(ed.contents[temp]>0)
                    {
                        //Fix tiles
                        ed.contents[temp]=ed.edgetile(i,j)+ed.base(ed.levx,ed.levy);
                    }
                }
            }
            break;
        case 2: //Lab
            for(int j=0; j<30; j++)
            {
                for(int i=0; i<40; i++)
                {
                    int temp=i+(ed.levx*40) + ed.vmult[j+(ed.levy*30)];
                    if(ed.contents[temp]>=3 && ed.contents[temp]<80)
                    {
                        //Fix spikes
                        ed.contents[temp]=ed.labspikedir(i,j, ed.level[ed.levx + (ed.maxwidth*ed.levy)].tilecol);
                    }
                    else if(ed.contents[temp]==2 || ed.contents[temp]>=680)
                    {
                        //Fix background
                        ed.contents[temp]=713;
                    }
                    else if(ed.contents[temp]>0)
                    {
                        //Fix tiles
                        ed.contents[temp]=ed.edgetile(i,j)+ed.base(ed.levx,ed.levy);
                    }
                }
            }
            break;
        case 3: //Warp Zone/Intermission
            for(int j=0; j<30; j++)
            {
                for(int i=0; i<40; i++)
                {
                    int temp=i+(ed.levx*40) + ed.vmult[j+(ed.levy*30)];
                    if(ed.contents[temp]>=3 && ed.contents[temp]<80)
                    {
                        //Fix spikes
                        ed.contents[temp]=ed.spikedir(i,j);
                    }
                    else if(ed.contents[temp]==2 || ed.contents[temp]>=680)
                    {
                        //Fix background
                        ed.contents[temp]=713;
                    }
                    else if(ed.contents[temp]>0)
                    {
                        //Fix tiles
                        ed.contents[temp]=ed.edgetile(i,j)+ed.base(ed.levx,ed.levy);
                    }
                }
            }
            break;
        case 4: //The ship
            for(int j=0; j<30; j++)
            {
                for(int i=0; i<40; i++)
                {
                    int temp=i+(ed.levx*40) + ed.vmult[j+(ed.levy*30)];
                    if(ed.contents[temp]>=3 && ed.contents[temp]<80)
                    {
                        //Fix spikes
                        ed.contents[temp]=ed.spikedir(i,j);
                    }
                    else if(ed.contents[temp]==2 || ed.contents[temp]>=680)
                    {
                        //Fix background
                        ed.contents[temp]=ed.backedgetile(i,j)+ed.backbase(ed.levx,ed.levy);
                    }
                    else if(ed.contents[temp]>0)
                    {
                        //Fix tiles
                        ed.contents[temp]=ed.edgetile(i,j)+ed.base(ed.levx,ed.levy);
                    }
                }
            }
            break;
        case 5: //The Tower
            break;
        case 6: //Custom Set 1
            break;
        case 7: //Custom Set 2
            break;
        case 8: //Custom Set 3
            break;
        case 9: //Custom Set 4
            break;
        }
    }
}

int editorclass::numtrinkets()
{
    int temp = 0;
    for (size_t i = 0; i < edentity.size(); i++)
    {
        if (edentity[i].t == 9)
        {
            temp++;
        }
    }
    return temp;
}

int editorclass::numcrewmates()
{
    int temp = 0;
    for (size_t i = 0; i < edentity.size(); i++)
    {
        if (edentity[i].t == 15)
        {
            temp++;
        }
    }
    return temp;
}

#endif /* NO_CUSTOM_LEVELS */
