/*
 * The following code is used by MDE to get the applications information.
 *
 * It gets the information from a INI-like configuration file.
 */

#define APP_INFO_FILE "mginit.rc"

static BOOL get_app_info (void)
{
    int i;
    APPITEM* item;

    /* Get the number of the applications */
    if (GetIntValueFromEtcFile (APP_INFO_FILE, "mginit", "nr", &app_info.nr_apps) != ETC_OK)
        return FALSE;

    if (app_info.nr_apps <= 0)
        return FALSE;

    /* Get the index of the autostart application. */
    GetIntValueFromEtcFile (APP_INFO_FILE, "mginit", "autostart", &app_info.autostart);

    if (app_info.autostart >= app_info.nr_apps || app_info.autostart < 0)
        app_info.autostart = 0;

    /* Allocate application information structures. */
    if ((app_info.app_items = (APPITEM*)calloc (app_info.nr_apps, sizeof (APPITEM))) == NULL) {
        return FALSE;
    }

    /* Get the path, name, and icon of every application. */
    item = app_info.app_items;
    for (i = 0; i < app_info.nr_apps; i++, item++) {
        char section [10];

        sprintf (section, "app%d", i);
        if (GetValueFromEtcFile (APP_INFO_FILE, section, "path", item->path, PATH_MAX) != ETC_OK)
            goto error;

        if (GetValueFromEtcFile (APP_INFO_FILE, section, "name", item->name, NAME_MAX) != ETC_OK)
            goto error;

        if (GetValueFromEtcFile (APP_INFO_FILE, section, "layer", item->layer, LEN_LAYER_NAME) != ETC_OK)
            goto error;

        if (GetValueFromEtcFile (APP_INFO_FILE, section, "tip", item->tip, TIP_MAX) != ETC_OK)
            goto error;

        strsubchr (item->tip, '&', ' ');

        if (GetValueFromEtcFile (APP_INFO_FILE, section, "icon", item->bmp_path, PATH_MAX + NAME_MAX) != ETC_OK)
            goto error;

        if (LoadBitmap (HDC_SCREEN, &item->bmp, item->bmp_path) != ERR_BMP_OK)
            goto error;

        item->cdpath = TRUE;
    }
    return TRUE;

error:
    free_app_info ();
    return FALSE;
}
