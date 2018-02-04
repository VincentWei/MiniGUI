/*
 *   This file is part of MiniGUI, a mature cross-platform windowing 
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 * 
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */

#ifndef SE_MINIGUI_H
#define SE_MINIGUI_H

#ifdef __cplusplus
extern "C"{
#endif

#define SE_FONT_DYNAMIC_LOADING_SUPPORT
#define SE_PALETTE_AUTO_ADJUST

/* Script Easy integration API with minigui */

#ifndef SE_EXPORT
    #define SE_EXPORT       extern
#endif

typedef int sestatus;
typedef unsigned short seunichar16;

/*{{ status_code_definition {{*/

#define SE_STATUS_OK                        0

#define SE_STATUS_VISIBLE                   1
#define SE_STATUS_INVISIBLE                 2
#define SE_STATUS_OUTSIDE                   3
#define SE_STATUS_NO_TEXT                   4
#define SE_STATUS_OUT_OF_START              5
#define SE_STATUS_OUT_OF_END                6

#define SE_STATUS_INVALID_PARAMETER         -1
#define SE_STATUS_INVALID_DEVICE_CONTEXT    -2
#define SE_STATUS_TEXT_ILL_FORMED           -3
#define SE_STATUS_MARKUP_ILL_FORMED         -4
#define SE_STATUS_TEXT_TOO_LONG             -5
#define SE_STATUS_NO_FONT                   -6
#define SE_STATUS_NOT_INITIALIZED           -7

/*}} status_code_definition }}*/

typedef unsigned int (*SeFontAccessFunc)(
    void*           name,
    void*           buffer,
    unsigned int    offset,
    unsigned int    size);

typedef struct
{
    int                 language;
    const void*         language_data;
    const void*         font_table;
    const void*         logical_font_table;
    SeFontAccessFunc    font_access_callback;
    void*               mem_pool;
    int                 mem_pool_size;
    void*               cache_mem_pool;
    int                 cache_mem_pool_size;
} SeInitInfo;

sestatus se_minigui_initialize(const SeInitInfo* init_info);
sestatus se_minigui_cleanup(void);
sestatus se_minigui_set_chip_param(void* chip_param);
sestatus se_minigui_set_cache(void* cache_mem_pool, int cache_mem_pool_size);

/*{{ SeLogFont_constants {{*/
#define SE_LOGFONT_BOLD             0x01
#define SE_LOGFONT_ITALIC           0x02
#define SE_LOGFONT_UNDERLINE        0x04
#define SE_LOGFONT_STRIKETHROUGH    0x08
#define SE_LOGFONT_PREFERRED        0x10
#define SE_LOGFONT_MONO             0x20
/*}} SeLogFont_constants }}*/

/*{{ SeLogFont_type {{*/
typedef struct
{
    unsigned short  size;
    unsigned char   attr;
    unsigned char   id;
} SeLogFont;
/*}} SeLogFont_type }}*/

int se_minigui_get_char_width (SeLogFont *logfont, seunichar16 wc);

int se_minigui_get_font_ascent (SeLogFont *logfont);

int se_minigui_get_font_descent (SeLogFont *logfont);

const void *se_minigui_get_char_pixmap(
    const SeLogFont         *logfont,
    const seunichar16       wc, 
    int                     *pitch);

int se_minigui_get_char_bbox(
    SeLogFont       *logfont, 
    seunichar16     wc,
    int             *px, 
    int             *py, 
    int             *pwidth,
    int             *pheight);

void se_minigui_get_char_advance(
    SeLogFont   *logfont,
    seunichar16 wc,
    int         *px, 
    int         *py);

/*{{ language_code_constants {{*/
#define SE_LANG_AA              0       /* Afar                            */
#define SE_LANG_AB              1       /* Abkhazia                        */
#define SE_LANG_AF              2       /* Afrikaans                       */
#define SE_LANG_AM              3       /* Amharic                         */
#define SE_LANG_AR              4       /* Arabic                          */
#define SE_LANG_AS              5       /* Assamese                        */
#define SE_LANG_AST             6       /* Asturian                        */
#define SE_LANG_AVA             7       /* Avaric                          */
#define SE_LANG_AY              8       /* Aymara                          */
#define SE_LANG_AZ              9       /* Azerbaijani                     */
#define SE_LANG_AZ_IR           10      /* Azerbaijani in Iran             */
#define SE_LANG_BA              11      /* Bashkir                         */
#define SE_LANG_BAM             12      /* Bambara                         */
#define SE_LANG_BE              13      /* Byelorussian                    */
#define SE_LANG_BG              14      /* Bulgarian                       */
#define SE_LANG_BH              15      /* Bihari (Devanagari script)      */
#define SE_LANG_BHO             16      /* Bhojpuri (Devanagari script)    */
#define SE_LANG_BI              17      /* Bislama                         */
#define SE_LANG_BIN             18      /* Edo or Bini                     */
#define SE_LANG_BN              19      /* Bengali (Bengali script)        */
#define SE_LANG_BO              20      /* Tibetan                         */
#define SE_LANG_BR              21      /* Breton                          */
#define SE_LANG_BS              22      /* Bosnian                         */
#define SE_LANG_BUA             23      /* Buriat (Buryat)                 */
#define SE_LANG_CA              24      /* Catalan                         */
#define SE_LANG_CE              25      /* Chechen                         */
#define SE_LANG_CH              26      /* Chamorro                        */
#define SE_LANG_CHM             27      /* Mari (Lower & Upper Cheremis)   */
#define SE_LANG_CHR             28      /* Cherokee                        */
#define SE_LANG_CO              29      /* Corsican                        */
#define SE_LANG_CS              30      /* Czech                           */
#define SE_LANG_CU              31      /* Old Church Slavonic             */
#define SE_LANG_CV              32      /* Chuvash                         */
#define SE_LANG_CY              33      /* Welsh                           */
#define SE_LANG_DA              34      /* Danish                          */
#define SE_LANG_DE              35      /* German                          */
#define SE_LANG_DZ              36      /* Dzongkha                        */
#define SE_LANG_EL              37      /* Greek                           */
#define SE_LANG_EN              38      /* English                         */
#define SE_LANG_EO              39      /* Esperanto                       */
#define SE_LANG_ES              40      /* Spanish                         */
#define SE_LANG_ET              41      /* Estonian                        */
#define SE_LANG_EU              42      /* Basque                          */
#define SE_LANG_FA              43      /* Persian                         */
#define SE_LANG_FI              44      /* Finnish                         */
#define SE_LANG_FJ              45      /* Fijian                          */
#define SE_LANG_FO              46      /* Faroese                         */
#define SE_LANG_FR              47      /* French                          */
#define SE_LANG_FUL             48      /* Fulah (Fula)                    */
#define SE_LANG_FUR             49      /* Friulian                        */
#define SE_LANG_FY              50      /* Frisian                         */
#define SE_LANG_GA              51      /* Irish                           */
#define SE_LANG_GD              52      /* Scots Gaelic                    */
#define SE_LANG_GEZ             53      /* Ethiopic (Geez)                 */
#define SE_LANG_GL              54      /* Galician                        */
#define SE_LANG_GN              55      /* Guarani                         */
#define SE_LANG_GU              56      /* Gujarati                        */
#define SE_LANG_GV              57      /* Manx Gaelic                     */
#define SE_LANG_HA              58      /* Hausa                           */
#define SE_LANG_HAW             59      /* Hawaiian                        */
#define SE_LANG_HE              60      /* Hebrew                          */
#define SE_LANG_HI              61      /* Hindi (Devanagari script)       */
#define SE_LANG_HO              62      /* Hiri Motu                       */
#define SE_LANG_HR              63      /* Croatian                        */
#define SE_LANG_HU              64      /* Hungarian                       */
#define SE_LANG_HY              65      /* Armenian                        */
#define SE_LANG_IA              66      /* Interlingua                     */
#define SE_LANG_IBO             67      /* Igbo (Ibo)                      */
#define SE_LANG_ID              68      /* Indonesian                      */
#define SE_LANG_IE              69      /* Interlingue                     */
#define SE_LANG_IK              70      /* Inupiaq (Inupiak, Eskimo)       */
#define SE_LANG_IO              71      /* Ido                             */
#define SE_LANG_IS              72      /* Icelandic                       */
#define SE_LANG_IT              73      /* Italian                         */
#define SE_LANG_IU              74      /* Inuktitut                       */
#define SE_LANG_JA              75      /* Japaness                        */
#define SE_LANG_KA              76      /* Georgian                        */
#define SE_LANG_KAA             77      /* Kara-Kalpak (Karakalpak)        */
#define SE_LANG_KI              78      /* Kikuyu                          */
#define SE_LANG_KK              79      /* Kazakh                          */
#define SE_LANG_KL              80      /* Greenlandic                     */
#define SE_LANG_KM              81      /* Khmer                           */
#define SE_LANG_KN              82      /* Kannada                         */
#define SE_LANG_KO              83      /* Korean                          */
#define SE_LANG_KOK             84      /* Kokani (Devanagari script)      */
#define SE_LANG_KS              85      /* Kashmiri (Devanagari script)    */
#define SE_LANG_KU              86      /* Kurdish                         */
#define SE_LANG_KU_IR           87      /* Kurdish in Iran                 */
#define SE_LANG_KUM             88      /* Kumyk                           */
#define SE_LANG_KV              89      /* Komi (Komi-Permyak/Komi-Siryan) */
#define SE_LANG_KW              90      /* Cornish                         */
#define SE_LANG_KY              91      /* Kirgiz                          */
#define SE_LANG_LA              92      /* Latin                           */
#define SE_LANG_LB              93      /* Luxembourgish (Letzeburgesch)   */
#define SE_LANG_LEZ             94      /* Lezghian (Lezgian)              */
#define SE_LANG_LN              95      /* Lingala                         */
#define SE_LANG_LO              96      /* Lao                             */
#define SE_LANG_LT              97      /* Lithuanian                      */
#define SE_LANG_LV              98      /* Latvian                         */
#define SE_LANG_MG              99      /* Malagasy                        */
#define SE_LANG_MH              100     /* Marshallese                     */
#define SE_LANG_MI              101     /* Maori                           */
#define SE_LANG_MK              102     /* Macedonian                      */
#define SE_LANG_ML              103     /* Malayalam                       */
#define SE_LANG_MN              104     /* Mongolian                       */
#define SE_LANG_MO              105     /* Moldavian                       */
#define SE_LANG_MR              106     /* Marathi (Devanagari script)     */
#define SE_LANG_MT              107     /* Maltese                         */
#define SE_LANG_MY              108     /* Burmese (Myanmar)               */
#define SE_LANG_NB              109     /* Norwegian Bokmal                */
#define SE_LANG_NDS             110     /* Low Saxon                       */
#define SE_LANG_NE              111     /* Nepali (Devanagari script)      */
#define SE_LANG_NL              112     /* Dutch                           */
#define SE_LANG_NN              113     /* Norwegian Nynorsk               */
#define SE_LANG_NO              114     /* Norwegian                       */
#define SE_LANG_NY              115     /* Chichewa                        */
#define SE_LANG_OC              116     /* Occitan                         */
#define SE_LANG_OM              117     /* Oromo or Galla                  */
#define SE_LANG_OR              118     /* Oriya                           */
#define SE_LANG_OS              119     /* Ossetic                         */
#define SE_LANG_PA              120     /* Punjabi (Gurumukhi script)      */
#define SE_LANG_PL              121     /* Polish                          */
#define SE_LANG_PS_AF           122     /* Pashto in Afghanistan           */
#define SE_LANG_PS_PK           123     /* Pashto in Pakistan              */
#define SE_LANG_PT              124     /* Portuguese                      */
#define SE_LANG_RM              125     /* Rhaeto-Romance                  */
#define SE_LANG_RO              126     /* Romanian                        */
#define SE_LANG_RU              127     /* Russian                         */
#define SE_LANG_SA              128     /* Sanskrit (Devanagari script)    */
#define SE_LANG_SAH             129     /* Yakut                           */
#define SE_LANG_SCO             130     /* Scots                           */
#define SE_LANG_SE              131     /* North Sami                      */
#define SE_LANG_SEL             132     /* Selkup (Ostyak-Samoyed)         */
#define SE_LANG_SH              133     /* Serbo-Croatian                  */
#define SE_LANG_SI              134     /* Sinhala (Sinhalese)             */
#define SE_LANG_SK              135     /* Slovak                          */
#define SE_LANG_SL              136     /* Slovenian                       */
#define SE_LANG_SM              137     /* Samoan                          */
#define SE_LANG_SMA             138     /* South Sami                      */
#define SE_LANG_SMJ             139     /* Lule Sami                       */
#define SE_LANG_SMN             140     /* Inari Sami                      */
#define SE_LANG_SMS             141     /* Skolt Sami                      */
#define SE_LANG_SO              142     /* Somali                          */
#define SE_LANG_SQ              143     /* Albanian                        */
#define SE_LANG_SR              144     /* Serbian                         */
#define SE_LANG_SV              145     /* Swedish                         */
#define SE_LANG_SW              146     /* Swahili                         */
#define SE_LANG_SYR             147     /* Syriac                          */
#define SE_LANG_TA              148     /* Tamil                           */
#define SE_LANG_TE              149     /* Telugu                          */
#define SE_LANG_TG              150     /* Tajik                           */
#define SE_LANG_TH              151     /* Thai                            */
#define SE_LANG_TI_ER           152     /* Eritrean Tigrinya               */
#define SE_LANG_TI_ET           153     /* Ethiopian Tigrinya              */
#define SE_LANG_TIG             154     /* Tigre                           */
#define SE_LANG_TK              155     /* Turkmen                         */
#define SE_LANG_TL              156     /* Tagalog                         */
#define SE_LANG_TN              157     /* Tswana                          */
#define SE_LANG_TO              158     /* Tonga                           */
#define SE_LANG_TR              159     /* Turkish                         */
#define SE_LANG_TS              160     /* Tsonga                          */
#define SE_LANG_TT              161     /* Tatar                           */
#define SE_LANG_TW              162     /* Twi                             */
#define SE_LANG_TYV             163     /* Tuvinian                        */
#define SE_LANG_UG              164     /* Uighur                          */
#define SE_LANG_UK              165     /* Ukrainian                       */
#define SE_LANG_UR              166     /* Urdu                            */
#define SE_LANG_UZ              167     /* Uzbek                           */
#define SE_LANG_VEN             168     /* Venda                           */
#define SE_LANG_VI              169     /* Vietnamese                      */
#define SE_LANG_VO              170     /* Volapuk                         */
#define SE_LANG_VOT             171     /* Votic                           */
#define SE_LANG_WA              172     /* Walloon                         */
#define SE_LANG_WEN             173     /* Sorbian                         */
#define SE_LANG_WO              174     /* Wolof                           */
#define SE_LANG_XH              175     /* Xhosa                           */
#define SE_LANG_YAP             176     /* Yapese                          */
#define SE_LANG_YI              177     /* Yiddish                         */
#define SE_LANG_YO              178     /* Yoruba                          */
#define SE_LANG_ZH_CN           179     /* Chinese (simplified)            */
#define SE_LANG_ZH_HK           180     /* Chinese (Hong Kong)             */
#define SE_LANG_ZH_MO           181     /* Chinese (Macau)                 */
#define SE_LANG_ZH_SG           182     /* Chinese (Singapore)             */
#define SE_LANG_ZH_TW           183     /* Chinese (traditional)           */
#define SE_LANG_ZU              184     /* Zulu                            */
/*}} language_code_constants }}*/


/* Script Easy font description type definition */

typedef struct{
    const char      *font_name;
    const int        font_id;
} se_font_name_map;

typedef struct
{
    int                        font_table_size;
    int                        font_name_map_size;
    const se_font_name_map     *name_map;
    const void                 **font_table;
    const unsigned char        *logical_font_table;
} se_font_description;

extern const se_font_description se_font_desc;
extern int   se_minigui_mem_pool[];
extern int   se_minigui_mem_pool_size;
extern int   se_minigui_cache_mem_pool[];
extern int   se_minigui_cache_mem_pool_size;


void se_minigui_init_fonts(void);
void se_minigui_destroy_fonts(void);
#ifdef SE_FONT_DYNAMIC_LOADING_SUPPORT
    unsigned int se_minigui_read_font(
            void*           name,
            void*           buffer,
            unsigned int    offset,
            unsigned int    size);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SE_MINIGUI_H */

