/* $Id: winfb.h 6341 2006-03-22 08:27:57Z weiym $
** winfb.h
**/

int win_FbAvailable (void);
void* win_FbInit (int w, int h, int depth);
void win_FbClose (void);
void win_FbLock (void);
void win_FbUnlock (void);


