#!/bin/bash
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

if [ $# -ne 1 ]; then
    echo "Usage: $0 <product>"
    exit 1
fi

PICTURE_DIR=$PWD/pictures/$1
DAT_DIR=$PWD/dat_files
C_DIR=$PWD/c_files
CRYPTOOL=$PWD/cryptool

if [ ! -d $PICTURE_DIR ]; then
    echo "Fatal: Picture directory '$PICTURE_DIR' does not exist"
    exit 1
fi

#RC4KEY=`echo -n $RANDOM | md5sum | awk '{print $1}'`
#RC4KEY=`awk 'BEGIN {srand();for (i=1;i<=16;i++) printf("%02x",int(256*rand()))}'`
#RC4KEY=`./genkey.sh | awk '{print $1}'`
#echo "KEY: $RC4KEY"
#echo

#TMPRC4KEY=`echo -n $RC4KEY | awk '{for(i=1;i<=32;i+=2) {print "0x"substr($1,i,2)}}'`
#echo $TMPRC4KEY

#
# picture -> dat
#
mkdir $DAT_DIR > /dev/null 2>&1
OLDPWD=$PWD
cd $PICTURE_DIR
LIST=`ls *.png`

for src in $LIST; do
    dst=`echo $src | sed 's/png$/dat/'`
    dst=$DAT_DIR/$dst

    echo "$src --> $dst"
#    $CRYPTOOL -e $src $dst $RC4KEY
    cp $src $dst
done

cd $OLDPWD


#
# dat -> c
#
mkdir $C_DIR > /dev/null 2>&1

# key.c
#CRC4KEY=`echo -n $TMPRC4KEY | sed "s/ /,/g"`
#echo $CRC4KEY
#echo "MG_LOCAL unsigned char splash_crypto_key[16] = {" > $C_DIR/key.c
#echo "    "$CRC4KEY >> $C_DIR/key.c
#echo "};" >> $C_DIR/key.c
#echo "" >> $C_DIR/key.c

cd $DAT_DIR; find . -name \*.dat | sed "s/^\.\/*//g" | sort > ../res.lst; cd ..

../inner-res-trans -m _MG_ENABLE_LICENSE -i $DAT_DIR -o $C_DIR -p __mg_splash -n _splash_inner_res -l res.lst

rm res.lst

find $C_DIR -name \*.dat.c -exec sed 's/unsigned/MG_LOCAL unsigned/' -i {} \;
