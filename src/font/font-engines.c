#include "common.h"
#include "minigui.h"
#include "gdi.h"

typedef struct {
        char* type;
        FONTOPS* ops;
}FONTOPS_INFO;

#ifdef _MGFONT_RBF
extern FONTOPS __mg_rbf_ops;
#endif

#ifdef _MGFONT_VBF
extern FONTOPS __mg_vbf_ops;
#endif

#ifdef _MGFONT_QPF
extern FONTOPS __mg_qpf_ops;
#endif

#ifdef _MGFONT_UPF
extern FONTOPS __mg_upf_ops;
#endif

#if defined _MGFONT_FT2 || defined _MGFONT_TTF
extern FONTOPS __mg_ttf_ops;
#endif

FONTOPS_INFO __mg_fontops_infos[] = {
#ifdef _MGFONT_RBF
    {FONT_TYPE_NAME_BITMAP_RAW, &__mg_rbf_ops},
#endif
#ifdef _MGFONT_VBF
    {FONT_TYPE_NAME_BITMAP_VAR, &__mg_vbf_ops},
#endif
#ifdef _MGFONT_QPF
    {FONT_TYPE_NAME_BITMAP_QPF, &__mg_qpf_ops},
#endif
#ifdef _MGFONT_UPF
    {FONT_TYPE_NAME_BITMAP_UPF, &__mg_upf_ops},
#endif
#if defined _MGFONT_FT2 || defined _MGFONT_TTF
    {FONT_TYPE_NAME_SCALE_TTF, &__mg_ttf_ops},
#endif
    {NULL, NULL},
};

