/*
 * Usage:
 *  1) Define your VBFINFO name as extern_vbfinfo in a C file.
 *  2) Please link this file with the C file defining your VBF file to compile an executable.
 *  3) Run the executable.
 *
 * Limitation: 
 *  Only VBF V1.0 is supported.
 */

#include <stdio.h>
#include <string.h>

#include "vbf.h"

#define LEN_VERSION_INFO    10

#define VBF_VERSION         "vbf-1.0**"

typedef int BOOL;
#define TRUE    1
#define FALSE   0

#define LEN_FONT_NAME               23
#define LEN_DEVFONT_NAME            79

extern VBFINFO extern_vbfinfo;

#define vbf_to_dump extern_vbfinfo

BOOL dumpVBF (VBFINFO* vbf, char* file)
{
    FILE* fp;
    char family [LEN_FONT_NAME + 1];
    char style [LEN_FONT_NAME + 1];
    int len_offsets;
    int len_widths;
    int len_header;
    int len_bits = vbf->len_bits;
    int len_file;
    int num_chars = (vbf->last_char - vbf->first_char + 1);

    len_header = 4 + 2 + 8 + 3 + 12 + 4 + LEN_VERSION_INFO;
    len_offsets = num_chars * sizeof (unsigned short);
    len_widths = num_chars * sizeof (unsigned char);

    len_file = len_header + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths + len_bits;
    sprintf (file, "%s.vbf", vbf->name);

    if ((fp = fopen (file, "w+")) == NULL)
        return FALSE;

    fwrite (VBF_VERSION, sizeof (char), LEN_VERSION_INFO, fp);
    fwrite (&len_header, sizeof (int), 1, fp);
    fwrite (&vbf->max_width, sizeof (char), 1, fp);
    fwrite (&vbf->ave_width, sizeof (char), 1, fp);
    fwrite (&vbf->height, sizeof (int), 1, fp);
    fwrite (&vbf->descent, sizeof (int), 1, fp);
    fwrite (&vbf->first_char, sizeof (unsigned char), 1, fp);
    fwrite (&vbf->last_char, sizeof (unsigned char), 1, fp);
    fwrite (&vbf->def_char, sizeof (unsigned char), 1, fp);

    fwrite (&len_offsets, sizeof (int), 1, fp);
    fwrite (&len_widths, sizeof (int), 1, fp);
    fwrite (&len_bits, sizeof (int), 1, fp);
    fwrite (&len_file, sizeof (int), 1, fp);

    fwrite (vbf->name, sizeof (char), LEN_DEVFONT_NAME + 1, fp);
    fwrite (vbf->offset, sizeof (unsigned short), num_chars, fp);
    fwrite (vbf->width, sizeof (unsigned char), num_chars, fp);

    fwrite (vbf->bits, sizeof (unsigned char), len_bits, fp);

    fclose (fp);
    return TRUE;
}

int main (void)
{
    char file [256];

    dumpVBF (&vbf_to_dump, file);
    printf ("The vbf font name: '%s'\n", vbf_to_dump.name);
    printf ("The VBF dump to file: '%s'\n", file);
    return 0;
}

