/*
** $Id$
**
** shape-test.c: Unit test for shape of arabic support.
**
** Copyright (C) 2003 ~ 2007 Feynman Software
**
** All right reserved by Feynman Software.
**
** Current maintainer: Liu Peng.
**
** Create date: 2008/01/25
*/

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>

#define LETTERNUM    36

static Uint8 letter_tab [LETTERNUM] = {
    0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6,  
    0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 
    0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2,  
    0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8,  
    0xD9, 0xDA, 0xE1, 0xE2, 0xE3, 0xE4,  
    0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA};


#if 0
static Uint8 text_isolate [256] = {
    0xC1, 0xA0, 0xC2, 0xA0, 0xC3, 0xA0, 0xC4, 0xA0, 0xC5, 0xA0, 0xC6, 0xA0, 
    0xC7, 0xA0, 0xC8, 0xA0, 0xC9, 0xA0, 0xCA, 0xA0, 0xCB, 0xA0, 0xCC, 0xA0, 
    0xCD, 0xA0, 0xCE, 0xA0, 0xCF, 0xA0, 0xD0, 0xA0, 0xD1, 0xA0, 0xD2, 0xA0, 
    0xD3, 0xA0, 0xD4, 0xA0, 0xD5, 0xA0, 0xD6, 0xA0, 0xD7, 0xA0, 0xD8, 0xA0, 
    0xD9, 0xA0, 0xDA, 0xA0, 0xE1, 0xA0, 0xE2, 0xA0, 0xE3, 0xA0, 0xE4, 0xA0, 
    0xE5, 0xA0, 0xE6, 0xA0, 0xE7, 0xA0, 0xE8, 0xA0, 0xE9, 0xA0, 0xEA, 0x00};
#endif

#define TEXTMAXLEN    256

static Uint8 text [TEXTMAXLEN];

static int get_letter_index (Uint8 c)
{
    int i = 0;

    for (; i < LETTERNUM; i++) {
        if (c == letter_tab [i])
            return i;
    }

    return -1;
    
}

static int test_correctness ()
{
    int glyph_value = -1;
    int index = -1;
    Uint8* c = text;
   
    for (; *c>0; c++) {
        index = get_letter_index (*c);
        if (index < 0) /* not a letter.*/
            continue;

        glyph_value = iso8859_6_char_glyph_value (NULL, 0, c, 1);
        fprintf (stderr, "%x 's glyph value = %x\n", *c, glyph_value);

        if (!test_glyph_value (index, glyph_value)) {
            fprintf (stderr, "Shape error: %x\n", *c);
            return 0;
        }
    }
    
    return 1;
}

#define SINGLE    0x01
#define DOUBLE    0x02
#define TRIPLE    0x03 
#define QUAD      0x04

Uint8 get_random_letter ()
{
    int i = 0;

    i = (int)(((float)LETTERNUM)*rand()/(RAND_MAX+1.0));
    return letter_tab [i];
} 

int generate_text (int flag)
{
    int i = 0;
    Uint8* c = text;

    if (SINGLE == flag) {
        for (; i < LETTERNUM; i++) {
            *c++ = letter_tab [i];
            *c++ = 0xA0; 
        }
        *c = 0x00;
        return 1;
    }
    else if (DOUBLE == flag) {
        for (; i<(TEXTMAXLEN-1)/3; i++) { 
            *c++ = get_random_letter ();
            *c++ = get_random_letter (); 
            *c++ = 0xA0;
        }
        *c = 0x00;
        return 1;
    }
    else if (TRIPLE == flag) {
        for (; i<(TEXTMAXLEN-1)/4; i++) { 
            *c++ = get_random_letter ();
            *c++ = get_random_letter (); 
            *c++ = get_random_letter ();
            *c++ = 0xA0; 
        }
        *c = 0x00;
        return 1;
    }
    else if (QUAD == flag) {
        for (; i<(TEXTMAXLEN-1)/5; i++) { 
            *c++ = get_random_letter ();
            *c++ = get_random_letter (); 
            *c++ = get_random_letter ();
            *c++ = get_random_letter ();
            *c++ = 0xA0; 
        }
        *c = 0x00;
        return 1; 
    }

    return 0;
}

char *pgm_name;

/* Print helpful syntax message and exit. */
static void usage (void)
{
  static const char *help[] =
    {
      "shape-test, unit test for MiniGUI2.2 arabic shape .\n",
      "Usage: %s [text type]  ...\n",
      "text type is one of:\n",
      "       single   single letter text. \n",
      "       double   two-letter text. \n",
      "       triple   three-letter text. \n",
      "       quad     four-letter text. \n",
     NULL,
    };

  const char **p;
  for (p = help; *p != NULL; p++)
    printf (*p, pgm_name);

  exit (EXIT_SUCCESS);
}

static void parse_command_line (char** args, int* text_type)
{
    if (strcmp (args[1], "single") == 0)
        *text_type = SINGLE;
    else if (strcmp (args[1], "double") == 0)
        *text_type = DOUBLE;
    else if (strcmp (args[1], "triple") == 0)
        *text_type = TRIPLE;
    else if (strcmp (args[1], "quad") == 0)
        *text_type = QUAD;
    else 
        *text_type = -1;
}

int main (int argc, char *argv[])
{
    int type;

    pgm_name = argv[0];
    if (argc < 2 || argc > 2)
        usage ();

    parse_command_line (argv, &type);
    if (type < 0) {
        fprintf (stderr, "parse parameter error.\n");
        return;
    }

#if 0
    fprintf (stderr, "parameter = %d.\n", type);
#endif

    if (!generate_text (type)) {
        fprintf (stderr, "generate text error. \n");
        return;
    }
    
    if(!test_correctness (text)) {
        fprintf (stderr, "Shape failed.\n");
    }

    fprintf (stderr, "Shape suceed. \n");
}


