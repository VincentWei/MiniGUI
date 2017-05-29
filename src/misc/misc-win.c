#ifdef WIN32

#include <windows.h>
#include <stdio.h>

void mswin_ping()
{
	MessageBeep(MB_OK);
}


#endif  //end of WIN32