#!/bin/bash

url='https://www.unicode.org/Public/11.0.0/ucd/'
files='Scripts.txt UnicodeData.txt IndicPositionalCategory.txt IndicSyllabicCategory.txt ArabicShaping.txt BidiBrackets.txt BidiMirroring.txt Blocks.txt'

if test ! -d ucd; then
    mkdir ucd
fi

cd ucd/

for file in $files
do
    if [ -f $file ]; then
        echo "$file exists, skipping"
    else
        echo "Fetching $url$file"
        wget $url$file
    fi
done
