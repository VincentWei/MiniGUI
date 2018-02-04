#!/bin/sh
##
##   This file is part of MiniGUI, a mature cross-platform windowing 
##   and Graphics User Interface (GUI) support system for embedded systems
##   and smart IoT devices.
## 
##   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
##   Copyright (C) 1998~2002, WEI Yongming
## 
##   This program is free software: you can redistribute it and/or modify
##   it under the terms of the GNU General Public License as published by
##   the Free Software Foundation, either version 3 of the License, or
##   (at your option) any later version.
## 
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
## 
##   You should have received a copy of the GNU General Public License
##   along with this program.  If not, see <http://www.gnu.org/licenses/>.
## 
##   Or,
## 
##   As this program is a library, any link to this program must follow
##   GNU General Public License version 3 (GPLv3). If you cannot accept
##   GPLv3, you need to be licensed from FMSoft.
## 
##   If you have got a commercial license of this program, please use it
##   under the terms and conditions of the commercial license.
## 
##   For more information about the commercial license, please refer to
##   <http://www.minigui.com/en/about/licensing-policy/>.
##

MGCFG_TRANS=mgcfg-trans #get it from svn+ssh://devsrv/home/projects/svn/mg-tools/trunk/mgcfg-trans/bin/windowsxp or bin/ubuntu10.04
INNER_RES_TRANS=inner-res-trans # get it from svn+ssh://devsrv/home/projects/svn/mg-tools/trunk/inner-res-trans/bin/windowsxp or bin/ubuntu10.04

MINIGUI_CFG=./MiniGUI.cfg #MiniGUI.cfg's path

MGETC_OUT=mgetc.c
FONT_OUTDIR=font
FONT_INDIR=/usr/local/share/minigui/res/ #same as MiniGUI.cfg [resinfo] respath 's value

#change the font want to translate into font_list
#same as MiniGUI.cfg 's font seciton value
# e.g if MiniGUI.cfg 's upf section is:
#[upf]
#font_number=2
#name0=upf-unifont-rrncnn-16-16-ISO8859-1,ISO8859-15,GB2312,BIG5, GBK,UTF-8, UTF-16LE, UTF-16BE
#fontfile0=font/unifont_160_50.upf
#name1=upf-times-rrncnn-5-10-ISO8859-1,ISO8859-15
#fontfile1=font/smoothtimes_100_50.upf
#
#[qpf]
#font_number=1
#name0=qpf-unifont-rrncnn-16-16-ISO8859-1,ISO8859-15,GB2312-0,GBK,BIG5
#fontfile0=font/unifont_160_50.qpf
# --
# the font_list should be
#font/unifont_160_50.upf   upf-unifont-rrncnn-16-16-ISO8859-1,ISO8859-15,GB2312,BIG5, GBK,UTF-8, UTF-16LE, UTF-16BE
#font/smoothtimes_100_50.upf upf-times-rrncnn-5-10-ISO8859-1,ISO8859-15
#font/unifont_160_50.qpf qpf-unifont-rrncnn-16-16-ISO8859-1,ISO8859-15,GB2312-0,GBK,BIG5
#---
# all the font section's value should be include one files
#
#
cat > font_list  << __AEOF
font/song-12-gb2312.bin	rbf-fixed-rrncnn-12-12-GB2312-0
font/Courier-rr-8-13.vbf	vbf-Courier-rrncnn-8-13-ISO8859-1
font/smoothtimes_100_50.upf	upf-times-rrncnn-5-10-ISO8859-1,ISO8859-15
__AEOF

mkdir -p $FONT_OUTDIR

$MGCFG_TRANS -o $MGETC_OUT $MINIGUI_CFG 
$INNER_RES_TRANS -l font_list -i $FONT_INDIR -o $FONT_OUTDIR -n _font_inner_res

echo "Translate OK!"
echo "Now, cp $MGETC_OUT $FONT_OUTDIR  minigui/src/sysres -a"


