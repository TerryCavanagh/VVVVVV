=== I N T R O D U C T I O N ===

This file will explain what you need to know when maintaining translations of VVVVVV (like adding new strings and syncing them across languages).



=== A D D I N G   N E W   S T R I N G S ===

If you want to add some new text to the game, all you generally need to do to make it translatable is wrap loc::gettext() around your raw strings in the code (you may need to add an #include "Localization.h"), and add the same strings to the English language file. The new strings can be automatically synced from English to all other language files using the translator menu.

For example, "Game paused" can be made translatable by changing it to loc::gettext("Game paused"). Its entry in the English language file could look like this:

    <string english="Game paused" translation="" explanation="pause screen" max="40"/>

The max value indicates how many characters of space there is for the text, and is further described below. It looks like "40" for single-line text, and "38*5" for multi-line text. The max value may not be applicable or may be hard to define, so this attribute can be completely left out. For example, when it's a diagonally-placed menu option, or because the limit depends on the lengths of other strings (like Low/Medium/High in the joystick menu), or a string looks progressively worse the longer it gets. As a general rule: if defining a hard limit would be misleading, then it can be exempt from having a limit.



=== T E X T   P R I N T I N G ===

These are the text printing functions:

 font::print(flags, x, y, text, r, g, b)
 font::print(flags, x, y, text, r, g, b, linespacing = -1, maxwidth = -1)

The flags argument can be 0, or a set of flags that do things like centering, enlarging, etc.

Some examples (can also be found in Font.h):

 Standard print
     font::print(0, 50, 50, "Hello world!", 255, 255, 255);

 Centered text
     font::print(PR_CEN, -1, 50, "Hello world!", 255, 255, 255);
     (set X to -1, unless you want to center *around* X)

 2x scale
     font::print(PR_2X, 50, 50, "V", 255, 255, 255);

 Centered 2x scale
     font::print(PR_CEN | PR_2X, -1, 50, "V", 255, 255, 255);

 Right-aligned 3x scale with a border around it
     font::print(PR_RIGHT | PR_3X | PR_BOR, 320, 50, "V", 255, 255, 255);

 Wordwrapped centered text
     font::print_wrap(PR_CEN, -1, 50, "Hello world, this will wordwrap to the screen width", 255, 255, 255);

A not-technically-exhaustive list of all flags (which are defined in Font.h):

- PR_2X/PR_3X/.../PR_8X  Print at larger scale (PR_1X is default)
- PR_FONT_INTERFACE      [DEFAULT] Use interface (VVVVVV language) font
- PR_FONT_LEVEL          Use level-specific font (room names, cutscenes, etc)
- PR_FONT_8X8            Use 8x8 font no matter what
- PR_BRIGHTNESS(value)   Use this brightness 0-255 (this value is mixed with
                         r, g and b for an alpha effect, and accounts for
                         colored glyphs correctly)
- PR_BOR                 Draw a black border around the text
- PR_LEFT                [DEFAULT] Left-align text/place at X coordinate
- PR_CEN                 Center-align text relative to X (X is center)
                         or to screen if X == -1
- PR_RIGHT               Right-align text to X
                         (X is now the right border, not left border)
- PR_CJK_CEN             [DEFAULT] Large fonts (Chinese/Japanese/Korean) should
                         stick out on top and bottom compared to 8x8 font
- PR_CJK_LOW             Larger fonts should stick out fully on the bottom
                         (draw at Y)
- PR_CJK_HIGH            Larger fonts should stick out fully on the top



=== S T R I N G   F O R M A T T I N G ===

Instead of sprintf-family functions, it is preferred to use VVVVVV's own formatting system, VFormat.

Strings sometimes have placeholders, which look like {name} or {name|flags}. For example, "{n_trinkets} of {max_trinkets}".

Placeholders can also have "flags" that modify their behavior. These can be added or removed in the translation as needed. Flags are separated by | (pipe).

For more info, see the documentation at the top of VFormat.h.

Full example:

 char buffer[100];
 vformat_buf(buffer, sizeof(buffer),
     "{crewmate} got {number} out of {total} trinkets in {m}:{s|digits=2}.{ms|digits=3}",
     "number:int, total:int, crewmate:str, m:int, s:int, ms:int",
     2, 20, "Vermilion", 2, 3, 1
 );

 => "Vermilion got 2 out of 20 trinkets in 2:03.001"



=== T R A N S L A T O R   M E N U ===

The translator menu has options for both translators and maintainers - it allows testing menus, translating room names within the game, syncing all language files with the English template files, getting statistics on translation progress, and more.

VVVVVV will show a "translator" menu in the main menu if either:
- The "lang" folder is NOT next to data.zip, and the game is running somewhere within a "desktop_version" folder, and desktop_version/lang IS found. This normally happens when compiling the game from source;
- The command line argument (or launch option) "-translator" is passed.
- ALWAYS_SHOW_TRANSLATOR_MENU is defined during compilation (see top of Localization.h)

To add new strings, add them to only the English strings.xml or strings_plural.xml, and use the option to sync all languages from the translator menu. This will copy the new strings to all translated language files.

The language file sync option has differing support for the language files. As indicated in the menu itself, it handles each file as follows:

[Full syncing EN→All]
For these files, the English version of the file is fully copied and overwrites every language's version, while all existing translations and customizations are inserted for every language. This means newly added strings are copied to every language, and removed strings are simultaneously removed from every language, bringing them fully up-to-date.
  - meta.xml
  - strings.xml
  - strings_plural.xml
  - cutscenes.xml
  - roomnames.xml
  - roomnames_special.xml

[Syncing not supported]
These files are untouched by the syncing feature.
  - numbers.xml



=== F I L E S ===

* meta.xml: This file contains some general information about a translation.

* strings.xml: This file contains general strings for the interface and some parts of the game.

* strings_plural.xml: Similar usage to strings.xml, but for strings that need different plural forms ("1 trinket", "2 trinkets")

* numbers.xml: This file contains all numbers from 0 to 100 written out (Zero, One, etc). This file also allows you to define the plural forms used in strings_plural.xml.

* cutscenes.xml: This file contains nearly all the cutscenes that appear in the main game.

* roomnames.xml: This file contains nearly all the room names for the main game.

* roomnames_special.xml: This file contains some special cases for roomnames, some names for rooms that usually aren't displayed as regular (like The Ship), and some general area names.
