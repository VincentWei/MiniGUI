#!/bin/bash

url='http://www.unicode.org/Public/emoji/12.0/'
files='emoji-data.txt emoji-sequences.txt emoji-variation-sequences.txt emoji-zwj-sequences.txt'

if test ! -d emoji; then
    mkdir emoji
fi

cd emoji/

for file in $files
do
    if [ -f $file ]; then
        echo "$file exists, skipping"
    else
        echo "Fetching $url$file"
        wget $url$file
    fi
done
