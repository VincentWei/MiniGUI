/*
**  $Id: leaks.h 7344 2007-08-16 03:56:40Z xgwang $
**  
**  Copyright (C) 2003 ~ 2007 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming.
**
*/

/* Define this if you want surface leak detection code enabled */
/*#define CHECK_LEAKS*/

/* Global variables used to check leaks in code using GAL */

#ifdef CHECK_LEAKS
extern int surfaces_allocated;
#endif
