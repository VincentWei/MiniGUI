/*
 * gen-vop-tab.c - generate unicode-vop-table.inc
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"

#include "packtab.h"

#include "unicode-version.h"

#ifdef _MGCHARSET_UNICODE_FULL
#define UNIBIDI_UNICODE_CHARS   0x110000
#else
#define UNIBIDI_UNICODE_CHARS   0xFFFE
#endif

#define appname "gen-vop-tab"
#define outputname "unicode-vop-table.inc"

static void
die (const char *msg)
{
  fprintf (stderr, appname ": %s\n", msg);
  exit (1);
}

static void
die2 (const char *fmt, const char *p
)
{
  fprintf (stderr, appname ": ");
  fprintf (stderr, fmt, p);
  fprintf (stderr, "\n");
  exit (1);
}

static void
die3 (const char *fmt, unsigned long l, const char *p)
{
  fprintf (stderr, appname ": ");
  fprintf (stderr, fmt, l, p);
  fprintf (stderr, "\n");
  exit (1);
}

enum VOPLinearEnumOffsetOne {
    U,
    R,
    Tu,
    Tr,
    NUM_TYPES
};

struct {
  const char *name;
  int key;
} type_names[] = {
    { "U",  U },    // Upright, the same orientation as in the code charts
    { "R",  R },     // Rotated 90 degrees clockwise compared to the code charts
    { "Tu", Tu },         // Transformed typographically, with fallback to Upright
    { "Tr", Tr },         // Transformed typographically, with fallback to Rotated
};

#define type_names_count (sizeof (type_names) / sizeof (type_names[0]))

static const char *names[type_names_count];

static char get_type (const char *s)
{
    unsigned int i;

    for (i = 0; i < type_names_count; i++)
        if (!strcmp (s, type_names[i].name))
            return type_names[i].key;
    die2 ("VO property name `%s' not found", s);
    return 0;
}

#define table_name "Vo"
#define macro_name "UCHAR_GET_VOP"

static signed int table[UNIBIDI_UNICODE_CHARS];
static char buf[4000];
static char tp[sizeof (buf)];

static void init (void)
{
    register int i;

    for (i = 0; i < type_names_count; i++)
        names[i] = 0;
    for (i = type_names_count - 1; i >= 0; i--)
        names[type_names[i].key] = type_names[i].name;
}

static void clear_tab (void)
{
    register Uchar32 c;

    for (c = 0; c < UNIBIDI_UNICODE_CHARS; c++)
        table[c] = R;
}

#if 0
static void init_tab_unicode_data_txt (void)
{
  register Uchar32 c;

  clear_tab ();
}
#endif

static void read_unicode_data_txt (FILE *f)
{
#if 1
    fprintf (stderr, "Skipped; Vertical Orientation Properties do not include in UnicodeData.txt\n");
#else
    unsigned long c, l;

    init_tab_unicode_data_txt ();

    l = 0;
    while (fgets (buf, sizeof buf, f))
    {
        int i;
        const char *s = buf;

        l++;

        while (*s == ' ')
            s++;

        if (s[0] == '#' || s[0] == '\0' || s[0] == '\n')
            continue;

        i = sscanf (s, "%lx;%*[^;];%*[^;];%*[^;];%[^; ]", &c, tp);
        if (i != 2 || c >= UNIBIDI_UNICODE_CHARS)
#if 0
            die3 ("invalid input at line %ld: %s", l, s);
#else
        fprintf (stderr, "skip character larger than UNIBIDI_UNICODE_CHARS at line %ld: %s", l, s);
#endif

        /* FIXME: We don't handle First..Last parts of UnicodeData.txt,
         * but it works, since all those are LTR. */
        table[c] = get_type (tp);
    }
#endif
}

/*
#  - Certain ranges of unassigned code points default to U. These ranges
#      are mostly associated with CJK scripts and punctuation, or with
#      a small number of other scripts which are predominantly Upright.
#      The private use areas also default to U, because of their most
#      common use for CJK. In the following list of explicit code points
#      and ranges, all unassigned code points default to U:
#         Canadian Syllabics Extended:        U+18B0..U+18FF
#         Reserved Default_Ignorable_Code_Point:      U+2065
#         Number Forms:                       U+2150..U+218F
#         Control Pictures & OCR              U+2400..U+245F
#         Symbols:                            U+2BB8..U+2BFF
#         CJK-Related & Yi:                   U+2E80..U+A4CF
#         Hangul Jamo Extended-A:             U+A960..U+A97F
#         Hangul Syllables & Jamo Extended-B: U+AC00..U+D7FF
#         PUA & CJK Compatibility Ideographs: U+E000..U+FAFF
#         Vertical Forms:                     U+FE10..U+FE1F
#         Small Form Variants:                U+FE50..U+FE6F
#         Fullwidth Forms:                            U+FFE7
#         Specials:                           U+FFF0..U+FFF8
#         Siddham:                           U+11580..U+115FF
#         Zanabazar Square & Soyombo:        U+11A00..U+11AAF
#         Egyptian Hieroglyphs & Controls:   U+13000..U+1343F
#         Anatolian Hieroglyphs:             U+14400..U+1467F
#         Ideographic Symbols & Tangut:      U+16FE0..U+18AFF
#         Kana Extended-A & Small Kana Ext:  U+1B100..U+1B16F
#         Nushu:                             U+1B170..U+1B2FF
#         Musical Symbols:                   U+1D000..U+1D1FF
#         Mayan Numerals:                    U+1D2E0..U+1D2FF
#         Symbols & Rods:                    U+1D300..U+1D37F
#         Sutton SignWriting:                U+1D800..U+1DAAF
#         Game Symbols:                      U+1F000..U+1F0FF
#         Enclosed Symbols:                  U+1F100..U+1F2FF
#         Symbols:                           U+1F680..U+1F7FF
#         Symbols and Pictographs:           U+1F900..U+1F9FF
#         Chess Symbols & Pictographs Ext-A: U+1FA00..U+1FAFF
#         Plane 2:                           U+20000..U+2FFFD
#         Plane 3:                           U+30000..U+3FFFD
#         Plane 15 PUA:                      U+F0000..U+FFFFD
#         Plane 16 PUA:                     U+100000..U+10FFFD
#
#  - All other code points, assigned and unassigned, that are not listed
#      explicitly in the data section of this file are given the value R.
*/

static void
init_tab_vertical_orientation_property_txt (void)
{
    register Uchar32 c;

    clear_tab ();

    for (c = 0x18B0; c < 0x18FF; c++)
        table[c] = U;

    table[0x2065] = U;
    for (c = 0x2150; c < 0x218F; c++)
        table[c] = U;

    for (c = 0x2400; c < 0x245F; c++)
        table[c] = U;

    for (c = 0x2BB8; c < 0x2BFF; c++)
        table[c] = U;

    for (c = 0x2E80; c < 0xA4CF; c++)
        table[c] = U;

    for (c = 0xA960; c < 0xA97F; c++)
        table[c] = U;

    for (c = 0xAC00; c < 0xD7FF; c++)
        table[c] = U;

    for (c = 0xE000; c < 0xFAFF; c++)
        table[c] = U;

    for (c = 0xFE10; c < 0xFE1F; c++)
        table[c] = U;

    for (c = 0xFE50; c < 0xFE6F; c++)
        table[c] = U;

    table[0xFFE7] = U;
    for (c = 0xFFF0; c < 0xFFF8; c++)
        table[c] = U;

    for (c = 0x11580; c < 0x115FF; c++)
        table[c] = U;

    for (c = 0x11A00; c < 0x11AAF; c++)
        table[c] = U;

    for (c = 0x13000; c < 0x1343F; c++)
        table[c] = U;

    for (c = 0x14400; c < 0x1467F; c++)
        table[c] = U;

    for (c = 0x16FE0; c < 0x18AFF; c++)
        table[c] = U;

    for (c = 0x1B100; c < 0x1B16F; c++)
        table[c] = U;

    for (c = 0x1B170; c < 0x1B2FF; c++)
        table[c] = U;

    for (c = 0x1D000; c < 0x1D1FF; c++)
        table[c] = U;

    for (c = 0x1D2E0; c < 0x1D2FF; c++)
        table[c] = U;

    for (c = 0x1D300; c < 0x1D37F; c++)
        table[c] = U;

    for (c = 0x1D800; c < 0x1DAAF; c++)
        table[c] = U;

    for (c = 0x1F000; c < 0x1F0FF; c++)
        table[c] = U;

    for (c = 0x1F100; c < 0x1F2FF; c++)
        table[c] = U;

    for (c = 0x1F680; c < 0x1F7FF; c++)
        table[c] = U;

    for (c = 0x1F900; c < 0x1F9FF; c++)
        table[c] = U;

    for (c = 0x1FA00; c < 0x1FAFF; c++)
        table[c] = U;

    for (c = 0x20000; c < 0x2FFFD; c++)
        table[c] = U;

    for (c = 0x30000; c < 0x3FFFD; c++)
        table[c] = U;

    for (c = 0xF0000; c < 0xFFFFD; c++)
        table[c] = U;

    for (c = 0x100000; c < 0x10FFFD; c++)
        table[c] = U;
}

static void
read_vertical_orientation_property_txt (FILE *f)
{
    unsigned long c, c2, l;

    init_tab_vertical_orientation_property_txt ();

    l = 0;
    while (fgets (buf, sizeof buf, f))
    {
        int i;
        register char typ;
        const char *s = buf;

        l++;

        while (*s == ' ')
            s++;

        if (s[0] == '#' || s[0] == '\0' || s[0] == '\n')
            continue;

        i = sscanf (s, "%lx ; %[^; ]", &c, tp);
        if (i == 2)
            c2 = c;
        else
            i = sscanf (s, "%lx..%lx ; %[^; ]", &c, &c2, tp) - 1;

        if (i != 2 || c > c2 || c2 >= UNIBIDI_UNICODE_CHARS)
            die3 ("invalid input at line %ld: %s", l, s);

        typ = get_type (tp);
        for (; c <= c2; c++)
            table[c] = typ;
    }
}

static void
read_data (const char *data_file_type, const char *data_file_name)
{
    FILE *f;

    if (!(f = fopen (data_file_name, "rt")))
        die2 ("error: cannot open `%s' for reading", data_file_name);

    if (!strcmp (data_file_type, "UnicodeData.txt"))
        read_unicode_data_txt (f);
    else if (!strcmp (data_file_type, "VerticalOrientation.txt"))
        read_vertical_orientation_property_txt (f);
    else
        die2 ("error: unknown data-file type %s", data_file_type);

    fclose (f);
}

static void
gen_vo_type_tab (int max_depth, const char *data_file_type)
{
  printf ("/* " outputname "\n * generated by " appname "\n"
          " * from the file %s of Unicode version "
          UNIBIDI_UNICODE_VERSION ". */\n\n", data_file_type);

  printf ("#define PACKTAB_UINT8 Uint8\n"
          "#define PACKTAB_UINT16 Uint16\n"
          "#define PACKTAB_UINT32 Uint32\n\n");

  if (!pack_table
      (table, UNIBIDI_UNICODE_CHARS, 1, U, max_depth, 3, names,
       "Uint8", table_name, macro_name, stdout))
    die ("error: insufficient memory, decrease max_depth");

  printf ("#undef PACKTAB_UINT8\n");
  printf ("#undef PACKTAB_UINT16\n");
  printf ("#undef PACKTAB_UINT32\n\n");

  printf ("/* End of generated " outputname " */\n");
}

int
main (int argc, const char **argv)
{
  const char *data_file_type = "VerticalOrientation.txt";

  if (argc < 3)
    die2 ("usage:\n  " appname " max-depth /path/to/%s [junk...]",
          data_file_type);

  {
    int max_depth = atoi (argv[1]);
    const char *data_file_name = argv[2];

    if (max_depth < 2)
      die ("invalid depth");

    init ();
    read_data (data_file_type, data_file_name);
    gen_vo_type_tab (max_depth, data_file_type);
  }

  return 0;
}

