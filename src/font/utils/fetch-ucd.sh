#!/bin/bash

url='https://www.unicode.org/Public/12.0.0/ucd/'
files='ReadMe.txt Scripts.txt UnicodeData.txt LineBreak.txt SpecialCasing.txt CaseFolding.txt CompositionExclusions.txt IndicPositionalCategory.txt IndicSyllabicCategory.txt ArabicShaping.txt BidiBrackets.txt BidiMirroring.txt Blocks.txt extracted/DerivedEastAsianWidth.txt extracted/DerivedBidiClass.txt'

if test ! -d ucd/extracted; then
    mkdir -p ucd/extracted
fi

cd ucd/

for file in $files
do
    if [ -f $file ]; then
        echo "$file exists, skipping"
    else
        echo "Fetching $url$file"
        wget -O $file $url$file
    fi
done
