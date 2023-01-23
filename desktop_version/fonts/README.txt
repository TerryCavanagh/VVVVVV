=== I N T R O D U C T I O N ===

This file will explain the font format.

If you need a font (like a TTF) converted into the format that the game can read, for now you might want to ask Dav, who has tools for it.



=== F O N T   F O R M A T ===

Fonts consist of two files: a .png and a .fontmeta. The .png contains all the "images" for all glyphs, and the .fontmeta is an XML document containing all information about which characters are in the file and other metadata.

For example, a font for Japanese might be called font_ja.png and font_ja.fontmeta.

The fontmeta file looks like this:


<?xml version="1.0" encoding="UTF-8"?>
<font_metadata>
    <display_name>日本語</display_name>
    <width>12</width>
    <height>12</height>
    <white_teeth>1</white_teeth>
    <chars>
        <range start="0x20" end="0x7F"/>
        <range start="0xA0" end="0x17F"/>
        <range start="0x18F" end="0x18F"/>
        <range start="0x218" end="0x21B"/>
        <range start="0x259" end="0x25A"/>
        <!-- ... -->
    </chars>
    <special>
        <range start="0x00" end="0x1F" advance="6"/>
        <range start="0xEB00" end="0xEBFF" color="1"/>
    </special>
</font_metadata>


* display_name: the name of the language the font is specifically meant for - in the language itself. Users will see this in the level editor when choosing a font to use. If this font is used equally by multiple translations, this could be set to a combination like "繁體中文/한국어"

* width/height: the width and height of each glyph in the font. Every character will always be drawn as a rectangle of this size.

* white_teeth: indicates that all characters in the font are white, so the game itself doesn't have to remove all color from the image and make all pixels white like it would in old versions of the game. If this is not set to 1, this font cannot have colored (button) glyphs.

* chars: defines which characters are in the image. Starting at the top left of the image, each character is simply a rectangle of equal size (defined in <width> and <height>) from left to right, top to bottom. In the example given above, the image first has Unicode every character from U+0020 up to and including U+007F, then every character from U+00A0 to U+017F, and so on. To include a single character, simply use a range with equal start and end attributes equal.

* special: defines special attributes that will be applied to a range of characters. One or more of the following attributes can be used:

 - color: set to 1 if these glyphs should be drawn with its original colors (for button glyphs, or even emoji...)

 - advance: instead of <width>, the cursor (for drawing the next character) should be advanced this amount of pixels to the right. This controls the width of a character, but it does not affect how characters are arranged in the image, and the full glyph will still be drawn. While this means the font system has support for variable-width fonts, it's recommended to not use this option. There are some problems arising from using a variable-width font (especially in text boxes), so please consider copying the font's fullwidth forms (U+FF01-U+FF5E) to ASCII U+0021-U+007E instead. One may argue that a monospaced font also fits more with the game's style.

