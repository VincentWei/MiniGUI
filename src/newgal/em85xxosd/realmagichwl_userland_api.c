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
#include "common.h"

#ifndef _MGGAL_EM85XXYUV

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "realmagichwl_userland/caribbean_plainc.h"
#include "realmagichwl_kernelland/realmagichwl.h"
#include "realmagichwl_userland/realmagichwl_userland_api.h"

RUA_handle RUA_OpenDevice (RMint32 no)
{
	RUA_handle h;	
	h = open ("/dev/realmagichwl0", O_RDONLY);
	return h;
}

void RUA_ReleaseDevice (RUA_handle h)
{	
	close(h);
}

void RUA_DECODER_RESET (RUA_handle h)
{	
	ioctl (h, REALMAGICHWL_IOCTL_DECODER_RESET, NULL);
}

RMint32 RUA_DECODER_PLAY (RUA_handle h, RMuint32 mode)
{
	return ioctl (h, REALMAGICHWL_IOCTL_DECODER_PLAY, &mode);
}

RMint32 RUA_DECODER_STOP (RUA_handle h)
{
	return ioctl (h, REALMAGICHWL_IOCTL_DECODER_STOP, NULL);
}

RMint32 RUA_DECODER_PAUSE(RUA_handle h)
{
	return ioctl(h, REALMAGICHWL_IOCTL_DECODER_PAUSE, NULL);
}

RMint32 RUA_DECODER_AUDIO_SWITCH (RUA_handle h, RMbool activate)
{
	RMint32 activateI = (activate)?1:0;
	
	return ioctl (h, REALMAGICHWL_IOCTL_DECODER_AUDIO_SWITCH, &activateI);
}

RMint32 RUA_DECODER_FEEDME (RUA_handle h, feedpacket *pP)
{
	return ioctl (h, REALMAGICHWL_IOCTL_DECODER_FEEDME, pP);
}

void RUA_DECODER_WAIT (RUA_handle h, RMuint64 timeout_microsecond, RMuint32 *pX)
{
	waitable hw;
	
	hw.mask = *pX;
	hw.timeout_microsecond = timeout_microsecond;
	
	ioctl (h, REALMAGICHWL_IOCTL_DECODER_HAPPENING_WAIT, &hw);
	*pX = hw.mask;
}

RMint32 RUA_DECODER_SET_PROPERTY (RUA_handle h, RMuint32 PropSet, RMuint32 PropId, RMuint32 PropTypeLength, void *pValue)
{
	decoderproperty Dp;
	Dp.PropSet = PropSet;
	Dp.PropId = PropId;
	Dp.PropTypeLength = PropTypeLength;
	Dp.pValue = pValue;
	return ioctl (h, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);
}

RMint32 RUA_DECODER_GET_PROPERTY (RUA_handle h, RMuint32 PropSet,RMuint32 PropId, RMuint32 PropTypeLength, void *pValue)
{
	decoderproperty Dp;
	
	Dp.PropSet = PropSet;
	Dp.PropId = PropId;
	Dp.PropTypeLength = PropTypeLength;
	Dp.pValue = pValue;
	return ioctl (h, REALMAGICHWL_IOCTL_DECODER_GET_PROPERTY, &Dp);
}

RMint32 RUA_OSDFB_SWITCH (RUA_handle h, OSDBuffer *osdbuffer)
{
	osdbuffer->control = 1;	// uCode controlled
	return ioctl (h, REALMAGICHWL_IOCTL_OSDFB_SWITCH, osdbuffer);
}

RMint32 RUA_OSDFB_SWITCH_EX (RUA_handle h, OSDBuffer *osdbuffer)
{
	return ioctl (h, REALMAGICHWL_IOCTL_OSDFB_SWITCH, osdbuffer);
}

RMint32 RUA_OSDFB_REFRESH (RUA_handle h, OSDBuffer *osdbuffer)
{
	return ioctl (h, REALMAGICHWL_IOCTL_OSDFB_REFRESH, osdbuffer);
}

RMint32 RUA_OSDFB_GENERAL_ALPHA (RUA_handle h, RMint32 alpha)
{
	return ioctl (h, REALMAGICHWL_IOCTL_OSDFB_GENERAL_ALPHA, &alpha);
}

RMint32 RUA_DECODER_CLEAR_SCREEN (RUA_handle h)
{
	return ioctl (h, REALMAGICHWL_IOCTL_DECODER_CLEAR_SCREEN, 0);
}

RMint32 RUA_DECODER_DISPLAY_YUV (RUA_handle h, YUVframe *pF)
{
	return ioctl (h,REALMAGICHWL_IOCTL_DECODER_DISPLAY_YUV, pF);
}

RMint32 RUA_SM2288_DIAGNOSIS (RUA_handle h, SM2288diagnosis_type test)
{
	return ioctl (h, REALMAGICHWL_IOCTL_ENCODER_TEST, test);
}

RMint32 RUA_ENCODER_RESET (RUA_handle h)
{
	return ioctl (h, REALMAGICHWL_IOCTL_ENCODER_RESET, 0);
}

RMint32 RUA_ENCODER_START (RUA_handle h)
{
	return ioctl (h, REALMAGICHWL_IOCTL_ENCODER_START, 0);
}
		
RMint32 RUA_ENCODER_PAUSE (RUA_handle h)
{
	return ioctl (h, REALMAGICHWL_IOCTL_ENCODER_PAUSE, 0);
}

RMint32 RUA_ENCODER_RESUME (RUA_handle h)
{
	return ioctl (h, REALMAGICHWL_IOCTL_ENCODER_RESUME, 0);
}

RMint32 RUA_ENCODER_STOP (RUA_handle h)
{
	return ioctl (h, REALMAGICHWL_IOCTL_ENCODER_STOP, 0);
}

RMint32 RUA_ENCODER_SETPARAMETERS (RUA_handle h, unsigned short params[SM2288_PARAMHSIZEINWORDS])
{
	return ioctl (h, REALMAGICHWL_IOCTL_ENCODER_SETPARAMETERS, params);
}

#endif /* !_MGGAL_EM85XXYUV */

