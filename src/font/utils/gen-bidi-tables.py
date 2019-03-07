#!/usr/bin/env python

# Wrotten by Vincent Wei for MiniGUI 3.4
# Create on 2019-03-07

from __future__ import print_function, division, absolute_import

import io, sys

NR_UNICODE_CHARS = 0x10FFFF + 1

#if len (sys.argv) != 4:
#    print ("usage: ./gen-bidi-tables.py", file=sys.stderr)
#    sys.exit (1)

filenames = ['ucd/extracted/DerivedBidiClass.txt', 'ucd/BidiBrackets.txt', 'ucd/BidiMirroring.txt']

files = [io.open (x, encoding='utf-8') for x in filenames]

headers = [[f.readline () for i in range (2)] for f in files]
headers.insert (0, ["UnicodeData.txt does not have a header."])

# Read fileds[] in UnicodeData.txt to data

data = [{} for f in files]
values = [{} for f in files]
for i, f in enumerate (files):
    for line in f:

        j = line.find ('#')
        if j >= 0:
            line = line[:j]

        fields = [x.strip () for x in line.split (';')]
        if len (fields) == 1:
            continue

        uu = fields[0].split ('..')
        start = int (uu[0], 16)
        if len (uu) == 1:
            end = start
        else:
            end = int (uu[1], 16)

        if i == 1:
            t = fields[2]
        else:
            t = fields[1]

        for u in range (start, end + 1):
            data[i][u] = t
        values[i][t] = values[i].get (t, 0) + end - start + 1

# The unassigned code points that default to AL are in the ranges:
#     [\u0600-\u07BF \u0860-\u086F \u08A0-\u08FF \uFB50-\uFDCF \uFDF0-\uFDFF \uFE70-\uFEFF
#      \U00010D00-\U00010D3F \U00010F30-\U00010F6F \U0001EC70-\U0001ECBF \U0001EE00-\U0001EEFF]
#
#     This includes code points in the Arabic, Syriac, and Thaana blocks, among others.
#
# The unassigned code points that default to R are in the ranges:
#     [\u0590-\u05FF \u07C0-\u085F \u0870-\u089F \uFB1D-\uFB4F \U00010800-\U00010CFF \U00010D40-\U00010F2F
#      \U00010F70-\U00010FFF \U0001E800-\U0001EC6F \U0001ECC0-\U0001EDFF \U0001EF00-\U0001EFFF]
#
#     This includes code points in the Hebrew, NKo, and Phoenician blocks, among others.
#
# The unassigned code points that default to ET are in the range:
#     [\u20A0-\u20CF]
#
#     This consists of code points in the Currency Symbols block.
#
# The unassigned code points that default to BN have one of the following properties:
#     Default_Ignorable_Code_Point
#     Noncharacter_Code_Point
#
# For all other cases:
#  All code points not explicitly listed for Bidi_Class
#  have the value Left_To_Right (L).
# @missing: 0000..10FFFF; Left_To_Right

# Initialize bidi_types list with the default Bidi class

bidi_types = ["BIDI_TYPE_LTR" for i in range(NR_UNICODE_CHARS)]

for i in range(0x0590, 0x0600):
    bidi_types[i] = "BIDI_TYPE_RTL"

for i in range(0x07C0, 0x0900):
    bidi_types[i] = "BIDI_TYPE_RTL"

for i in range(0xFB1D, 0xFB50):
    bidi_types[i] = "BIDI_TYPE_RTL"

for i in range(0x0600, 0x07C0):
    bidi_types[i] = "BIDI_TYPE_AL"

for i in range(0xFB50, 0xFDD0):
    bidi_types[i] = "BIDI_TYPE_AL"

for i in range(0xFDF0, 0xFE00):
    bidi_types[i] = "BIDI_TYPE_AL"

for i in range(0xFE70, 0xFF00):
    bidi_types[i] = "BIDI_TYPE_AL"

for i in range(0x2060, 0x2070):
    bidi_types[i] = "BIDI_TYPE_BN"

for i in range(0x20A0, 0x20D0):
    bidi_types[i] = "BIDI_TYPE_ET"

for i in range(0xFDD0, 0xFDF0):
    bidi_types[i] = "BIDI_TYPE_BN"

for i in range(0xFFF0, 0xFFF9):
    bidi_types[i] = "BIDI_TYPE_BN"

for i in range(0xFFFF, NR_UNICODE_CHARS, 0x10000):
    bidi_types[i - 1] = bidi_types[i] = "BIDI_TYPE_BN"

for i in range(0x10800, 0x10D00):
    bidi_types[i] = "BIDI_TYPE_RTL"

for i in range(0x10D40, 0x10F30):
    bidi_types[i] = "BIDI_TYPE_RTL"

for i in range(0x10F70, 0x11000):
    bidi_types[i] = "BIDI_TYPE_RTL"

for i in range(0x1E800, 0x1EC70):
    bidi_types[i] = "BIDI_TYPE_RTL"

for i in range(0x1ECC0, 0x1EE00):
    bidi_types[i] = "BIDI_TYPE_RTL"

for i in range(0x1EF00, 0x1F000):
    bidi_types[i] = "BIDI_TYPE_RTL"

for i in range(0xE0000, 0xE1000):
    bidi_types[i] = "BIDI_TYPE_BN"

# Fill bidi_types
d = data[0]
for u, v in d.items():
    v = v.strip()
    if cmp(v, 'L') == 0:
        bidi_types[u] = 'BIDI_TYPE_LTR'
    elif cmp(v, 'R') == 0:
        bidi_types[u] = 'BIDI_TYPE_RTL'
    elif cmp(v, 'EN') == 0:
        bidi_types[u] = 'BIDI_TYPE_EN'
    elif cmp(v, 'ES') == 0:
        bidi_types[u] = 'BIDI_TYPE_ES'
    elif cmp(v, 'ET') == 0:
        bidi_types[u] = 'BIDI_TYPE_ET'
    elif cmp(v, 'AN') == 0:
        bidi_types[u] = 'BIDI_TYPE_AN'
    elif cmp(v, 'CS') == 0:
        bidi_types[u] = 'BIDI_TYPE_CS'
    elif cmp(v, 'B') == 0:
        bidi_types[u] = 'BIDI_TYPE_BS'
    elif cmp(v, 'S') == 0:
        bidi_types[u] = 'BIDI_TYPE_SS'
    elif cmp(v, 'WS') == 0:
        bidi_types[u] = 'BIDI_TYPE_WS'
    elif cmp(v, 'ON') == 0:
        bidi_types[u] = 'BIDI_TYPE_ON'
    elif cmp(v, 'BN') == 0:
        bidi_types[u] = 'BIDI_TYPE_BN'
    elif cmp(v, 'NSM') == 0:
        bidi_types[u] = 'BIDI_TYPE_NSM'
    elif cmp(v, 'AL') == 0:
        bidi_types[u] = 'BIDI_TYPE_AL'
    elif cmp(v, 'LRO') == 0:
        bidi_types[u] = 'BIDI_TYPE_LRO'
    elif cmp(v, 'RLO') == 0:
        bidi_types[u] = 'BIDI_TYPE_RLO'
    elif cmp(v, 'LRE') == 0:
        bidi_types[u] = 'BIDI_TYPE_LRE'
    elif cmp(v, 'RLE') == 0:
        bidi_types[u] = 'BIDI_TYPE_RLE'
    elif cmp(v, 'PDF') == 0:
        bidi_types[u] = 'BIDI_TYPE_PDF'
    elif cmp(v, 'LRI') == 0:
        bidi_types[u] = 'BIDI_TYPE_LRI'
    elif cmp(v, 'RLI') == 0:
        bidi_types[u] = 'BIDI_TYPE_RLI'
    elif cmp(v, 'FSI') == 0:
        bidi_types[u] = 'BIDI_TYPE_FSI'
    elif cmp(v, 'PDI') == 0:
        bidi_types[u] = 'BIDI_TYPE_PDI'
    else:
        print ("Unknown BIDI type: %s" % v)

print ("/* == Start of generated table == */")
print ("/*")
print (" * The following table is generated by running:")
print (" *")
print (" *   ./gen-bidi-tables.py")
print (" *")
print (" * on ucd/extracted/DerivedBidiClass.txt, ucd/BidiBrackets.txt, and ucd/BidiMirroring.txt files with these headers:")
print (" *")
for h in headers:
    for l in h:
        print (" * %s" % (l.strip()))
print (" */")
print ()

print ("#ifndef _UNICODE_BIDI_TABLES_H")
print ("#define _UNICODE_BIDI_TABLES_H")
print ("")
print ("static const BIDICHAR_TYPE_MAP __mg_unicode_bidi_char_type_map[] =")
print ("{")

cur_chv = 0
cur_count = 1
cur_type = bidi_types[0]
for i in range(1, NR_UNICODE_CHARS):
    if bidi_types[i] is not cur_type:
        if cur_count == 0:
            cur_count = 1
        print ("    { 0x%06X, 0x%04X, %s }," % (cur_chv, cur_count, cur_type, ))
        cur_chv = i
        cur_count = 1
        cur_type = bidi_types[i]
    else:
        cur_count += 1

print ("};")
print ("")
print ("static const BIDICHAR_BRACKET __mg_unicode_bracket_table[] =")
print ("{")

d = data[1]
uu = sorted (d.keys())
for u in uu:
    v = d[u]
    v = v.strip()
    if cmp(v, 'o') == 0:
        bracket_type = "BIDICHAR_BRACKET_OPEN"
    elif cmp(v, 'c') == 0:
        bracket_type = "BIDICHAR_BRACKET_CLOSE"
    else:
        bracket_type = "BIDICHAR_BRACKET_NONE"

    print ("    { 0x%06X, %s }," % (u, bracket_type, ))

print ("};")
print ("")

print ("static const BIDICHAR_MIRROR_MAP __mg_unicode_mirror_table[] =")
print ("{")

d = data[2]
uu = sorted (d.keys())
for u in uu:
    v = d[u]
    mirrored = int (v, 16)
    print ("    { 0x%06X, 0x%06X }," % (u, mirrored, ))

print ("};")

print ()
print ("#endif /* !_UNICODE_BIDI_TABLES_H */")

print ()
print ("/* == End of generated table == */")

