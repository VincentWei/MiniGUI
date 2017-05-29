/*
** $Id: threadx_pprivate.c 8944 2007-12-29 08:29:16Z xwyan $
**
** threadx_pprivate.c: This file contains the implementation of the POSIX 
**      pthread functions for ThreadX.
**
** Copyright (C) 2005 ~ 2007 Feynman Software.
** All rights reserved.
**
** Author: Yan Xiaowei
**
** Create Date: 2005-01-11
*/

#include "mgconfig.h"

#if defined (__THREADX__) && defined (_MGUSE_OWN_PTHREAD)

#include "tx_api.h"

#include "threadx_pprivate.h"

extern TX_MUTEX __txpth_pthread_mutex;
void __txpth_get_name (char *name, char type, int lock)
{
	static char *name_template = "fm-00000000";
	int i;
	static int j = 0, k;

	if (lock) tx_mutex_get (&__txpth_pthread_mutex, TX_WAIT_FOREVER);
	
	for (i = 0; name_template[i]; i++)
		name[i] = name_template[i];
	
	name[2] = type;
	
	k = j;
	for (i = 10; i > 2; i--) {
		name [i] = "0123456789ABCDEF" [k&0xF];
		k >>= 4;
	}
	j++;

	if (lock) tx_mutex_put (&__txpth_pthread_mutex);
}

int *__errno_location (void)
{
    TX_THREAD* thread = tx_thread_identify ();

    pthread_info *info = (pthread_info *)thread->tx_thread_name;

    return &info->pth_errno;
}

#endif /* __THREADX__ && _MGUSE_OWN_PTHREAD */
