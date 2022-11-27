#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include <stdint.h>
#include <string>
#include <vector>

/* The translator menu will appear in any of the following circumstances:
 * - The "lang" folder is NOT next to data.zip, but it is found in "desktop_version" within which the game is running
 * - The command line argument "-translator" is passed
 * - ALWAYS_SHOW_TRANSLATOR_MENU is defined
 */
// #define ALWAYS_SHOW_TRANSLATOR_MENU

namespace loc
{

struct LangMeta
{
    bool active; // = true, language is shown in the list
    std::string code;
    std::string nativename;
    std::string credit;
    std::string action_hint;
    bool autowordwrap; // = true; enable automatic wordwrapping
    bool toupper; // = true; enable automatic full-caps for menu options
    bool toupper_i_dot; // = false; enable Turkish i mapping when uppercasing
    bool toupper_lower_escape_char; // = false; enable ~ to mark lowercase letters for uppercasing
    std::string menu_select;
    std::string menu_select_tight;
    unsigned char font_w;
    unsigned char font_h;
};

struct TextboxFormat
{
    const char* text;
    unsigned short wraplimit; // = 36*8-pad_left-pad_right; no effect if tt or !langmeta.autowordwrap
    unsigned short wraplimit_raw; // original value of wraplimit, only used for language file sync
    bool tt; // teletype, don't auto-wordwrap
    bool centertext; // whether the text should be centered inside the box
    unsigned char pad_left; // pad with X characters
    unsigned char pad_right;
    unsigned short padtowidth; // pad to X pixels (0 to disable)
};

extern bool lang_set;
extern std::string lang;
extern std::string lang_custom;
extern LangMeta langmeta;
extern std::vector<LangMeta> languagelist;
extern int languagelist_curlang;
extern bool show_translator_menu;
extern size_t limitscheck_current_overflow;

extern int n_untranslated_roomnames;
extern int n_unexplained_roomnames;
extern int n_untranslated_roomnames_custom;
extern int n_unexplained_roomnames_custom;
extern int n_untranslated_roomnames_area[9];

enum n_untranslated_index
{
    UNTRANSLATED_STRINGS = 0,
    UNTRANSLATED_NUMBERS,
    UNTRANSLATED_STRINGS_PLURAL,
    UNTRANSLATED_CUTSCENES,
    UNTRANSLATED_ROOMNAMES_SPECIAL,
    COUNT_UNTRANSLATED_INDEX
};
extern int n_untranslated[COUNT_UNTRANSLATED_INDEX];


const LangMeta* get_langmeta(void);

const char* gettext(const char* eng);
const char* gettext_plural(const char* eng_plural, const char* eng_singular, int count);
void gettext_plural_fill(char* buf, size_t buf_len, const char* eng_plural, const char* eng_singular, const char* args_index, ...);
std::string getnumber(int n);
const TextboxFormat* gettext_cutscene(const std::string& script_id, const std::string& eng, char textcase);
const char* get_roomname_explanation(bool custom_level, int roomx, int roomy);
const char* get_roomname_translation(bool custom_level, int roomx, int roomy);
const char* gettext_roomname(bool custom_level, int roomx, int roomy, const char* eng, bool special);
const char* gettext_roomname_special(const char* eng);

bool is_cutscene_translated(const std::string& script_id);

uint32_t toupper_ch(uint32_t ch);
std::string toupper(const std::string& lower);
std::string remove_toupper_escape_chars(const std::string& _s);

} /* namespace loc */

#endif /* LOCALIZATION_H */
