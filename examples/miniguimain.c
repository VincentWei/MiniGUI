/*
 * This program parses the command line arguments.
 * If the user specified a layer name by using "-layer <layer_name", 
 * the program will try to join the layer, otherwise create a new layer.
 */
int MiniGUIMain (int args, const char* arg[])
{
#ifdef _MGRM_PROCESSES
    int i;
    const char* layer = NULL;

    for (i = 1; i < args; i++) {
        if (strcmp (arg[i], "-layer") == 0) {
            layer = arg[i + 1];
            break;
        }
    }

    if (GetLayerInfo (layer, NULL, NULL, NULL) == INV_LAYER_HANDLE) {
        printf ("GetLayerInfo: the requested layer does not exist.\n");
    }

    if (JoinLayer (layer, arg[0], 0, 0) == INV_LAYER_HANDLE) {
        printf ("JoinLayer: invalid layer handle.\n");
        exit (1);
    }
#endif

    ...

    return 0;
}

