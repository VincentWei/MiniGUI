#include <stdio.h>
#include "xxvfb.h"
static void usage( char *app )
{
    printf( "Usage: %s [-width width] [-height height] [-depth depth] "
            "Supported depths: 1, 4, 8, 32\n", app );
}

int main(int argc, char *argv[])
{
    int width = 240;
    int height = 320;
    int depth = 24;
    int ppid;
    char caption[256];
    char *skin;

    if(argc >= 2)
    {
        ppid = atoi (argv[1]);
    }

    if(argc >= 3)
    {
        char* str = argv[2];
        memcpy(caption,str,strlen(str)+1);
    }
    if(argc >= 4)
    {
        width = atoi (argv[3]);
        height = atoi (strchr (argv[3], 'x') + 1);
        depth = atoi (strrchr (argv[3], '-') + 1);
    }
    if(argc >= 5) //skin
    {
        skin=argv[4];
    }

    fprintf(stderr,"ppid:%d,width:%d,height:%d,depth:%d,caption:%s\n",ppid,width,height,depth,caption);
    MainWndLoop(ppid,width,height,depth,caption);
}
