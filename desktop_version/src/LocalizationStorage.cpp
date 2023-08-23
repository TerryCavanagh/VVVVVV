#define LOCALIZATIONSTORAGE_CPP
#include "Localization.h"
#include "LocalizationStorage.h"

#include "Alloc.h"
#include "Constants.h"
#include "CustomLevels.h"
#include "FileSystemUtils.h"
#include "Font.h"
#include "Graphics.h"
#include "Unused.h"
#include "UtilityClass.h"
#include "VFormat.h"
#include "Vlogging.h"


namespace loc
{

bool inited = false;
bool inited_custom = false;

char* custom_level_path = NULL;

std::vector<TextOverflow> text_overflows;

bool load_lang_doc(
    const std::string& cat,
    tinyxml2::XMLDocument& doc,
    const std::string& langcode /*= lang*/,
    const std::string& asset_cat /*= ""*/
)
{
    /* Load a language-related XML file.
     * cat is the "category", so "strings", "numbers", etc.
     *
     * asset_cat is only used when loading
     * from custom level assets is possible. */

    bool asset_loaded = false;
    if (!asset_cat.empty())
    {
        asset_loaded = FILESYSTEM_loadAssetTiXml2Document(("lang/" + langcode + "/" + asset_cat + ".xml").c_str(), doc);
    }
    if (!asset_loaded && !FILESYSTEM_loadTiXml2Document(("lang/" + langcode + "/" + cat + ".xml").c_str(), doc))
    {
        vlog_debug("Could not load language file %s/%s.", langcode.c_str(), cat.c_str());
        return false;
    }
    if (doc.Error())
    {
        vlog_error("Error parsing language file %s/%s: %s", langcode.c_str(), cat.c_str(), doc.ErrorStr());
        return false;
    }
    return true;
}

static void loadmeta(LangMeta& meta, const std::string& langcode = lang)
{
    meta.active = true;
    meta.code = langcode;
    meta.autowordwrap = true;
    meta.toupper = true;
    meta.toupper_i_dot = false;
    meta.toupper_lower_escape_char = false;
    meta.menu_select = "[ {label} ]";
    meta.menu_select_tight = "[{label}]";
    meta.font_idx = font::get_font_idx_8x8();

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;

    if (!load_lang_doc("meta", doc, langcode))
    {
        return;
    }

    FOR_EACH_XML_ELEMENT(hDoc, pElem)
    {
        const char* pKey = pElem->Value();
        const char* pText = pElem->GetText();
        if (pText == NULL)
        {
            pText = "";
        }

        if (SDL_strcmp(pKey, "active") == 0)
            meta.active = help.Int(pText);
        else if (SDL_strcmp(pKey, "nativename") == 0)
            meta.nativename = std::string(pText);
        else if (SDL_strcmp(pKey, "credit") == 0)
            meta.credit = std::string(pText);
        else if (SDL_strcmp(pKey, "action_hint") == 0)
            meta.action_hint = std::string(pText);
        else if (SDL_strcmp(pKey, "gamepad_hint") == 0)
            meta.gamepad_hint = std::string(pText);
        else if (SDL_strcmp(pKey, "autowordwrap") == 0)
            meta.autowordwrap = help.Int(pText);
        else if (SDL_strcmp(pKey, "toupper") == 0)
            meta.toupper = help.Int(pText);
        else if (SDL_strcmp(pKey, "toupper_i_dot") == 0)
            meta.toupper_i_dot = help.Int(pText);
        else if (SDL_strcmp(pKey, "toupper_lower_escape_char") == 0)
            meta.toupper_lower_escape_char = help.Int(pText);
        else if (SDL_strcmp(pKey, "menu_select") == 0)
            meta.menu_select = std::string(pText);
        else if (SDL_strcmp(pKey, "menu_select_tight") == 0)
            meta.menu_select_tight = std::string(pText);
        else if (SDL_strcmp(pKey, "font") == 0)
            font::find_main_font_by_name(pText, &meta.font_idx);
    }
}

static void map_store_translation(Textbook* textbook, hashmap* map, const char* eng, const char* tra)
{
    /* Add the texts to the given textbook and set the translation in the given hashmap. */
    if (eng == NULL)
    {
        return;
    }
    if (tra == NULL)
    {
        tra = "";
    }
    const char* tb_eng = textbook_store(textbook, eng);
    const char* tb_tra = textbook_store(textbook, tra);

    if (tb_eng == NULL || tb_tra == NULL)
    {
        return;
    }

    hashmap_set(map, tb_eng, SDL_strlen(tb_eng), (uintptr_t) tb_tra);
}

unsigned char form_for_count(int n)
{
    int n_ix;
    if (n > -100 && n < 100)
    {
        /* Plural forms for negative numbers are debatable in any language I'd imagine...
         * But they shouldn't appear anyway unless there's a bug or you're asking for it.
         * Or do YOU ever get -10 deaths while collecting -1 trinket? */
        n_ix = SDL_abs(n);
    }
    else
    {
        /* Plural forms for 100 and above always just keep repeating. Thank goodness. */
        n_ix = SDL_abs(n % 100) + 100;
    }

    return number_plural_form[n_ix];
}

static void callback_free_map_value(void* key, size_t ksize, uintptr_t value, void* usr)
{
    UNUSED(key);
    UNUSED(ksize);
    UNUSED(usr);

    hashmap_free((hashmap*) value);
}

static void resettext_custom(bool final_shutdown)
{
    /* Reset/initialize custom level strings only.
     * If final_shutdown, this just does a last cleanup of any allocations,
     * otherwise it makes storage ready for first use (or reuse by a new language). */

    if (inited_custom)
    {
        hashmap_iterate(map_translation_cutscene_custom, callback_free_map_value, NULL);
        hashmap_free(map_translation_cutscene_custom);

        textbook_clear(&textbook_custom);
    }
    else if (!final_shutdown)
    {
        inited_custom = true;

        textbook_init(&textbook_custom);
    }

    if (!final_shutdown)
    {
        map_translation_cutscene_custom = hashmap_create();

        SDL_zeroa(translation_roomnames_custom);
        SDL_zeroa(explanation_roomnames_custom);

        n_untranslated_roomnames_custom = 0;
        n_unexplained_roomnames_custom = 0;
    }
}

void unloadtext_custom(void)
{
    resettext_custom(false);

    loc::lang_custom = "";

    VVV_free(custom_level_path);
    custom_level_path = NULL;
}

void resettext(bool final_shutdown)
{
    /* Reset/initialize strings.
     * If final_shutdown, this just does a last cleanup of any allocations,
     * otherwise it makes storage ready for first use (or reuse by a new language). */

    if (inited)
    {
        hashmap_free(map_translation);
        hashmap_iterate(map_translation_cutscene, callback_free_map_value, NULL);
        hashmap_free(map_translation_cutscene);
        hashmap_free(map_translation_plural);
        hashmap_free(map_translation_roomnames_special);

        textbook_clear(&textbook_main);
    }
    else if (!final_shutdown)
    {
        inited = true;

        textbook_init(&textbook_main);
    }

    if (!final_shutdown)
    {
        map_translation = hashmap_create();
        map_translation_cutscene = hashmap_create();
        map_translation_plural = hashmap_create();

        for (size_t i = 0; i <= 100; i++)
        {
            number[i] = "";
        }
        SDL_zeroa(number_plural_form);

        SDL_zeroa(translation_roomnames);
        SDL_zeroa(explanation_roomnames);

        n_untranslated_roomnames = 0;
        n_unexplained_roomnames = 0;
        SDL_zeroa(n_untranslated_roomnames_area);

        SDL_zeroa(n_untranslated);

        map_translation_roomnames_special = hashmap_create();
    }

    resettext_custom(final_shutdown);
}

bool parse_max(const char* max, unsigned short* max_w, unsigned short* max_h)
{
    /* Parse a max string, like "33" or "33*3", into two shorts.
     * Returns true if successful and max_w/max_h have gotten valid values, false otherwise. */
    if (max == NULL)
    {
        return false;
    }

    char* max_mut = SDL_strdup(max);
    if (max_mut == NULL)
    {
        return false;
    }

    char* asterisk = SDL_strchr(max_mut, '*');
    if (asterisk != NULL)
    {
        asterisk[0] = '\0';
        *max_h = (unsigned short) help.Int(&asterisk[1], 0);
    }
    else
    {
        *max_h = 1;
    }
    *max_w = (unsigned short) help.Int(max_mut, 0);

    VVV_free(max_mut);

    return *max_w != 0 && *max_h != 0;
}

static bool max_check_string(const char* str, const char* max)
{
    /* Stores a detected overflow in the overflows vector, returns true if this happened */
    unsigned short max_w, max_h;
    if (str == NULL || !parse_max(max, &max_w, &max_h))
    {
        return false;
    }

    /* Special case that must ALWAYS be 2 lines even when the font is bigger */
    if (SDL_strcmp(str, "You have rescued a crew member!") == 0 && max_h == 1)
    {
        max_h = 2;
    }

    uint8_t font_idx = get_langmeta()->font_idx;
    uint32_t print_flags = PR_FONT_IDX(font_idx) | PR_CJK_LOW;
    uint8_t font_w = 8;
    uint8_t font_h = 8;
    font::glyph_dimensions(print_flags, &font_w, &font_h);

    unsigned short max_w_px = max_w * 8;
    unsigned short max_h_px = max_h * 10;

    bool does_overflow = false;

    if (max_h == 1)
    {
        max_h_px = font_h;
        does_overflow = font::len(print_flags, str) > (int) max_w_px;
    }
    else
    {
        short lines;
        font::string_wordwrap(print_flags, str, max_w_px, &lines);
        does_overflow = lines*SDL_max(10, font_h) > (short) max_h_px;
    }

    // Convert max_w and max_h from 8x8 into local
    max_w = max_w_px / font_w;
    max_h = max_h_px / SDL_max(10, font_h);

    if (does_overflow)
    {
        TextOverflow overflow;
        overflow.lang = lang;
        overflow.text = textbook_store(&textbook_main, str);
        overflow.max_w = max_w;
        overflow.max_h = max_h;
        overflow.max_w_px = max_w_px;
        overflow.max_h_px = max_h_px;
        overflow.multiline = max_h > 1;
        overflow.flags = print_flags;

        text_overflows.push_back(overflow);

        vlog_warn("\"%s\" DOESN'T FIT into %s which is %dx%d or %dx%dpx",
            str, max, max_w, max_h, max_w_px, max_h_px
        );
    }
    else
    {
        vlog_debug("\"%s\" fits into %s which is %dx%d or %dx%dpx",
            str, max, max_w, max_h, max_w_px, max_h_px
        );
    }

    return does_overflow;
}

static void max_check_string_plural(unsigned char form, const char* str, const char* max, const char* var, unsigned int expect)
{
    if (str == NULL || var == NULL)
    {
        return;
    }

    /* Create an args index from just the name of the variable.
     * Also get rid of all other placeholders.*/
    char args_index[60];
    vformat_buf(args_index, sizeof(args_index), "{var}:int, _:int", "var:str", var);

    char buf[20*SCREEN_WIDTH_CHARS + 1];

    if (expect > 100)
    {
        /* Treat `expect` as a single example, it's the number of digits that's most important */
        if (form_for_count(expect) == form)
        {
            vformat_buf(buf, sizeof(buf), str, args_index, expect, 0);

            max_check_string(buf, max);
        }
    }
    else
    {
        /* Test all numbers from 0 to `expect`, since if we have wordy numbers, they have differing lengths */
        for (unsigned int test = 0; test <= expect; test++)
        {
            if (form_for_count(test) == form)
            {
                vformat_buf(buf, sizeof(buf), str, args_index, test, 0);

                if (max_check_string(buf, max))
                {
                    /* One is enough */
                    break;
                }
            }
        }
    }
}

static void tally_untranslated(const char* tra, int* counter)
{
    /* Count this translation in the untranslated count if it's untranslated. */
    if (!show_translator_menu)
    {
        return;
    }

    if (tra == NULL || tra[0] == '\0')
    {
        (*counter)++;
    }
}

static void loadtext_strings(bool check_max)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;

    if (!load_lang_doc("strings", doc))
    {
        return;
    }

    FOR_EACH_XML_ELEMENT(hDoc, pElem)
    {
        EXPECT_ELEM(pElem, "string");

        const char* eng = pElem->Attribute("english");
        const char* tra = pElem->Attribute("translation");

        char textcase = pElem->UnsignedAttribute("case", 0);

        if (textcase == 0)
        {
            map_store_translation(
                &textbook_main,
                map_translation,
                eng,
                tra
            );
        }
        else
        {
            /* Only prefix with a disambiguator if a specific case number is set */
            char* eng_prefixed = add_disambiguator(textcase, eng, NULL);
            if (eng_prefixed == NULL)
            {
                continue;
            }
            map_store_translation(
                &textbook_main,
                map_translation,
                eng_prefixed,
                tra
            );
            VVV_free(eng_prefixed);
        }

        /* Only tally an untranslated string if English isn't blank */
        if (eng != NULL && eng[0] != '\0')
        {
            tally_untranslated(tra, &n_untranslated[UNTRANSLATED_STRINGS]);
        }
        if (check_max)
        {
            /* VFormat placeholders distort the limits check.
             * (max_check_string ignores NULL strings.) */
            char* filled = vformat_alloc(tra, "_:int", 0);
            max_check_string(filled, pElem->Attribute("max"));
            VVV_free(filled);
        }
    }
}

static void loadtext_strings_plural(bool check_max)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;

    if (!load_lang_doc("strings_plural", doc))
    {
        return;
    }

    FOR_EACH_XML_ELEMENT(hDoc, pElem)
    {
        EXPECT_ELEM(pElem, "string");

        const char* eng_plural = pElem->Attribute("english_plural");
        if (eng_plural == NULL)
        {
            continue;
        }

        tinyxml2::XMLElement* subElem;
        FOR_EACH_XML_SUB_ELEMENT(pElem, subElem)
        {
            EXPECT_ELEM(subElem, "translation");

            unsigned char form = subElem->IntAttribute("form", 0);
            char* key = add_disambiguator(form+1, eng_plural, NULL);
            if (key == NULL)
            {
                continue;
            }

            map_store_translation(
                &textbook_main,
                map_translation_plural,
                key,
                subElem->Attribute("translation")
            );

            VVV_free(key);

            tally_untranslated(subElem->Attribute("translation"), &n_untranslated[UNTRANSLATED_STRINGS_PLURAL]);
            if (check_max)
            {
                max_check_string_plural(
                    form, subElem->Attribute("translation"),
                    pElem->Attribute("max"),
                    pElem->Attribute("var"), pElem->UnsignedAttribute("expect", 101)
                );
            }
        }
    }
}

static bool get_level_lang_path(bool custom_level, const char* cat, std::string& doc_path, std::string& doc_path_asset)
{
    /* Calculate the path to a translation file for either the MAIN GAME or
     * a CUSTOM LEVEL. cat can be "roomnames", "cutscenes", etc.
     *
     * doc_path and doc_path_asset are "out" parameters, and will be set to
     * the appropriate filenames to use for language files outside of or
     * inside level assets respectively (translations for custom levels can
     * live in the main language folders too)
     *
     * Returns whether this is a (valid) custom level path. */

    if (custom_level
        && custom_level_path != NULL
        && SDL_strncmp(custom_level_path, "levels/", 7) == 0
        && SDL_strlen(custom_level_path) > (sizeof(".vvvvvv")-1)
    )
    {
        /* Get rid of .vvvvvv */
        size_t len = SDL_strlen(custom_level_path)-7;
        doc_path = std::string(custom_level_path, len);
        doc_path.append("/custom_");
        doc_path.append(cat);

        /* For the asset path, also get rid of the levels/LEVELNAME/ */
        doc_path_asset = "custom_";
        doc_path_asset.append(cat);

        return true;
    }
    else
    {
        doc_path = cat;
        doc_path_asset = "";

        return false;
    }
}

const char* get_level_original_lang(tinyxml2::XMLHandle& hDoc)
{
    /* cutscenes and roomnames files can specify the original language as
     * an attribute of the root tag to change the attribute names of the
     * original text (normally "english"). This makes level translations
     * less confusing if the original language isn't English. */

    const char* original = NULL;
    tinyxml2::XMLElement* pRoot = hDoc.FirstChildElement().ToElement();
    if (pRoot != NULL)
    {
        original = pRoot->Attribute("original");
    }
    if (original == NULL)
    {
        original = "english";
    }
    return original;
}

static std::string& get_level_lang_code(bool custom_level)
{
    if (!custom_level || lang_custom == "")
    {
        return lang;
    }

    return lang_custom;
}

static void loadtext_cutscenes(bool custom_level)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;

    std::string doc_path;
    std::string doc_path_asset;
    bool valid_custom_level = get_level_lang_path(custom_level, "cutscenes", doc_path, doc_path_asset);
    if (custom_level && !valid_custom_level)
    {
        return;
    }
    if (!load_lang_doc(doc_path, doc, get_level_lang_code(custom_level), doc_path_asset))
    {
        return;
    }

    Textbook* textbook;
    hashmap* map;
    if (custom_level)
    {
        textbook = &textbook_custom;
        map = map_translation_cutscene_custom;
    }
    else
    {
        textbook = &textbook_main;
        map = map_translation_cutscene;
    }

    const char* original = get_level_original_lang(hDoc);

    FOR_EACH_XML_ELEMENT(hDoc, pElem)
    {
        EXPECT_ELEM(pElem, "cutscene");

        const char* script_id = textbook_store(textbook, pElem->Attribute("id"));
        if (script_id == NULL)
        {
            continue;
        }

        hashmap* cutscene_map = hashmap_create();
        hashmap_set_free(
            map,
            script_id,
            SDL_strlen(script_id),
            (uintptr_t) cutscene_map,
            callback_free_map_value,
            NULL
        );

        tinyxml2::XMLElement* subElem;
        FOR_EACH_XML_SUB_ELEMENT(pElem, subElem)
        {
            EXPECT_ELEM(subElem, "dialogue");

            const char* eng = subElem->Attribute(original);
            const char* tra = subElem->Attribute("translation");
            if (!custom_level)
            {
                tally_untranslated(tra, &n_untranslated[UNTRANSLATED_CUTSCENES]);
            }
            if (eng == NULL || tra == NULL)
            {
                continue;
            }
            const std::string eng_unwrapped = font::string_unwordwrap(eng);
            char* eng_prefixed = add_disambiguator(subElem->UnsignedAttribute("case", 1), eng_unwrapped.c_str(), NULL);
            if (eng_prefixed == NULL)
            {
                continue;
            }
            const char* tb_eng = textbook_store(textbook, eng_prefixed);
            const char* tb_tra = textbook_store(textbook, tra);
            VVV_free(eng_prefixed);
            if (tb_eng == NULL || tb_tra == NULL)
            {
                continue;
            }
            TextboxFormat format;
            format.text = tb_tra;
            format.tt = subElem->BoolAttribute("tt", false);
            format.centertext = subElem->BoolAttribute("centertext", false);
            format.pad_left = subElem->UnsignedAttribute("pad_left", 0);
            format.pad_right = subElem->UnsignedAttribute("pad_right", 0);
            unsigned short pad = subElem->UnsignedAttribute("pad", 0);
            format.pad_left += pad;
            format.pad_right += pad;
            format.wraplimit_raw = subElem->UnsignedAttribute("wraplimit", 0);
            format.wraplimit = format.wraplimit_raw;
            if (format.wraplimit == 0)
            {
                format.wraplimit = 36*8 - (format.pad_left+format.pad_right)*8;
            }
            format.padtowidth = subElem->UnsignedAttribute("padtowidth", 0);

            const TextboxFormat* tb_format = (TextboxFormat*) textbook_store_raw(
                textbook,
                &format,
                sizeof(TextboxFormat)
            );
            if (tb_format == NULL)
            {
                continue;
            }
            hashmap_set(cutscene_map, tb_eng, SDL_strlen(tb_eng), (uintptr_t) tb_format);
        }
    }
}

static void loadtext_numbers(void)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;

    if (!load_lang_doc("numbers", doc))
    {
        return;
    }

    FOR_EACH_XML_ELEMENT(hDoc, pElem)
    {
        EXPECT_ELEM(pElem, "number");

        const char* value_str = pElem->Attribute("value");
        int value = help.Int(value_str);
        if (value >= 0 && value <= 100)
        {
            const char* tra = pElem->Attribute("translation");
            if (tra == NULL)
            {
                tra = "";
            }
            number[value] = std::string(tra);

            tally_untranslated(tra, &n_untranslated[UNTRANSLATED_NUMBERS]);
        }
        if (value >= 0 && value <= 199)
        {
            int form = pElem->IntAttribute("form", 0);
            number_plural_form[value] = form;
            if (value < 100)
            {
                number_plural_form[value+100] = form;
            }
        }
    }
}

bool fix_room_coords(bool custom_level, int* roomx, int* roomy)
{
    *roomx %= 100;
    *roomy %= 100;

    if (!custom_level && *roomx == 9 && *roomy == 4)
    {
        // The Tower has two rooms, unify them
        *roomy = 9;
    }

    int max_x = MAP_MAX_X;
    int max_y = MAP_MAX_Y;
    if (custom_level)
    {
        max_x = CUSTOM_MAP_MAX_X;
        max_y = CUSTOM_MAP_MAX_Y;
    }

    return !(*roomx < 0 || *roomy < 0 || *roomx > max_x || *roomy > max_y);
}

static unsigned coords_to_area(int roomx, int roomy)
{
    if (!fix_room_coords(false, &roomx, &roomy))
    {
        return false;
    }

    /* We want to know per-area how many room names are untranslated... */
    enum area_letter {
        _, /* None */
        S, /* SS1 */
        L, /* Lab */
        T, /* Tower */
        Y, /* SS2 */
        W, /* Warp */
        I, /* Intermission */
        G, /* Gravitron */
        F  /* Final */
    };
    static enum area_letter area_map[MAP_MAX_Y+1][MAP_MAX_Y+1] = {
        {_,L,L,L,L,L,L,L,_,T,_,_,_,W,W,W,W,W,W,W},
        {_,L,L,L,L,L,L,_,_,T,_,_,_,_,W,W,W,W,W,W},
        {_,_,_,_,L,_,_,_,_,T,_,_,_,_,W,W,W,W,W,W},
        {_,_,_,_,L,_,_,_,_,T,_,_,S,S,S,S,W,W,W,W},
        {_,_,L,L,L,_,_,_,_,T,T,T,S,S,S,S,_,_,_,_},
        {_,_,_,_,_,_,_,_,_,T,Y,Y,S,S,S,S,_,_,_,_},
        {_,_,_,_,_,_,_,_,_,T,Y,Y,S,S,S,S,S,_,_,_},
        {_,_,_,_,_,_,_,_,_,T,Y,Y,S,S,S,S,S,S,S,_},
        {_,_,_,_,_,_,_,_,_,T,_,_,_,Y,Y,S,Y,Y,Y,_},
        {_,_,_,_,_,_,_,_,T,T,_,_,_,Y,Y,Y,Y,Y,Y,_},
        {_,_,_,_,_,_,_,_,_,T,_,_,_,Y,Y,Y,Y,Y,Y,_},
        {_,_,_,_,_,_,_,_,_,T,_,Y,Y,Y,Y,Y,Y,Y,Y,_},
        {_,_,_,_,_,_,_,_,_,T,_,Y,Y,Y,Y,Y,Y,_,Y,_},
        {_,_,_,_,_,_,_,_,_,T,_,Y,Y,Y,Y,Y,Y,_,Y,_},
        {_,_,_,_,_,_,_,_,_,T,_,Y,Y,_,_,_,_,_,Y,_},
        {_,_,_,_,_,_,_,L,_,T,_,_,_,_,_,_,_,_,_,_},
        {_,_,L,L,L,L,L,L,_,T,_,_,_,_,_,_,_,_,_,_},
        {_,L,L,L,L,L,L,L,_,T,_,_,_,_,_,_,_,_,_,_},
        {L,L,L,L,L,_,_,L,_,T,_,_,_,_,_,_,_,_,_,_},
        {L,L,L,L,L,_,_,L,_,T,_,_,_,_,_,_,_,_,_,_}
    };
    static bool area_map_has_final = false;

    if (!area_map_has_final)
    {
        static const enum area_letter final_map[9][14] = {
            {_,_,_,_,_,_,_,_,_,_,_,_,G,F},
            {_,_,_,_,_,_,_,_,_,_,_,_,G,F},
            {_,_,_,_,_,_,_,_,_,_,_,_,G,F},
            {F,F,F,F,F,F,F,F,F,F,_,_,G,F},
            {F,F,F,F,F,_,F,F,F,F,_,_,G,F},
            {_,_,_,_,_,_,_,_,F,F,F,F,F,F},
            {_,_,_,_,_,F,F,F,F,F,F,_,_,_},
            {_,_,_,_,_,_,_,_,_,_,_,_,_,_},
            {I,I,I,I,I,I,I,I,I,I,I,I,I,I}
        };

        for (int y = 0; y < 9; y++)
        {
            for (int x = 0; x < 14; x++)
            {
                area_map[MAP_MAX_Y+1 - 9 + y][MAP_MAX_X+1 - 14 + x] = final_map[y][x];
            }
        }
        area_map_has_final = true;
    }

    if (area_map[roomy][roomx] == 0)
    {
        vlog_error("LocalizationStorage: Room %d,%d has no area associated with it", roomx, roomy);
    }

    return area_map[roomy][roomx];
}

static void update_left_counter(const char* old_text, const char* new_text, int* counter, int* counter_area)
{
    bool now_filled = new_text[0] != '\0';
    if ((old_text == NULL || old_text[0] == '\0') && now_filled)
    {
        (*counter)--;
        if (counter_area != NULL)
        {
            (*counter_area)--;
        }
    }
    else if (old_text != NULL && old_text[0] != '\0' && !now_filled)
    {
        (*counter)++;
        if (counter_area != NULL)
        {
            (*counter_area)++;
        }
    }
}

bool store_roomname_translation(bool custom_level, int roomx, int roomy, const char* tra, const char* explanation)
{
    if (!fix_room_coords(custom_level, &roomx, &roomy))
    {
        return false;
    }

    /* We have some arrays filled with pointers, and we need to change those pointers */
    const char** ptr_translation;
    const char** ptr_explanation;
    int* ptr_n_untranslated;
    int* ptr_n_untranslated_area = NULL;
    int* ptr_n_unexplained;
    if (custom_level)
    {
        ptr_translation = &translation_roomnames_custom[roomy][roomx];
        ptr_explanation = &explanation_roomnames_custom[roomy][roomx];
        ptr_n_untranslated = &n_untranslated_roomnames_custom;
        ptr_n_unexplained = &n_unexplained_roomnames_custom;
    }
    else
    {
        ptr_translation = &translation_roomnames[roomy][roomx];
        ptr_explanation = &explanation_roomnames[roomy][roomx];
        ptr_n_untranslated = &n_untranslated_roomnames;
        ptr_n_untranslated_area = &n_untranslated_roomnames_area[coords_to_area(roomx, roomy)];
        ptr_n_unexplained = &n_unexplained_roomnames;
    }

    if (tra != NULL)
    {
        update_left_counter(*ptr_translation, tra, ptr_n_untranslated, ptr_n_untranslated_area);
        *ptr_translation = textbook_store(&textbook_main, tra);
    }
    if (explanation != NULL)
    {
        update_left_counter(*ptr_explanation, explanation, ptr_n_unexplained, NULL);
        *ptr_explanation = textbook_store(&textbook_main, explanation);
    }

    return true;
}

static void loadtext_roomnames(bool custom_level)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;

    std::string doc_path;
    std::string doc_path_asset;
    bool valid_custom_level = get_level_lang_path(custom_level, "roomnames", doc_path, doc_path_asset);
    if (custom_level && !valid_custom_level)
    {
        return;
    }
    if (!load_lang_doc(doc_path, doc, get_level_lang_code(custom_level), doc_path_asset))
    {
        return;
    }

    const char* original = get_level_original_lang(hDoc);

    FOR_EACH_XML_ELEMENT(hDoc, pElem)
    {
        EXPECT_ELEM(pElem, "roomname");

        int x = pElem->IntAttribute("x", -1);
        int y = pElem->IntAttribute("y", -1);

        if (custom_level)
        {
            /* Extra safeguard: make sure the original room name matches! */
            const char* original_roomname = pElem->Attribute(original);
            if (original_roomname == NULL)
            {
                continue;
            }

            const RoomProperty* const room = cl.getroomprop(x, y);
            if (SDL_strcmp(original_roomname, room->roomname.c_str()) != 0)
            {
                continue;
            }

            n_untranslated_roomnames_custom++;
            n_unexplained_roomnames_custom++;
        }
        else
        {
            n_untranslated_roomnames++;
            n_unexplained_roomnames++;
            n_untranslated_roomnames_area[coords_to_area(x, y)]++;
        }

        store_roomname_translation(
            custom_level,
            x,
            y,
            pElem->Attribute("translation"),
            show_translator_menu ? pElem->Attribute("explanation") : NULL
        );
    }
}

static void loadtext_roomnames_special(void)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;

    if (!load_lang_doc("roomnames_special", doc))
    {
        return;
    }

    FOR_EACH_XML_ELEMENT(hDoc, pElem)
    {
        EXPECT_ELEM(pElem, "roomname");

        map_store_translation(
            &textbook_main,
            map_translation_roomnames_special,
            pElem->Attribute("english"),
            pElem->Attribute("translation")
        );

        tally_untranslated(pElem->Attribute("translation"), &n_untranslated[UNTRANSLATED_ROOMNAMES_SPECIAL]);
    }
}

void loadtext_custom(const char* custom_path)
{
    resettext_custom(false);
    if (custom_level_path == NULL && custom_path != NULL)
    {
        custom_level_path = SDL_strdup(custom_path);
    }
    loadtext_cutscenes(true);
    loadtext_roomnames(true);
}

void loadtext(bool check_max)
{
    resettext(false);
    loadmeta(langmeta);

    if (lang == "en")
    {
        if (show_translator_menu)
        {
            // We may still need the room name explanations
            loadtext_roomnames(false);
            n_untranslated_roomnames = 0;
            SDL_zeroa(n_untranslated_roomnames_area);
        }
    }
    else
    {
        loadtext_numbers();
        loadtext_strings(check_max);
        loadtext_strings_plural(check_max);
        loadtext_cutscenes(false);
        loadtext_roomnames(false);
        loadtext_roomnames_special();
    }

    if (custom_level_path != NULL)
    {
        loadtext_custom(NULL);
    }
}

void loadlanguagelist(void)
{
    // Load the list of languages for the language screen
    languagelist.clear();

    size_t opt = 0;
    languagelist_curlang = 0;
    EnumHandle handle = {};
    const char* code;
    while ((code = FILESYSTEM_enumerateLanguageCodes(&handle)) != NULL)
    {
        LangMeta meta;
        loadmeta(meta, code);
        if (meta.active)
        {
            languagelist.push_back(meta);

            if (SDL_strcmp(lang.c_str(), code) == 0)
            {
                languagelist_curlang = opt;
            }
            opt++;
        }
    }
    FILESYSTEM_freeEnumerate(&handle);
}


const char* map_lookup_text(hashmap* map, const char* eng, const char* fallback)
{
    uintptr_t ptr_tra;
    bool found = hashmap_get(map, eng, SDL_strlen(eng), &ptr_tra);
    const char* tra = (const char*) ptr_tra;

    if (found && tra != NULL && tra[0] != '\0')
    {
        return tra;
    }

    return fallback;
}

char* add_disambiguator(char disambiguator, const char* original_string, size_t* ext_alloc_len)
{
    /* Create a version of the string prefixed with the given byte.
     * This byte is used when the English string is just not enough to identify the correct translation.
     * It's needed to store plural forms, and when the same text appears multiple times in a cutscene.
     * Caller must VVV_free. */

    size_t alloc_len = 1+SDL_strlen(original_string)+1;

    char* alloc = (char*) SDL_malloc(alloc_len);
    if (alloc == NULL)
    {
        return NULL;
    }
    alloc[0] = disambiguator;
    SDL_memcpy(&alloc[1], original_string, alloc_len-1);

    if (ext_alloc_len != NULL)
    {
        *ext_alloc_len = alloc_len;
    }

    return alloc;
}

} /* namespace loc */
