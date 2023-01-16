#ifndef LOCALIZATIONSTORAGE_H
#define LOCALIZATIONSTORAGE_H

#include <tinyxml2.h>

#include "Textbook.h"
#include "XMLUtils.h"

extern "C"
{
#include <c-hashmap/map.h>
}

#if defined(LOCALIZATIONSTORAGE_CPP)
    #define LS_INTERN
#else
    #define LS_INTERN extern
#endif

namespace loc
{

#if defined(LOCALIZATION_CPP) || defined(LOCALIZATIONSTORAGE_CPP) || defined(LOCALIZATIONMAINT_CPP)
    LS_INTERN Textbook textbook_main;
    LS_INTERN Textbook textbook_custom;

    LS_INTERN hashmap* map_translation;
    LS_INTERN hashmap* map_translation_plural;
    LS_INTERN std::string number[101]; /* 0..100 */
    LS_INTERN unsigned char number_plural_form[200]; /* [0..99] for 0..99, [100..199] for *00..*99 */
    LS_INTERN hashmap* map_translation_cutscene;
    LS_INTERN hashmap* map_translation_cutscene_custom;
    LS_INTERN hashmap* map_translation_roomnames_special;

    #define MAP_MAX_X 54
    #define MAP_MAX_Y 56
    #define CUSTOM_MAP_MAX_X 19
    #define CUSTOM_MAP_MAX_Y 19
    LS_INTERN const char* translation_roomnames[MAP_MAX_Y+1][MAP_MAX_X+1];
    LS_INTERN const char* explanation_roomnames[MAP_MAX_Y+1][MAP_MAX_X+1];
    LS_INTERN const char* translation_roomnames_custom[CUSTOM_MAP_MAX_Y+1][CUSTOM_MAP_MAX_X+1];
    LS_INTERN const char* explanation_roomnames_custom[CUSTOM_MAP_MAX_Y+1][CUSTOM_MAP_MAX_X+1];
#endif


struct TextOverflow
{
    std::string lang;
    const char* text;
    unsigned short max_w, max_h;
    unsigned short max_w_px, max_h_px;
    bool multiline;
    uint32_t flags;
};

extern std::vector<TextOverflow> text_overflows;


bool load_lang_doc(
    const std::string& cat,
    tinyxml2::XMLDocument& doc,
    const std::string& langcode = lang,
    const std::string& asset_cat = ""
);

unsigned char form_for_count(int n);

void unloadtext_custom(void);
void resettext(bool final_shutdown);

const char* get_level_original_lang(tinyxml2::XMLHandle& hDoc);

bool store_roomname_translation(bool custom_level, int roomx, int roomy, const char* tra, const char* explanation);

bool fix_room_coords(bool custom_level, int* roomx, int* roomy);

void loadtext(bool check_max);
void loadtext_custom(const char* custom_path);
void loadlanguagelist(void);

const char* map_lookup_text(hashmap* map, const char* eng, const char* fallback);

char* add_disambiguator(char disambiguator, const char* original_string, size_t* ext_alloc_len);

} /* namespace loc */

#undef LS_INTERN

#endif /* LOCALIZATIONSTORAGE_H */
