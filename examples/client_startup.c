/* 
 * This program tries to create a new layer named "vcongui" if
 * there is no such layer. If there is already a layer named "vcongui",
 * this program brings the layer to be the topmost one.
 */
int MiniGUIMain (int args, const char* arg[])
{
    GHANDLE layer;

    layer = GetLayerInfo ("vcongui", NULL, NULL, NULL);
    if (layer != INV_LAYER_HANDLE) {
        SetTopmostLayer (TRUE, layer, NULL);
        return 0;
    }

    if (JoinLayer ("vcongui", "vcongui", 0, 0) == INV_LAYER_HANDLE) {
        printf ("JoinLayer: invalid layer handle.\n");
        return 1;
    }

    ...

    return 0;
}

