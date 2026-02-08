/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --bpp 1 --size 16 --font C:/Users/viore/SquareLine/assets/Segoe-UI-Black.ttf -o C:/Users/viore/SquareLine/assets\ui_font_segblack16.c --format lvgl -r 0x20-0x7f --no-compress --no-prefilter
 ******************************************************************************/

#include "ui.h"

#ifndef UI_FONT_SEGBLACK16
#define UI_FONT_SEGBLACK16 1
#endif

#if UI_FONT_SEGBLACK16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xff, 0xf8, 0xff, 0x80,

    /* U+0022 "\"" */
    0xee, 0xdd, 0xb8,

    /* U+0023 "#" */
    0x19, 0x8d, 0x9f, 0xef, 0xf3, 0x31, 0x99, 0xfd,
    0xfe, 0x66, 0x33, 0x0,

    /* U+0024 "$" */
    0x18, 0x7d, 0xff, 0xfe, 0x3e, 0x1f, 0x1f, 0xf,
    0x1f, 0xff, 0xe1, 0x83, 0x0,

    /* U+0025 "%" */
    0x78, 0xe7, 0xe7, 0x33, 0x71, 0x9f, 0xf, 0xf8,
    0x3f, 0xf8, 0x3f, 0xe1, 0xf3, 0x1d, 0x99, 0xcf,
    0xce, 0x3c,

    /* U+0026 "&" */
    0x1f, 0x7, 0xf8, 0x73, 0x87, 0xb8, 0x3f, 0x3,
    0xee, 0x6e, 0xee, 0x7c, 0xe3, 0xcf, 0xfe, 0x3e,
    0xe0,

    /* U+0027 "'" */
    0xed, 0x80,

    /* U+0028 "(" */
    0x3b, 0x9d, 0xce, 0x73, 0x9c, 0xe7, 0x1c, 0xe3,
    0x80,

    /* U+0029 ")" */
    0xe3, 0x1c, 0x63, 0x9c, 0xe7, 0x39, 0xdc, 0xee,
    0x0,

    /* U+002A "*" */
    0x18, 0xb5, 0xf8, 0xc7, 0xc4, 0x80,

    /* U+002B "+" */
    0x18, 0x18, 0xff, 0xff, 0x18, 0x18, 0x18,

    /* U+002C "," */
    0x77, 0x6e,

    /* U+002D "-" */
    0xff,

    /* U+002E "." */
    0xff, 0x80,

    /* U+002F "/" */
    0x3, 0x83, 0x81, 0xc1, 0xc0, 0xe0, 0x70, 0x70,
    0x38, 0x38, 0x1c, 0x1c, 0xe, 0xe, 0x0,

    /* U+0030 "0" */
    0x3c, 0x7e, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7,
    0xe7, 0x7e, 0x3c,

    /* U+0031 "1" */
    0x1d, 0xff, 0xff, 0x1c, 0x71, 0xc7, 0x1c, 0x71,
    0xc0,

    /* U+0032 "2" */
    0x7c, 0x7e, 0x47, 0x7, 0xf, 0x1f, 0x3e, 0x78,
    0xf0, 0xff, 0xff,

    /* U+0033 "3" */
    0xfd, 0xfe, 0x38, 0x77, 0xcf, 0x83, 0x87, 0x8f,
    0xfb, 0xe0,

    /* U+0034 "4" */
    0xf, 0x7, 0x87, 0xc3, 0xe3, 0xf3, 0xbb, 0x9d,
    0xff, 0xff, 0x83, 0x81, 0xc0,

    /* U+0035 "5" */
    0x7f, 0x7f, 0x70, 0x70, 0x7e, 0x7f, 0x7, 0x7,
    0x87, 0xfe, 0xfc,

    /* U+0036 "6" */
    0x1e, 0x3e, 0x70, 0xe0, 0xfe, 0xff, 0xe7, 0xe7,
    0xe7, 0x7e, 0x3c,

    /* U+0037 "7" */
    0xff, 0xff, 0xe, 0xe, 0x1e, 0x1c, 0x3c, 0x38,
    0x38, 0x38, 0x78,

    /* U+0038 "8" */
    0x3c, 0xff, 0xe7, 0xe7, 0x7e, 0x7e, 0xe7, 0xe7,
    0xe7, 0x7e, 0x3c,

    /* U+0039 "9" */
    0x3c, 0x7e, 0xe7, 0xe7, 0xe7, 0xff, 0x7f, 0x7,
    0xe, 0x7e, 0x78,

    /* U+003A ":" */
    0xff, 0x81, 0xff,

    /* U+003B ";" */
    0xff, 0x80, 0x3e, 0xd8,

    /* U+003C "<" */
    0x3, 0x1f, 0x7c, 0xe0, 0xf8, 0x3f, 0x7, 0x1,

    /* U+003D "=" */
    0xff, 0xff, 0x0, 0xff, 0xff,

    /* U+003E ">" */
    0xc0, 0xf8, 0x3f, 0x7, 0x1f, 0xfc, 0xe0, 0x0,

    /* U+003F "?" */
    0xfd, 0xfe, 0x78, 0xf3, 0xcf, 0x1c, 0x0, 0x70,
    0xe1, 0xc0,

    /* U+0040 "@" */
    0x7, 0xe0, 0x7f, 0xc3, 0xc7, 0x9d, 0xf7, 0xff,
    0xdf, 0xf3, 0x7f, 0xcd, 0xff, 0x77, 0xff, 0xfb,
    0xfb, 0xc7, 0x84, 0xf, 0xf0, 0xf, 0x80,

    /* U+0041 "A" */
    0xf, 0x1, 0xf8, 0x1f, 0x81, 0xf8, 0x3f, 0xc3,
    0x9c, 0x39, 0xc7, 0xfe, 0x7f, 0xe7, 0xe, 0xf0,
    0xf0,

    /* U+0042 "B" */
    0xff, 0x7f, 0xf8, 0xfc, 0x7f, 0xe7, 0xfb, 0x8f,
    0xc7, 0xe3, 0xff, 0xbf, 0x80,

    /* U+0043 "C" */
    0x1f, 0x9f, 0xdf, 0xfe, 0xe, 0x7, 0x3, 0x81,
    0xe0, 0x7f, 0xbf, 0xc7, 0xe0,

    /* U+0044 "D" */
    0xfe, 0x3f, 0xee, 0x3b, 0x87, 0xe1, 0xf8, 0x7e,
    0x1f, 0x87, 0xe3, 0xbf, 0xcf, 0xe0,

    /* U+0045 "E" */
    0xff, 0xff, 0x87, 0xe, 0x1f, 0xff, 0xf0, 0xe1,
    0xff, 0xf8,

    /* U+0046 "F" */
    0xff, 0xff, 0x87, 0xe, 0x1f, 0xff, 0xf0, 0xe1,
    0xc3, 0x80,

    /* U+0047 "G" */
    0x1f, 0x8f, 0xe7, 0x3, 0x80, 0xe7, 0xf9, 0xfe,
    0x1f, 0x87, 0x71, 0xdf, 0xf1, 0xfc,

    /* U+0048 "H" */
    0xe1, 0xf8, 0x7e, 0x1f, 0x87, 0xff, 0xff, 0xff,
    0xff, 0x87, 0xe1, 0xf8, 0x7e, 0x1c,

    /* U+0049 "I" */
    0xff, 0xff, 0xff, 0xff, 0x80,

    /* U+004A "J" */
    0xe, 0x1c, 0x38, 0x70, 0xe1, 0xc3, 0xc7, 0xff,
    0xfb, 0xe0,

    /* U+004B "K" */
    0xe7, 0xb9, 0xee, 0xf3, 0xb8, 0xfe, 0x3f, 0x8f,
    0xe3, 0xbc, 0xef, 0x39, 0xee, 0x3c,

    /* U+004C "L" */
    0xe1, 0xc3, 0x87, 0xe, 0x1c, 0x38, 0x70, 0xe1,
    0xff, 0xf8,

    /* U+004D "M" */
    0xf8, 0xff, 0xc7, 0xff, 0x3f, 0xf9, 0xff, 0xdf,
    0xf6, 0xdf, 0xbe, 0xfd, 0xf7, 0xef, 0xbf, 0x39,
    0xf9, 0xce,

    /* U+004E "N" */
    0xf0, 0xfe, 0x1f, 0xe3, 0xfe, 0x7f, 0xef, 0xdf,
    0xfb, 0xff, 0x3f, 0xe3, 0xfc, 0x3f, 0x83, 0x80,

    /* U+004F "O" */
    0x1f, 0x7, 0xf9, 0xff, 0x78, 0xfe, 0xf, 0xc1,
    0xf8, 0x3f, 0x8f, 0x7f, 0xc7, 0xf0, 0x7c, 0x0,

    /* U+0050 "P" */
    0xfe, 0x7f, 0xf8, 0xfc, 0x7e, 0x3f, 0xfb, 0xf9,
    0xc0, 0xe0, 0x70, 0x38, 0x0,

    /* U+0051 "Q" */
    0x1f, 0x3, 0xfc, 0x7f, 0xcf, 0x1e, 0xe0, 0xee,
    0xe, 0xe0, 0xef, 0x1e, 0x7f, 0xc3, 0xfc, 0x1f,
    0xe0, 0xe, 0x0, 0x70,

    /* U+0052 "R" */
    0xfe, 0x3f, 0xee, 0x3b, 0x8e, 0xe7, 0xbf, 0xcf,
    0xf3, 0xbc, 0xe7, 0xb8, 0xee, 0x38,

    /* U+0053 "S" */
    0x3e, 0x7e, 0xe0, 0xf0, 0xfc, 0x7e, 0x3f, 0xf,
    0x8f, 0xfe, 0xfc,

    /* U+0054 "T" */
    0xff, 0xff, 0xf1, 0xc0, 0x70, 0x1c, 0x7, 0x1,
    0xc0, 0x70, 0x1c, 0x7, 0x1, 0xc0,

    /* U+0055 "U" */
    0xe1, 0xf8, 0x7e, 0x1f, 0x87, 0xe1, 0xf8, 0x7e,
    0x1f, 0xcf, 0xff, 0xdf, 0xe3, 0xf0,

    /* U+0056 "V" */
    0xf1, 0xee, 0x3d, 0xc7, 0x3c, 0xe3, 0xbc, 0x77,
    0xe, 0xe0, 0xfc, 0x1f, 0x3, 0xe0, 0x3c, 0x0,

    /* U+0057 "W" */
    0xf1, 0xe7, 0xb8, 0xf3, 0x9c, 0xf9, 0xce, 0x7c,
    0xe7, 0xb7, 0x73, 0xfb, 0xb0, 0xfd, 0xf8, 0x7e,
    0xfc, 0x3e, 0x3e, 0x1f, 0x1e, 0x7, 0x8f, 0x0,

    /* U+0058 "X" */
    0x79, 0xef, 0x38, 0xef, 0x1f, 0xc1, 0xf0, 0x3e,
    0x7, 0xe1, 0xfc, 0x3b, 0xcf, 0x3b, 0xc7, 0x80,

    /* U+0059 "Y" */
    0xf3, 0xbc, 0xe7, 0x71, 0xdc, 0x3e, 0xf, 0x81,
    0xc0, 0x70, 0x1c, 0x7, 0x1, 0xc0,

    /* U+005A "Z" */
    0xff, 0xff, 0xc3, 0xe1, 0xe1, 0xe1, 0xe0, 0xf0,
    0xf0, 0xf0, 0x7f, 0xff, 0xe0,

    /* U+005B "[" */
    0xff, 0xee, 0xee, 0xee, 0xee, 0xef, 0xf0,

    /* U+005C "\\" */
    0xe0, 0x38, 0x1c, 0x7, 0x3, 0x80, 0xc0, 0x70,
    0x38, 0xe, 0x7, 0x1, 0xc0, 0xe0, 0x38,

    /* U+005D "]" */
    0xff, 0xce, 0x73, 0x9c, 0xe7, 0x39, 0xcf, 0xff,
    0x80,

    /* U+005E "^" */
    0xc, 0xe, 0x7, 0x86, 0xc3, 0x33, 0x99, 0x8e,

    /* U+005F "_" */
    0xfe,

    /* U+0060 "`" */
    0xe3, 0x8e,

    /* U+0061 "a" */
    0x7e, 0x7f, 0x7, 0x7f, 0xf7, 0xe7, 0xff, 0x77,

    /* U+0062 "b" */
    0xe0, 0x70, 0x38, 0x1d, 0xef, 0xff, 0xbf, 0x8f,
    0xc7, 0xe3, 0xff, 0xbf, 0x80,

    /* U+0063 "c" */
    0x3e, 0xff, 0xc7, 0xe, 0x1e, 0x1f, 0x9f,

    /* U+0064 "d" */
    0x3, 0x81, 0xc0, 0xe7, 0xf7, 0xff, 0x9f, 0x8f,
    0xc7, 0xe3, 0xbf, 0xce, 0xe0,

    /* U+0065 "e" */
    0x3c, 0x7e, 0xe7, 0xff, 0xff, 0xe0, 0x7e, 0x3e,

    /* U+0066 "f" */
    0x1e, 0x7c, 0xe7, 0xff, 0xe7, 0xe, 0x1c, 0x38,
    0x70, 0xe0,

    /* U+0067 "g" */
    0x3f, 0xbf, 0xfc, 0xfc, 0x7e, 0x3f, 0x1d, 0xfe,
    0x77, 0x3, 0xa3, 0x9f, 0xcf, 0x80,

    /* U+0068 "h" */
    0xe0, 0xe0, 0xe0, 0xee, 0xff, 0xe7, 0xe7, 0xe7,
    0xe7, 0xe7, 0xe7,

    /* U+0069 "i" */
    0xff, 0xff, 0xff, 0xff, 0x80,

    /* U+006A "j" */
    0x39, 0xce, 0x73, 0x9c, 0xe7, 0x39, 0xce, 0x73,
    0xfb, 0xc0,

    /* U+006B "k" */
    0xe0, 0x70, 0x38, 0x1d, 0xee, 0xf7, 0x73, 0xf1,
    0xfc, 0xee, 0x77, 0xb9, 0xc0,

    /* U+006C "l" */
    0xff, 0xff, 0xff, 0xff, 0x80,

    /* U+006D "m" */
    0xee, 0x77, 0xff, 0xfd, 0xcf, 0xce, 0x7e, 0x73,
    0xf3, 0x9f, 0x9c, 0xfc, 0xe7,

    /* U+006E "n" */
    0xee, 0xff, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7,

    /* U+006F "o" */
    0x3e, 0x3f, 0xb8, 0xfc, 0x7e, 0x3f, 0x1d, 0xfc,
    0x7c,

    /* U+0070 "p" */
    0xef, 0x7f, 0xbd, 0xfc, 0x7e, 0x3f, 0x1f, 0xfd,
    0xfc, 0xe0, 0x70, 0x38, 0x1c, 0x0,

    /* U+0071 "q" */
    0x3f, 0xbf, 0xfc, 0xfc, 0x7e, 0x3f, 0x1d, 0xfe,
    0x77, 0x3, 0x81, 0xc0, 0xe0, 0x70,

    /* U+0072 "r" */
    0xef, 0xff, 0x38, 0xe3, 0x8e, 0x38,

    /* U+0073 "s" */
    0x7d, 0xfb, 0x87, 0xc7, 0xc3, 0xbf, 0x7c,

    /* U+0074 "t" */
    0x71, 0xcf, 0xff, 0x71, 0xc7, 0x1c, 0x7c, 0xf0,

    /* U+0075 "u" */
    0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xff, 0xff, 0x77,

    /* U+0076 "v" */
    0xf3, 0xdc, 0xe7, 0x39, 0xee, 0x3f, 0xf, 0xc3,
    0xe0, 0x78,

    /* U+0077 "w" */
    0xf7, 0xbf, 0xbd, 0xdd, 0xee, 0xef, 0x67, 0xdf,
    0x1e, 0xf8, 0xf7, 0xc7, 0x9c,

    /* U+0078 "x" */
    0xf3, 0xde, 0xe3, 0xf0, 0xfc, 0x3f, 0xf, 0xc7,
    0x7b, 0xcf,

    /* U+0079 "y" */
    0x71, 0xdc, 0xe7, 0xb8, 0xee, 0x3b, 0x7, 0xc1,
    0xf0, 0x78, 0xe, 0x7, 0x87, 0xc1, 0xe0,

    /* U+007A "z" */
    0xff, 0xfc, 0x71, 0xe7, 0x8e, 0x3f, 0xff,

    /* U+007B "{" */
    0x3b, 0xdc, 0xe7, 0x3b, 0x9c, 0x73, 0x9c, 0xe7,
    0x9c,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xf8,

    /* U+007D "}" */
    0xe7, 0x9c, 0xe7, 0x38, 0xe7, 0x73, 0x9c, 0xef,
    0x70,

    /* U+007E "~" */
    0x73, 0x7f, 0xb3, 0x80
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 71, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 94, .box_w = 3, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 6, .adv_w = 149, .box_w = 7, .box_h = 3, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 9, .adv_w = 153, .box_w = 9, .box_h = 10, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 21, .adv_w = 153, .box_w = 7, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 34, .adv_w = 230, .box_w = 13, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 52, .adv_w = 189, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 69, .adv_w = 88, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 71, .adv_w = 108, .box_w = 5, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 80, .adv_w = 108, .box_w = 5, .box_h = 13, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 89, .adv_w = 124, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 95, .adv_w = 183, .box_w = 8, .box_h = 7, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 102, .adv_w = 82, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 104, .adv_w = 104, .box_w = 4, .box_h = 2, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 105, .adv_w = 82, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 107, .adv_w = 123, .box_w = 9, .box_h = 13, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 122, .adv_w = 153, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 133, .adv_w = 124, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 142, .adv_w = 153, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 153, .adv_w = 153, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 159, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 176, .adv_w = 153, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 187, .adv_w = 153, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 198, .adv_w = 153, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 209, .adv_w = 153, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 220, .adv_w = 153, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 231, .adv_w = 82, .box_w = 3, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 234, .adv_w = 82, .box_w = 3, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 238, .adv_w = 183, .box_w = 8, .box_h = 8, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 246, .adv_w = 183, .box_w = 8, .box_h = 5, .ofs_x = 2, .ofs_y = 2},
    {.bitmap_index = 251, .adv_w = 183, .box_w = 8, .box_h = 8, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 259, .adv_w = 108, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 269, .adv_w = 243, .box_w = 14, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 292, .adv_w = 192, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 309, .adv_w = 177, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 322, .adv_w = 161, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 335, .adv_w = 194, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 349, .adv_w = 141, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 359, .adv_w = 139, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 369, .adv_w = 186, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 383, .adv_w = 205, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 397, .adv_w = 89, .box_w = 3, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 402, .adv_w = 131, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 412, .adv_w = 177, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 426, .adv_w = 139, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 436, .adv_w = 253, .box_w = 13, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 454, .adv_w = 208, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 470, .adv_w = 196, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 486, .adv_w = 168, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 499, .adv_w = 196, .box_w = 12, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 519, .adv_w = 175, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 533, .adv_w = 146, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 544, .adv_w = 163, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 558, .adv_w = 190, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 572, .adv_w = 180, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 588, .adv_w = 270, .box_w = 17, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 612, .adv_w = 180, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 628, .adv_w = 166, .box_w = 10, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 642, .adv_w = 164, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 655, .adv_w = 108, .box_w = 4, .box_h = 13, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 662, .adv_w = 123, .box_w = 9, .box_h = 13, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 677, .adv_w = 108, .box_w = 5, .box_h = 13, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 686, .adv_w = 183, .box_w = 9, .box_h = 7, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 694, .adv_w = 106, .box_w = 7, .box_h = 1, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 695, .adv_w = 89, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 9},
    {.bitmap_index = 697, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 705, .adv_w = 164, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 718, .adv_w = 127, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 725, .adv_w = 164, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 738, .adv_w = 143, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 746, .adv_w = 110, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 756, .adv_w = 164, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 770, .adv_w = 158, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 781, .adv_w = 79, .box_w = 3, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 786, .adv_w = 79, .box_w = 5, .box_h = 15, .ofs_x = -1, .ofs_y = -4},
    {.bitmap_index = 796, .adv_w = 152, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 809, .adv_w = 79, .box_w = 3, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 814, .adv_w = 243, .box_w = 13, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 827, .adv_w = 160, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 835, .adv_w = 162, .box_w = 9, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 844, .adv_w = 164, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 858, .adv_w = 164, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 872, .adv_w = 112, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 878, .adv_w = 124, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 885, .adv_w = 107, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 893, .adv_w = 160, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 901, .adv_w = 151, .box_w = 10, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 911, .adv_w = 216, .box_w = 13, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 924, .adv_w = 159, .box_w = 10, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 934, .adv_w = 147, .box_w = 10, .box_h = 12, .ofs_x = -1, .ofs_y = -4},
    {.bitmap_index = 949, .adv_w = 130, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 956, .adv_w = 108, .box_w = 5, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 965, .adv_w = 102, .box_w = 3, .box_h = 15, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 971, .adv_w = 108, .box_w = 5, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 980, .adv_w = 183, .box_w = 9, .box_h = 3, .ofs_x = 2, .ofs_y = 3}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t ui_font_segblack16 = {
#else
lv_font_t ui_font_segblack16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 16,          /*The maximum line height required by the font*/
    .base_line = 4,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if UI_FONT_SEGBLACK16*/

