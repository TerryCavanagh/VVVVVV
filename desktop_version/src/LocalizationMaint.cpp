#define LOCALIZATIONMAINT_CPP
#include "Localization.h"
#include "LocalizationStorage.h"

#include <tinyxml2.h>

#include "Alloc.h"
#include "FileSystemUtils.h"
#include "Font.h"
#include "Graphics.h"
#include "Script.h"
#include "Vlogging.h"
#include "XMLUtils.h"


namespace loc
{

static void write_max_local(tinyxml2::XMLElement* pElem, uint8_t glyph_w, uint8_t glyph_h)
{
    const char* max;
    if ((max = pElem->Attribute("max")) != NULL)
    {
        unsigned short max_w, max_h, max_local_w, max_local_h;
        if (parse_max(max, &max_w, &max_h))
        {
            max_local_w = (max_w*8) / glyph_w;
            max_local_h = (max_h*10) / SDL_max(10, glyph_h);

            if (max_local_h == 0)
            {
                max_local_h = 1;
            }

            char buf[16];
            if (max_h == 1)
            {
                SDL_snprintf(buf, sizeof(buf), "%d", max_local_w);
            }
            else
            {
                SDL_snprintf(buf, sizeof(buf), "%d*%d", max_local_w, max_local_h);
            }
            pElem->SetAttribute("max_local", buf);
        }
    }
}

static void write_max_local_decl(tinyxml2::XMLDocument* doc, uint8_t glyph_w, uint8_t glyph_h)
{
    char buf[16];
    SDL_snprintf(buf, sizeof(buf), "%dx%d", glyph_w, glyph_h);
    doc->FirstChildElement()->SetAttribute("max_local_for", buf);
}

static void sync_lang_file(const std::string& langcode)
{
    /* Update translation files for the given language with new strings from templates.
     * This basically takes the (English) templates, fills in existing translations, and saves.
     * Any FILESYSTEM_saveTiXml2Document() writes to main lang dir */
    vlog_info("Syncing %s with templates...", langcode.c_str());

    lang = langcode;
    loadtext(false);

    uint8_t glyph_w = 8, glyph_h = 8;
    font::glyph_dimensions(PR_FONT_IDX(langmeta.font_idx), &glyph_w, &glyph_h);
    bool max_local_needed = glyph_w != 8 || glyph_h != 8;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;
    tinyxml2::XMLElement* subElem;

    if (load_lang_doc("meta", doc, "en"))
    {
        FOR_EACH_XML_ELEMENT(hDoc, pElem)
        {
            const char* pKey = pElem->Value();

            if (SDL_strcmp(pKey, "active") == 0)
                pElem->SetText((int) langmeta.active);
            else if (SDL_strcmp(pKey, "nativename") == 0)
                pElem->SetText(langmeta.nativename.c_str());
            else if (SDL_strcmp(pKey, "credit") == 0)
                pElem->SetText(langmeta.credit.c_str());
            else if (SDL_strcmp(pKey, "action_hint") == 0)
                pElem->SetText(langmeta.action_hint.c_str());
            else if (SDL_strcmp(pKey, "gamepad_hint") == 0)
                pElem->SetText(langmeta.gamepad_hint.c_str());
            else if (SDL_strcmp(pKey, "autowordwrap") == 0)
                pElem->SetText((int) langmeta.autowordwrap);
            else if (SDL_strcmp(pKey, "toupper") == 0)
                pElem->SetText((int) langmeta.toupper);
            else if (SDL_strcmp(pKey, "toupper_i_dot") == 0)
                pElem->SetText((int) langmeta.toupper_i_dot);
            else if (SDL_strcmp(pKey, "toupper_lower_escape_char") == 0)
                pElem->SetText((int) langmeta.toupper_lower_escape_char);
            else if (SDL_strcmp(pKey, "menu_select") == 0)
                pElem->SetText(langmeta.menu_select.c_str());
            else if (SDL_strcmp(pKey, "menu_select_tight") == 0)
                pElem->SetText(langmeta.menu_select_tight.c_str());
            else if (SDL_strcmp(pKey, "font") == 0)
                pElem->SetText(font::get_main_font_name(langmeta.font_idx));
        }

        /* This part exists because we want to preserve blank lines between the commented
         * options for clarity, so we have to take matters into our own hands. */
        for (
            tinyxml2::XMLNode* pNode = hDoc.FirstChildElement().FirstChild().ToNode();
            pNode != NULL;
            pNode = pNode->NextSibling()
        )
        {
            tinyxml2::XMLComment* pCom = pNode->ToComment();
            if (pCom != NULL)
            {
                tinyxml2::XMLNode* pPrevNode = pCom->PreviousSibling();
                if (pPrevNode != NULL)
                {
                    doc.FirstChildElement()->InsertAfterChild(pPrevNode, doc.NewText("\n\n    "));
                }
                doc.FirstChildElement()->InsertAfterChild(pCom, doc.NewText("\n    "));
            }
        }

        FILESYSTEM_saveTiXml2Document((langcode + "/meta.xml").c_str(), doc);
    }

    if (load_lang_doc("strings", doc, "en"))
    {
        if (max_local_needed)
        {
            write_max_local_decl(&doc, glyph_w, glyph_h);
        }

        FOR_EACH_XML_ELEMENT(hDoc, pElem)
        {
            EXPECT_ELEM(pElem, "string");

            const char* eng = pElem->Attribute("english");
            if (eng != NULL)
            {
                char textcase = pElem->UnsignedAttribute("case", 0);
                const char* tra;
                if (textcase == 0)
                {
                    tra = map_lookup_text(map_translation, eng, "");
                }
                else
                {
                    char* eng_prefixed = add_disambiguator(textcase, eng, NULL);
                    if (eng_prefixed == NULL)
                    {
                        /* Are we out of memory? Stop, don't blank our language files... */
                        return;
                    }
                    /* Note the fallback: if this string used to not be cased and now it is,
                     * simply fill in the old single variant we already had. */
                    tra = map_lookup_text(
                        map_translation,
                        eng_prefixed,
                        map_lookup_text(map_translation, eng, "")
                    );
                    VVV_free(eng_prefixed);
                }

                pElem->SetAttribute("translation", tra);
            }

            if (max_local_needed)
            {
                write_max_local(pElem, glyph_w, glyph_h);
            }
        }

        FILESYSTEM_saveTiXml2Document((langcode + "/strings.xml").c_str(), doc);
    }

    if (load_lang_doc("strings_plural", doc, "en"))
    {
        /* Form 255 is technically invalid, but we have to account for it */
        bool form_id_used[256];
        SDL_zeroa(form_id_used);
        for (int num = 0; num < 200; num++)
        {
            form_id_used[number_plural_form[num]] = true;
        }

        if (max_local_needed)
        {
            write_max_local_decl(&doc, glyph_w, glyph_h);
        }

        FOR_EACH_XML_ELEMENT(hDoc, pElem)
        {
            EXPECT_ELEM(pElem, "string");

            if (max_local_needed)
            {
                write_max_local(pElem, glyph_w, glyph_h);
            }

            pElem->DeleteChildren();

            const char* eng_plural = pElem->Attribute("english_plural");

            for (int form_id = 0; form_id < 255; form_id++)
            {
                if (form_id_used[form_id] && eng_plural != NULL)
                {
                    subElem = doc.NewElement("translation");
                    pElem->LinkEndChild(subElem);

                    subElem->SetAttribute("form", form_id);

                    char* key = add_disambiguator(form_id+1, eng_plural, NULL);
                    if (key == NULL)
                    {
                        /* Out of memory or something, stop */
                        return;
                    }

                    subElem->SetAttribute("translation", map_lookup_text(map_translation_plural, key, ""));

                    VVV_free(key);
                }
            }
        }

        FILESYSTEM_saveTiXml2Document((langcode + "/strings_plural.xml").c_str(), doc);
    }

    if (load_lang_doc("cutscenes", doc, "en"))
    {
        FOR_EACH_XML_ELEMENT(hDoc, pElem)
        {
            EXPECT_ELEM(pElem, "cutscene");

            const char* cutscene_id = pElem->Attribute("id");
            if (cutscene_id == NULL)
            {
                continue;
            }

            hashmap* map = map_translation_cutscene;

            uintptr_t ptr_cutscene_map;
            bool found = hashmap_get(map, cutscene_id, SDL_strlen(cutscene_id), &ptr_cutscene_map);
            hashmap* cutscene_map = (hashmap*) ptr_cutscene_map;
            if (!found || cutscene_map == NULL)
            {
                continue;
            }

            FOR_EACH_XML_SUB_ELEMENT(pElem, subElem)
            {
                EXPECT_ELEM(subElem, "dialogue");

                const char* eng = subElem->Attribute("english");
                if (eng == NULL)
                {
                    continue;
                }

                size_t alloc_len;
                const std::string eng_unwrapped = font::string_unwordwrap(eng);
                char* eng_prefixed = add_disambiguator(subElem->UnsignedAttribute("case", 1), eng_unwrapped.c_str(), &alloc_len);
                if (eng_prefixed == NULL)
                {
                    /* Out of memory or something, stop */
                    return;
                }

                uintptr_t ptr_format;
                found = hashmap_get(cutscene_map, eng_prefixed, alloc_len-1, &ptr_format);
                const TextboxFormat* format = (TextboxFormat*) ptr_format;

                VVV_free(eng_prefixed);

                if (!found || format == NULL)
                {
                    continue;
                }

                subElem->DeleteAttribute("tt");
                subElem->DeleteAttribute("wraplimit");
                subElem->DeleteAttribute("centertext");
                subElem->DeleteAttribute("pad");
                subElem->DeleteAttribute("pad_left");
                subElem->DeleteAttribute("pad_right");
                subElem->DeleteAttribute("padtowidth");

                bool buttons = subElem->BoolAttribute("buttons", false);
                subElem->DeleteAttribute("buttons"); // we want this at the end...

                if (format->text != NULL)
                    subElem->SetAttribute("translation", format->text);
                if (format->tt)
                    subElem->SetAttribute("tt", 1);
                if (format->wraplimit_raw != 0)
                    subElem->SetAttribute("wraplimit", format->wraplimit_raw);
                if (format->centertext)
                    subElem->SetAttribute("centertext", 1);
                if (format->pad_left == format->pad_right && format->pad_left != 0)
                {
                    subElem->SetAttribute("pad", format->pad_left);
                }
                else
                {
                    if (format->pad_left != 0)
                        subElem->SetAttribute("pad_left", format->pad_left);
                    if (format->pad_right != 0)
                        subElem->SetAttribute("pad_right", format->pad_right);
                }
                if (format->padtowidth != 0)
                    subElem->SetAttribute("padtowidth", format->padtowidth);
                if (buttons)
                    subElem->SetAttribute("buttons", 1);
            }
        }

        FILESYSTEM_saveTiXml2Document((langcode + "/cutscenes.xml").c_str(), doc);
    }

    if (load_lang_doc("roomnames", doc, "en"))
    {
        FOR_EACH_XML_ELEMENT(hDoc, pElem)
        {
            EXPECT_ELEM(pElem, "roomname");

            pElem->SetAttribute("translation",
                get_roomname_translation(false, pElem->UnsignedAttribute("x"), pElem->UnsignedAttribute("y"))
            );
        }

        FILESYSTEM_saveTiXml2Document((langcode + "/roomnames.xml").c_str(), doc);
    }

    if (load_lang_doc("roomnames_special", doc, "en"))
    {
        FOR_EACH_XML_ELEMENT(hDoc, pElem)
        {
            EXPECT_ELEM(pElem, "roomname");

            const char* eng = pElem->Attribute("english");
            if (eng != NULL)
            {
                pElem->SetAttribute("translation", map_lookup_text(map_translation_roomnames_special, eng, ""));
            }
        }

        FILESYSTEM_saveTiXml2Document((langcode + "/roomnames_special.xml").c_str(), doc);
    }
}

bool sync_lang_files(void)
{
    /* Returns false if we can't set the lang write dir, true otherwise.
     * This could maybe be extended with better error reporting,
     * problem is getting across which files failed in which languages. */
    std::string oldlang = lang;
    if (!FILESYSTEM_setLangWriteDir())
    {
        vlog_error("Cannot set write dir to lang dir, not syncing language files");
        return false;
    }

    for (size_t i = 0; i < languagelist.size(); i++)
    {
        if (languagelist[i].code != "en")
            sync_lang_file(languagelist[i].code);
    }

    FILESYSTEM_restoreWriteDir();
    lang = oldlang;
    loadtext(false);

    return true;
}

bool save_roomname_to_file(const std::string& langcode, bool custom_level, int roomx, int roomy, const char* tra, const char* explanation)
{
    if (custom_level)
    {
        vlog_error("Saving custom level room names not implemented");
        return false;
    }

    if (!fix_room_coords(custom_level, &roomx, &roomy))
    {
        return false;
    }

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;

    if (!load_lang_doc("roomnames", doc, langcode))
    {
        return false;
    }

    bool found = false;
    FOR_EACH_XML_ELEMENT(hDoc, pElem)
    {
        EXPECT_ELEM(pElem, "roomname");

        int x = pElem->IntAttribute("x", -1);
        int y = pElem->IntAttribute("y", -1);

        if (x == roomx && y == roomy)
        {
            if (explanation != NULL)
            {
                pElem->SetAttribute("explanation", explanation);
            }
            if (tra != NULL)
            {
                pElem->SetAttribute("translation", tra);
            }
            found = true;
        }
    }

    if (!found)
    {
        vlog_error("Could not find room %d,%d in language file to replace!", roomx, roomy);
        return false;
    }

    if (!FILESYSTEM_setLangWriteDir())
    {
        vlog_error("Cannot set write dir to lang dir, so room name can't be saved");
        return false;
    }
    bool save_success = FILESYSTEM_saveTiXml2Document((langcode + "/roomnames.xml").c_str(), doc);
    FILESYSTEM_restoreWriteDir();
    if (!save_success)
    {
        vlog_error("Could not write roomnames document!");
        return false;
    }
    return store_roomname_translation(custom_level, roomx, roomy, tra, explanation);
}

bool save_roomname_explanation_to_files(bool custom_level, int roomx, int roomy, const char* explanation)
{
    bool success = true;
    for (size_t i = 0; i < languagelist.size(); i++)
    {
        if (!save_roomname_to_file(languagelist[i].code, custom_level, roomx, roomy, NULL, explanation))
        {
            success = false;
            vlog_warn("Could not save room name explanation to language %s", languagelist[i].code.c_str());
        }
    }

    return !languagelist.empty() && success;
}

void local_limits_check(void)
{
    text_overflows.clear();
    loadtext(true);
    limitscheck_current_overflow = 0;
}

void global_limits_check(void)
{
    text_overflows.clear();

    std::string oldlang = lang;

    textbook_clear(&textbook_main);
    textbook_set_protected(&textbook_main, true);

    for (size_t i = 0; i < languagelist.size(); i++)
    {
        if (languagelist[i].code != "en")
        {
            lang = languagelist[i].code;
            loadtext(true);
        }
    }

    lang = oldlang;
    loadtext(false);

    textbook_set_protected(&textbook_main, false);

    limitscheck_current_overflow = 0;
}

void populate_testable_script_ids(void)
{
    testable_script_ids.clear();

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;

    if (!load_lang_doc("cutscenes", doc))
    {
        return;
    }

    FOR_EACH_XML_ELEMENT(hDoc, pElem)
    {
        EXPECT_ELEM(pElem, "cutscene");

        const char* id = pElem->Attribute("id");
        if (id != NULL)
        {
            testable_script_ids.push_back(id);
        }
    }
}

bool populate_cutscene_test(const char* script_id)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;

    if (!load_lang_doc("cutscenes", doc))
    {
        return false;
    }

    const char* original = get_level_original_lang(hDoc);

    FOR_EACH_XML_ELEMENT(hDoc, pElem)
    {
        EXPECT_ELEM(pElem, "cutscene");

        if (SDL_strcmp(pElem->Attribute("id"), script_id) != 0)
        {
            /* Not the correct cutscene */
            continue;
        }

        tinyxml2::XMLElement* subElem;
        FOR_EACH_XML_SUB_ELEMENT(pElem, subElem)
        {
            EXPECT_ELEM(subElem, "dialogue");

            const char* tra = subElem->Attribute("translation");
            const char* speaker = subElem->Attribute("speaker");
            const char* eng = subElem->Attribute(original);
            if (tra != NULL && tra[0] != '\0' && speaker != NULL && eng != NULL)
            {
                script.add_test_line(
                    speaker,
                    eng,
                    subElem->UnsignedAttribute("case", 1),
                    subElem->BoolAttribute("buttons", false)
                );
            }
        }

        return true;
    }

    return false;
}

} /* namespace loc */
