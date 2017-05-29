#!/bin/bash

TMP_FILE=_tmp_list

echo "MiniGUI license management $@!@!fsdfewuwqz^%#$@!dfqewqe12132321" > $TMP_FILE
find pictures/ -name \*.png -exec md5sum {} \; | sort -k2 >> $TMP_FILE
md5sum $TMP_FILE
rm -f $TMP_FILE
