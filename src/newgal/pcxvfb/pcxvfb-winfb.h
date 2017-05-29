/**
 ** $Id: pcxvfb-winfb.h 7358 2007-08-16 05:05:28Z xgwang $
 **  
 ** pcxvfb-winfb.h: win32 virtual fb implementation and ipc operations
 **                 used by pcxvfb.
 **
 ** Copyright (C) 2004 ~ 2007 Feynman Software.
 */

int win_PCXVFbAvailable (void);
void* win_PCXVFbInit (char* execl_file, char* cmdline, const char *skin);
void win_PCXVFbClose (void);
void win_PCXVFbLock (void);
void win_PCXVFbUnlock (void);
int win_PCXVFbCloseSocket (int sockfd);


