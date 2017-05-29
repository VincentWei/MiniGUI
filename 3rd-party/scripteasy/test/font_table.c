
/*
 * Copyright (C) 2000-2007 Beijing Komoxo Inc.
 * All rights reserved.
 */

#include "se_minigui.h"
#include "stdio.h"
#include "string.h"

#include "kmx_latin1_ttf.h"
#include "fzxh_gb2312_ttf.h"
#include "fzxh_gb18030_1_ttf_stub.h"
#include "fzxh_gb18030_2_ttf_stub.h"

#define ASSERT(a)

#ifdef SE_FONT_DYNAMIC_LOADING_SUPPORT
    #define MAX_PATH                256
    #define MAX_LOGICAL_FONT_SIZE   16  /* Physical font count per logical font */
#endif

#define MAX_LOGICAL_TABLE_SIZE  256 

/* Script Easy font description data definition */

static void* font_table[] =
{
    (void*) &kmx_latin1_ttf, 0,
    (void*) &fzxh_gb2312_ttf, 0,
    (void*) &fzxh_gb18030_1_ttf, 0,
    (void*) &fzxh_gb18030_2_ttf, 0,

    0
};

#define FONT_TABLE_SIZE ((sizeof(font_table) / sizeof(void*) - 1) / 2)

static unsigned char logical_font_table_config[][MAX_LOGICAL_FONT_SIZE] = 
{
    {4, 0, 1, 2, 3}
};
#define LOGICAL_FONT_TABLE_SIZE (sizeof(logical_font_table_config) / (MAX_LOGICAL_FONT_SIZE * sizeof(unsigned char)))
#define DEFAULT_LOGICAL_FONT_INDEX  0

static unsigned char logical_font_table[MAX_LOGICAL_TABLE_SIZE];

static unsigned int logical_font_table_index_map[LOGICAL_FONT_TABLE_SIZE];

#ifdef SE_FONT_DYNAMIC_LOADING_SUPPORT
    static const 
    struct {
        const char    *font_name;
        int           logical_font_index;
    }
    font_name_config[] = 
    {
        {0, 0},
        {0, 0},
       {"fzxh_gb18030_1_ttf.le", 0},
       {"fzxh_gb18030_2_ttf.le", 0},
    };
#endif /* #ifdef SE_FONT_DYNAMIC_LOADING_SUPPORT */

static const se_font_name_map font_name_map[] = {
    /* iso8859-1 */
    {"ttf-fzxh-rrncnn-0-0-iso8859-1", 0},
    /* GB2312 */
    {"ttf-fzxh-rrncnn-0-0-GB2312", 0},
    /* utf-8 */
    {"ttf-fzxh-rrncnn-0-0-utf-8", 0},
};

#define FONT_NAME_MAP_SIZE (sizeof(font_name_map) / sizeof(se_font_name_map))

const se_font_description se_font_desc = {
    FONT_TABLE_SIZE,
    FONT_NAME_MAP_SIZE,
    (const se_font_name_map*)font_name_map,
    (const void **)font_table,
    logical_font_table
};

/* The following is the scripteasy mem pool definition */

#define SE_MEM_POOL_SIZE	        (64 * 1024)
#define SE_CACHE_MEM_POOL_SIZE      (256 * 1024)

int   se_minigui_mem_pool[SE_MEM_POOL_SIZE];
int   se_minigui_mem_pool_size = SE_MEM_POOL_SIZE * sizeof(int);
int   se_minigui_cache_mem_pool[SE_CACHE_MEM_POOL_SIZE];
int   se_minigui_cache_mem_pool_size = SE_CACHE_MEM_POOL_SIZE * sizeof(int);


/* The Following is for initializing font */

#ifdef SE_FONT_DYNAMIC_LOADING_SUPPORT
    static const char * se_minigui_get_font_dir(void)
    {
        /* TO BE CUSTOMIZED */
        static char font_dir[MAX_PATH + 1] = "/usr/minigui/se_new/minigui/src/fonts/";
        
        return font_dir;
    }

    static FILE* se_minigui_open_font_file(const char *name)
    {
        static char     path[MAX_PATH + 1];
        const char      *font_dir;
        FILE            *handle = 0;

        font_dir = se_minigui_get_font_dir();

        if(!font_dir)
        {
            return 0;
        }

        strcpy(path, font_dir);
        strcat(path, name);

        handle = fopen(path, "rb");

        return handle;
    }

    unsigned int se_minigui_read_font(
        void*           name,
        void*           buffer,
        unsigned int    offset,
        unsigned int    size)
    {
        FILE*           handle = (FILE*) name;
        unsigned int    read = 0;
        int             result = 0;
    
        if(!handle)
            return 0;
    
        fseek(handle, offset, SEEK_SET);
        printf("In se_minigui_read_font...\n");
        return fread(buffer, 1, size, handle);
    }

#endif /* #ifdef SE_FONT_DYNAMIC_LOADING_SUPPORT */

static int append_logical_font(int pos, const unsigned char *logical_font)
{
    memcpy (logical_font_table + pos, logical_font, logical_font[0] + 1);
    return logical_font[0] + 1;
}

static void build_logical_font_table (void)
{
    unsigned int i = 0;
    int          pos = 0;
    for (; i < LOGICAL_FONT_TABLE_SIZE; ++i)
    {
        pos += append_logical_font (pos, logical_font_table_config[logical_font_table_index_map[i]]);
    }

    logical_font_table[pos] = (unsigned char) 0;
}

static void init_logical_font_map(void)
{
    unsigned int i = 0;

    for (; i < LOGICAL_FONT_TABLE_SIZE; ++i)
    {
        logical_font_table_index_map[i] = i;
    }
}

void se_minigui_destroy_fonts(void)
{
#ifdef SE_FONT_DYNAMIC_LOADING_SUPPORT
    int i = 0;
    for(; i < FONT_TABLE_SIZE; ++i)
    {
        if(font_table[i * 2 + 1])
        {
            fclose((FILE*) font_table[i * 2 + 1]);
            font_table[i * 2 + 1] = 0;
        }
    }
#endif
}

void se_minigui_init_fonts(void)
{
    int i = 0;
    int pos = 0;
    
    ASSERT (DEFAULT_LOGICAL_FONT_INDEX >= 0 && 
            DEFAULT_LOGICAL_FONT_INDEX < LOGICAL_FONT_TABLE_SIZE);

    se_minigui_destroy_fonts();
    
    init_logical_font_map();

#ifdef SE_FONT_DYNAMIC_LOADING_SUPPORT
    for(; i < FONT_TABLE_SIZE; ++i)
    {
        if(font_name_config[i].font_name)
        {
            FILE* handle;
            
            handle = se_minigui_open_font_file(font_name_config[i].font_name);
            
            ASSERT (font_table[i * 2 + 1] == 0);

            if (handle)
            {
                font_table[i * 2 + 1] = (void*)handle;
            }
            else
            {
                ASSERT (font_name_config[i].logical_font_index >= 0 && 
                        font_name_config[i].logical_font_index < LOGICAL_FONT_TABLE_SIZE);
                ASSERT (logical_font_table_index_map[font_name_config[i].logical_font_index] == font_name_config[i].logical_font_index);
                
                logical_font_table_index_map[font_name_config[i].logical_font_index] = DEFAULT_LOGICAL_FONT_INDEX;
            }
        }
    }
#endif

    build_logical_font_table();
}
