/*
 *   This file is part of MiniGUI, a mature cross-platform windowing 
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 * 
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */
/*
** threadx_pprivate.c: This file contains the implementation of the POSIX 
**      pthread functions for ThreadX.
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
