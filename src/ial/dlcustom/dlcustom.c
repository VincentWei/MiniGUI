#include "common.h"

#ifdef _MGIAL_DLCUSTOM

#include <dlfcn.h>
#include "ial.h"
#include "dlcustom.h"

#define DEFAULT_CUSTOM_IAL_LIB_PATH "/usr/lib/libiale_custom.so"

static void *dl_handle = NULL;
static BOOL (*sym_InitDLCustomInput) (INPUT* input, const char* mdev, const char* mtype) = NULL;
static void (*sym_TermDLCustomInput) (void) = NULL;

BOOL InitDLCustomInput (INPUT* input, const char* mdev, const char* mtype)
{
    const char *path;

    if ((path = getenv("MG_ENV_DLCUSTOM_IAL")) == NULL)
    {
        path = DEFAULT_CUSTOM_IAL_LIB_PATH;
    }

    dl_handle = dlopen(path, RTLD_NOW);
    if (dl_handle == NULL)
    {
        fprintf(stderr, "dlopen(%s): %s\n", path, dlerror());
        return FALSE;
    }

    sym_InitDLCustomInput = dlsym(dl_handle, "InitDLCustomInput");
    if (! sym_InitDLCustomInput)
    {
        fprintf(stderr, "dlsym(%s): %s\n", "InitDLCustomInput", dlerror());
        return FALSE;
    }

    sym_TermDLCustomInput = dlsym(dl_handle, "TermDLCustomInput");
    if (! sym_TermDLCustomInput)
    {
        fprintf(stderr, "dlsym(%s): %s\n", "TermDLCustomInput", dlerror());
        return FALSE;
    }

    return sym_InitDLCustomInput(input, mdev, mtype);
}

void TermDLCustomInput (void)
{
    if (sym_TermDLCustomInput)
    {
        sym_TermDLCustomInput();
    }
    if (dl_handle)
    {
        dlclose(dl_handle);
    }
}

#endif  /* _MGIAL_DLCUSTOM*/
