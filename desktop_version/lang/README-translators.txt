=== I N T R O D U C T I O N ===

This file will explain everything you need to know when making translations of VVVVVV.

WARNING TO VOLUNTEERS: VVVVVV translation is not a community effort where anyone can submit translations! It's *possible* that, on a case-by-case basis, someone can volunteer to become an official translator. But you would need to get approval from Terry first (or you can express interest in there being an official translation into your language, it may already be planned!)

Likewise, you're welcome to report issues in existing translations (or to submit PRs to fix these issues), but it's not a good idea to rewrite significant parts of a translation, and then contribute it without warning. If you think there are errors or things that could be improved, please give an explanation as to why. (We may decide to discuss it with the official translator.)

It *is* possible to make a fan translation for fun, for sharing with others, etc. But it probably won't be distributed with the game officially.



=== A D D I N G   A   N E W   L A N G U A G E ===

The English language files are basically templates for other languages (all the translations are empty).

To create a new language, simply copy the `en` folder, and start by filling out meta.xml (further explained below).



=== E X C E L ===

The game uses XML files for storing the translations. If you prefer, there is an .xlsm file which can be used as an editor. This can load in all the XML files, and then save changes back as XML.

If you're an official translator, you should have received a version of this spreadsheet. If not, a blank version can be found here: https://github.com/Dav999-v/TranslationEditor



=== T R A N S L A T O R   M E N U ===

The translator menu has options for both translators and maintainers - it allows testing menus, translating room names within the game, syncing all language files with the English template files, getting statistics on translation progress, and more. The translator menu is hidden from players in regular versions of the game.

When the translator menu is unlocked, you can also press F12 anywhere in the game to reload the current language files. So you can save translations and immediately preview them (except for menu buttons and the current cutscene dialogue, which can't be reloaded on the fly). You will hear a coin sound when the language files have been reloaded via F12.



=== L O W E R C A S E   A N D   U P P E R C A S E ===

If lowercase and uppercase does not exist in your language (Chinese, Japanese and Korean for example), you can set toupper to 0 in meta.xml, and ignore any directions about using lowercase or uppercase.

VVVVVV's menu system has the style of using lowercase for unselected options and uppercase for selected options, for example:

  play
    levels
    [ OPTIONS ]
        translator
          credits
            quit

The menu options are stored as their full-lowercase version, and they're normally commented as "menu option" in the translation files. A built-in function (toupper in Localization.cpp) automatically converts the lowercase text to uppercase when needed. This function has support for a good number of accented characters, Cyrillic and Greek, but more could be added if needed. It also accounts for special cases in Turkish and Irish.

Turkish: The uppercase of i is İ, for example, "dil" becomes "DİL" and not "DIL". To enable this, set toupper_i_dot to 1 in meta.xml.

Irish: Specific letters may be kept in lowercase when making a string full-caps. For example, "mac tíre na hainnise" should be "MAC TÍRE NA hAINNISE" instead of "MAC TÍRE NA HAINNISE". If enabled, you can use the ~ character before the letter which should be forced in lowercase: "mac tíre na ~hainnise". This ~ character only has an effect in strings which are subject to automatic uppercasing (otherwise it'll be visible as å). This can be enabled by setting toupper_lower_escape_char to 1 in meta.xml.



=== W O R D W R A P P I N G   A N D   L E N G T H   L I M I T S ===

For most languages, VVVVVV can automatically wordwrap based on spaces. This may not work for some languages (like Chinese, Japanese and Korean), so instead, newlines can be inserted manually (see below) and automatic wordwrapping can be disabled in meta.xml.

VVVVVV's resolution is 320x240, and the default font is 8x8, which means there is a 40x30 character grid (although we don't adhere to this grid for the UI, but it gives a good indication). Naturally, if the font has a different size like 12x12, less characters will fit on the screen too.

Strings are usually annotated with their limits (for example, max="38*3"). This can be formatted like one of the following:
  (A) 33
  (B) 33*3

(A) if it's a single number (for example "33"): the hard maximum number of characters that are known to fit. Being exactly on the limit may not look good, so try to go at least a character under it if possible.

(B) if X*Y (for example 33*3): the text should fit within an area of X characters wide and Y lines high. The text is automatically word-wrapped to fit (unless disabled in meta.xml). If automatic word-wrapping is disabled, you need to manually insert newlines with |, or possibly as a literal newline.

If your language uses a font with a different size than 8x8, there will be two limits given: `max`, which is the original limit based on the 8x8 font, and `max_local`, which is adapted to the size of your font. To get this notation, use the maintenance option to sync language files from within VVVVVV. Ensure the correct font is set in meta.xml first.

The translator menu has an option ("limits check") to automatically find strings that break the given limits. There may be a few cases where this detection isn't perfect, but it should be a helpful quality assurance tool.

The maximum lengths are not always given. Notoriously, menu option buttons are placed diagonally, thus they have maximums that are hard to look up. Even more so, making an option differ too much in length from the other options might make it look out of place. Best thing to do there is probably just translate as usual and then test all menus via the "menu test" option in the translator menu. However, menus do automatically reposition based on the text length, so worst-case scenario, if an option is 36 characters long, all options are displayed right underneath each other.



=== F O N T S ===

The game uses an 8x8 pixel font by default (font.png and font.fontmeta in the "fonts" folder). If your language can be represented in 8x8 characters, it is preferable to use this font, or for this font to be extended.

The fonts directory also has a README.txt file that explains how the font format works.



=== N U M B E R S   A N D   P L U R A L   F O R M S ===

In certain places, VVVVVV (perhaps unconventionally) writes out numbers as full words. For example:

  - One out of Fifteen
  - Two crewmates remaining
  - Two remaining

These words can be found in numbers.xml. The numbers Zero through Twenty will be the most commonly seen. It's always possible for numbers up to One Hundred to be seen though (players can put up to 100 trinkets and crewmates in a custom level).

Your language may not allow the same word to be used for the same number in different scenarios. For example, in Polish, "twenty out of twenty" may be "dwadzieścia z dwudziestu". You can choose when these "wordy" numbers are used and when numeric forms (20 out of 20) are used (see "STRING FORMATTING" below). It's also possible to leave the translations for all the numbers empty. In that case, numeric forms will always be used.

In English, using Title Case is appropriate, but in most other languages, it probably isn't. Therefore, you may want to translate all numbers in lowercase, when it's more appropriate to use "twenty out of twenty" than "Twenty out of Twenty". You can then apply auto-uppercasing to any placeholder you choose (see "STRING FORMATTING" below), making it possible to display "Twenty out of twenty".

As for plural forms: English and some other languages have a singular (1 crewmate) and a plural (2 crewmates). Some languages may have different rules (like for 0, or numbers that end in 2, 3 and 4). VVVVVV can accommodate these rules and allows you to translate certain strings (strings_plural.xml) in different ways depending on the number. The different forms can be defined by changing the "form" attribute on each number in numbers.xml. For English, form "1" is used for singular, and form "0" is used for plural. You can set up any amount of plural forms you will need.

Numbers that identify the forms do not need to be sequential, you may use any number between 0 and 254 to identify the different forms. So instead of using forms 0, 1, 2 and 3, you could also name them 1, 2, 5 and 7.

Suppose you need a different form for the number 1, the numbers 2-4, and all other numbers. You could use "form 1" for the number 1, "form 2" for 2-4, and "form 0" for all other numbers:

<numbers>
    <number value="0"  form="0"  ... />
    <number value="1"  form="1"  ... />
    <number value="2"  form="2"  ... />
    <number value="3"  form="2"  ... />
    <number value="4"  form="2"  ... />
    <number value="5"  form="0"  ... />
    <number value="6"  form="0"  ... />
    ...

When translating the plural strings, you can add translations for every unique form. For example:

    <string english_plural="You rescued {n_crew} crewmates" english_singular="You rescued {n_crew} crewmate">
        <translation form="0" translation="You saved {n_crew} crewmates"/>
        <translation form="1" translation="You saved {n_crew} crewmate"/>
        <translation form="2" translation="You saved {n_crew} crewmateys"/>
    </string>

Plural forms can appear both for wordy numbers ("you saved one crewmate") as well as numbery numbers ("you died 136 times in this room"), so we need the plural forms to go further than 100.

For the numbers 100 and higher: as far as I can find (with information about plural rules across 160 languages) - the plural forms always repeat themselves every 100 numbers. So numbers 100-199 always have the same forms as 200-299, 300-399, and so on. However, 100-119 (200-219, etc) don't always work the same as 0-19 do (in English for example, it's not "101 trinket" despite ending in 01). Therefore, forms for 100-119 can also be filled in. The system will simply copy 20-99 for 120-199, and that should be enough to cover all numbers from 0 to infinity. Technically the system supports providing forms until 199, but it should never be necessary to go higher than 119, so they're not in the language files by default.

Numbers higher than 100 cannot have a written out translation ("one hundred and one" does not exist).



=== S T R I N G   F O R M A T T I N G ===

Strings sometimes have placeholders, which look like {name} or {name|flags}. For example, "{n_trinkets} of {max_trinkets}".

Placeholders can also have "flags" that modify their behavior. These can be added or removed in the translation as needed. Flags are separated by | (pipe).

For example, "{n_trinkets|wordy}" makes the number of trinkets display as a "wordy" number (twenty instead of 20) (See "NUMBERS AND PLURAL FORMS"). "{n_trinkets|wordy|upper}" makes that word start with a capital letter (Twenty instead of twenty). So for example, "{n_trinkets|wordy|upper} of {max_trinkets|wordy}" may be displayed as "Twenty out of twenty" - assuming numbers.xml is translated all-lowercase.

The valid flags are:
  - wordy         [ints only] use number words (Twenty) instead of digits (20)
  - digits=n      [ints only] force minimum n digits, like n=5 --> 00031
  - spaces        [only if using digits=n] use leading spaces instead of 0s
  - upper         uppercase the first character with loc::toupper_ch



=== S T O R Y   A N D   C H A R A C T E R   I N F O R M A T I O N ===

This is a brief story and character overview for reference. Any further questions can be directed to Terry: https://distractionware.com/email/

== The Crewmates ==

VVVVVV is about a crew of curious and super-intelligent aliens exploring the universe. There are six crewmates onboard the ship, in six different colours. They are:

* Viridian (Cyan, the player character)
* Verdigris (Green)
* Vitellary (Yellow)
* Vermilion (Red)
* Victoria (Blue)
* Violet (Purple)

All six characters have names that start with V. In addition, each crewmate's name is an obscure word that suggests their colour - for example, Verdigris is the name of the green pigment that forms on copper when it oxidises. This might be hard to translate! (So, potentially it just makes sense not to translate the names at all, unless you have a good idea about how to do it - for example, if you can pull off the same colour/name beginning with V trick in your language.)

Each crewmate has the following "rank", i.e. their job on the ship. These are all basically just Star Trek inspired roles:

* "Captain" Viridian (Captain as in leader)
* "Chief" Verdigris (Chief as in Lead Engineer)
* "Professor" Vitellary (Kind of the senior scientist on board)
* "Officer" Vermilion (Officer as in Away-Officer, the one who usually goes out exploring)
* "Doctor" Victoria (Doctor in the scientific sense)
* "Doctor" Violet (Doctor in the medical sense)

Verdigris, Vitellary and Vermilion are male. Victoria and Violet are female. Viridian is deliberately unspecified - when translating cutscenes, if at all possible, try to avoid specifying their gender, even if that leads to some otherwise awkward phrasing or requires the use of some cutting edge grammar tricks in your language.

== Personalities ==

If it helps with tone: the running joke in VVVVVV's writing is that all six characters are hyper intelligent prodigies, who will nevertheless speak to each other as if they're small children. E.g. Viridian "we were just playing with it!", Vermilion "I'm helping!", Verdigris "I'm an engineer!", etc. More specifically:

* Viridian is a classic hero - unphased by danger, never worried, always completely certain that they'll be able to fix everything.
* Verdigris is a romantic - he has a huge (reciprocated!) crush on Violet, which he does a terrible job of keeping secret.
* Vitellary is an academic - he loves science, and has a dry and long winded science thing to tell you in almost every cutscene.
* Vermilion is bold and adventurous - after you rescue him, you'll find him exploring different parts of the world on his own quest to find the rest of the crew (he's not much help, though).
* Victoria is a worrier - she's quick to feelings of despair. Victoria's sprite is almost always sad!
* Violet is a caretaker - she's the ship's Doctor, and most of her cutscenes are status updates about the other crewmates.

== Dimension VVVVVV ==

The world you're exploring is filled with terminals, with text logs from the previous inhabitants, who we never see. We don't know much about them.

The ship you're all on is called the "D.S.S. Souleye", which is a minor easter egg. D.S.S. just stands for "Dimensional Space Ship" - a craft that warps between different dimensions. Souleye is the pseudonym for Magnus Pålsson, the game's composer.



=== S P R I T E S   T R A N S L A T I O N ===

There are several enemies in the game which contain words: STOP, YES, OBEY, LIES and TRUTH. These, as well as the C on checkpoints, can be translated.

This may be a bit tricky - the sizes of the translated graphics should be as close to the original English as possible, which means that even though letters can be compressed a bit, they will quickly be too long. For example, if you'd like to translate LIES with 5 letters, it really doesn't help if one of those 5 letters is not an I (the most slim letter).

Fortunately, the translation does not have to be literal, as the words themselves are only referenced in room names at most, and the exact meanings aren't that important, only the spirit of them is. OBEY is a They Live reference, so there are a lot of other signs you could use. Some inspiration:

* STOP: this is on a stop sign, so may not need to be translated
* OBEY: maybe something like FOLLOW, or one of the other words in "They Live" (WORK, BUY, CONSUME, CONFORM, ...)
* LIES: maybe singular LIE, maybe some form of NONSENSE (as in BS but non-offensive), FALSE, FAKE, BLAH, FABLE
* TRUTH: maybe FACTS


== Implementation of translated sprites ==

If you'd like to pixel translated sprites yourself: Take sprites.png and flipsprites.png from the graphics folder in data.zip. These spritesheets are a 32x32 grid - for example, you cannot extend OBEY upwards or to the left.

The translated file does not have to contain any untranslated sprites - these can simply be made transparent, for optimization. So: the files should be transparent sheets with some translations in the middle.

The translated versions of sprites.png and flipsprites.png can be placed in a "graphics" subfolder of your language folder.

Then, put a file next to them called "spritesmask.xml", with the following contents:


<?xml version="1.0" encoding="UTF-8"?>
<spritesmask sprite_w="32" sprite_h="32">
    <sprite x="8" y="1" w="2"/> <!-- Checkpoints -->
    <sprite x="4" y="2" w="4"/> <!-- STOP -->
    <sprite x="4" y="3" w="4"/> <!-- YES -->
    <sprite x="3" y="4"/>       <!-- OBEY -->
    <sprite x="2" y="5" w="2"/> <!-- LIES receiver and LIES -->
    <sprite x="4" y="5" w="2"/> <!-- TRUTH -->
</spritesmask>


This file defines which sprites in the translated files should overwrite the original ones. Remove any lines for sprites you did not translate. After that, your translated sprites should work!

For completeness: sprite_w and sprite_h on the <spritesmask> tag define the size of each unit in the attributes of <sprite>. The possible attributes of <sprite> are:

* x, y: the position of the sprite to copy (in blocks of sprite_w by sprite_h)
* w, h: the width and height of the sprite to copy (default 1 for both)
* dx, dy: the destination position of the sprite (default x,y; so the same position as copied from)



=== F I L E S ===

== meta.xml ==
This file contains some general information about this translation. It contains the following attributes:

* active: If 0, this language will not be shown in the languages menu

* nativename: The name of the language in itself, fully in lowercase (so not "spanish" or "Español", but "español"). A language name can be at most 16 characters wide (in the 8x8 font)

* credit: You can fill in credit here that will appear on the language screen, like "Spanish translation by X". May be in your language. Max 38*2 @8x8

* action_hint: This is displayed at the bottom of the language screen when your language is highlighted, to show that Space/Z/V sets the selected option as the language. Max 40 @8x8

* autowordwrap: Whether automatic wordwrapping is enabled. Can be disabled for CJK (in which case newlines have to be inserted manually in text)

* toupper: Whether to enable automatic uppercasing of menu options (unselected, SELECTED). May be disabled for languages such as CJK that don't have lowercase and uppercase.

* toupper_i_dot: When automatically uppercasing, map i to İ, as in Turkish.

* toupper_lower_escape_char: When automatically uppercasing, allow ~ to be used to stop the next letter from being uppercased, for Irish.

* menu_select: The indication that a certain menu option or button is selected, in addition to the automatic uppercasing if "toupper" is enabled. For example, "[ {label} ]" looks like "[ SELECTED ]"

* menu_select_tight: Similar to menu_select, except used in cases where space is a bit more limited (like the map screen). "[{label}]" looks like "[SELECTED]"


== strings.xml ==
This file contains general strings for the interface and some parts of the game. In the XML, the tag for one string looks like this:

    <string english="Game paused" translation="" explanation="pause screen" max="40"/>

To translate a string, simply fill in the translation attribute, like this:

    <string english="Game paused" translation="Spel gepauzeerd" explanation="pause screen" max="40"/>

If the translation is left blank, the English text will be used as a fallback. Translations should NOT be left blank on purpose if the text is the same however; the string will be counted as untranslated and it'll be harder to keep track of what's new. Always just copy-paste the English string into the translation in that case.

The following attributes may be found for each string:

* english: the English text.

* translation: the translation.

* case: a number (1, 2, etc) for separate strings that are identical in English but may need different translations.

* explanation: an explanation about the context, location and possibly the formatting.

* max: length restrictions, described above in "WORDWRAPPING AND LENGTH LIMITS"


== strings_plural.xml ==

See "NUMBERS AND PLURAL FORMS" above.

You can define the plural forms in numbers.xml.

Then, simply add translations for each form you set up in numbers.xml. For example:

    <translation form="0" translation="Shows up for all numbers with form=0"/>
    <translation form="1" translation="Shows up for all numbers with form=1"/>
    <translation form="2" translation="Shows up for all numbers with form=2"/>

The "wordy" flag indicates a word will be filled in (like twelve), otherwise a number (12). As described above in "STRING FORMATTING", you can change this as needed in your translations.

The `var` attribute indicates which placeholder will be filled in, the `expect` attribute indicates how high the values are that you may expect to be filled in. For example, expect="20" means any value above 20 will probably not be used in this string. This is mainly needed so that the limits check knows not to worry about a number like "seventy seven" making the string too long, but it may also be a useful context clue.


== numbers.xml ==
This file contains all numbers from 0 to 100 written out (Zero, One, etc).

This will be filled in strings like:
- One out of Fifteen
- Two crewmates remaining
- Two remaining

If this can't work for your language, or wordy numbers are really unfitting, you can leave all of these empty, in which case numbers will be used (20 out of 20).

You may want to do it all-lowercase in order to not get English-style title casing. "Twenty out of Twenty" may be grammatically incorrect in MANY languages, and "twenty out of twenty" would be better. Translating the numbers all-lowercase allows you to apply context-specific uppercasing, like "Twenty out of twenty" (see "STRING FORMATTING" above)

This file also allows you to define the plural forms used in strings_plural.xml.

For more information, see "NUMBERS AND PLURAL FORMS" above.


== cutscenes.xml ==
This file contains nearly all the cutscenes that appear in the main game. Each line has a "speaker" attribute, which is not used by the game - it's just for translators to know who says what and to establish context.

The dialogues are automatically text-wrapped, except if automatic wrapping is disabled in meta.xml. In that case, the maximum line length is 36 8x8 characters (288 pixels) or 24 12x12 characters.

In the few cases where the same text appears multiple times in a cutscene, these have the attribute "case" added to them (for example case="1", case="2", etc), so they can be translated separately if needed. (These match up with textcase(x) commands in the scripts themselves)

You may find some additional formatting attributes on each <dialogue> tag. These are used to make spacing and formatting in translations consistent with the original English text (for example, centered text, padding on both sides, etc). You can change any of these if you need, and you can also add them yourself to ANY dialogue tag.

* tt: teletype. if "1", disable automatic word wrapping, even if autowordwrap is enabled in meta.xml. You will have to add newlines manually for this textbox, either with hard enters, or with |

* wraplimit: change the maximum width of the text before it wordwraps, in pixels. Only if tt is not enabled. Example:
    [Hello world!]  --[wraplimit="56"]-->  [Hello ]   (56=7*8)
                                           [world!]
  If autowordwrap is disabled in meta.xml, this also doesn't work, but it does give you an advisory maximum text width.
  The default is 288 (36*8 or 24*12).

* centertext: center the text (but keep it aligned to the grid), for example:
    [You have rescued]  --[centertext="1"]-->  [You have rescued]
    [a crewmember!]                            [ a crewmember!  ]

* pad: pad each line of text with a number of spaces (0 by default), for example:
    [You have rescued]  --[pad="2"]-->  [  You have rescued  ]
    [ a crewmember!  ]                  [   a crewmember!    ]
  This will automatically make the wrap limit smaller accordingly, unless a custom wraplimit is given.

* pad_left/pad_right: same as pad, but only affects the left or right side. For example:
    [You have rescued]  --[ pad_left="5"]--\  [     You have rescued  ]
    [ a crewmember!  ]  --[pad_right="2"]--/  [      a crewmember!    ]

* padtowidth: pad the text on both sides if it's not this many pixels wide. For example:
    [-= Personal Log =-]  --[padtowidth="224"]--> [     -= Personal Log =-     ]   (224=28*8)


== roomnames.xml ==
This file contains nearly all the room names for the main game. The limit is always 40 8x8 characters (320 pixels) or 26 12x12 characters.

It's recommended to translate the room names in-game to see why all rooms are called what they are. To do this, enable room name translation mode in translator > translator options > translate room names.


== roomnames_special.xml ==
This file contains some special cases for roomnames, some names for rooms that usually aren't displayed as regular (like The Ship), and some general area names.

One room ("Prize for the Reckless") is intentionally missing spikes in a time trial and no death mode so the player does not have to die there, and the room is called differently in both cases (for time trial "Imagine Spikes There, if You Like", and for no death mode "I Can't Believe You Got This Far").

There are also some roomnames in the game which gradually transform into others or cycle through a few minor variations.

