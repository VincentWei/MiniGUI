#!/usr/bin/env python

# Copied from HarfBuzz
#   https://github.com/harfbuzz/harfbuzz
# On 2019-03-07
#
# Revised by Vincent Wei for MiniGUI 3.4

from __future__ import print_function, division, absolute_import

import io, os.path, sys

#if len (sys.argv) != 4:
#	print ("usage: ./gen-arabic-table.py", file=sys.stderr)
#	sys.exit (1)

filenames = ['ucd/ArabicShaping.txt', 'ucd/UnicodeData.txt', 'ucd/Blocks.txt']

files = [io.open (x, encoding='utf-8') for x in filenames]

headers = [[files[0].readline (), files[0].readline ()], [files[2].readline (), files[2].readline ()]]
headers.append (["UnicodeData.txt does not have a header."])
while files[0].readline ().find ('##################') < 0:
	pass

blocks = {}
def read_blocks(f):
	global blocks
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

		t = fields[1]

		for u in range (start, end + 1):
			blocks[u] = t

def print_joining_table(f):

	values = {}
	for line in f:

		if line[0] == '#':
			continue

		fields = [x.strip () for x in line.split (';')]
		if len (fields) == 1:
			continue

		u = int (fields[0], 16)

		if fields[3] in ["ALAPH", "DALATH RISH"]:
			value = "JOINING_GROUP_" + fields[3].replace(' ', '_')
		else:
			value = "JOINING_TYPE_" + fields[2]
		values[u] = value

	short_value = {}
	for value in set([v for v in values.values()] + ['JOINING_TYPE_X']):
		short = ''.join(x[0] for x in value.split('_')[2:])
		assert short not in short_value.values()
		short_value[value] = short

	print ()
	for value,short in short_value.items():
		print ("#define %s	%s" % (short, value))

	uu = sorted(values.keys())
	num = len(values)
	all_blocks = set([blocks[u] for u in uu])

	last = -100000
	ranges = []
	for u in uu:
		if u - last <= 1+16*5:
			ranges[-1][-1] = u
		else:
			ranges.append([u,u])
		last = u

	print ()
	print ("static const Uint8 joining_table[] =")
	print ("{")
	last_block = None
	offset = 0
	for start,end in ranges:

		print ()
		print ("#define joining_offset_0x%04xu %d" % (start, offset))

		for u in range(start, end+1):

			block = blocks.get(u, last_block)
			value = values.get(u, "JOINING_TYPE_X")

			if block != last_block or u == start:
				if u != start:
					print ()
				if block in all_blocks:
					print ("\n  /* %s */" % block)
				else:
					print ("\n  /* FILLER */")
				last_block = block
				if u % 32 != 0:
					print ()
					print ("  /* %04X */" % (u//32*32), "  " * (u % 32), end="")

			if u % 32 == 0:
				print ()
				print ("  /* %04X */ " % u, end="")
			print ("%s," % short_value[value], end="")
		print ()

		offset += end - start + 1
	print ()
	occupancy = num * 100. / offset
	print ("}; /* Table items: %d; occupancy: %d%% */" % (offset, occupancy))
	print ()

	print ("")
	print ("static inline BOOL uc_in_range(Uchar32 uc, Uchar32 lo, Uchar32 hi)")
	print ("{")
	print ("   /* The casts below are important as if T is smaller than int,")
	print ("    * the subtract results will become a signed int! */")
	print ("   return (Uchar32)(u - lo) <= (Uchar32)(hi - lo);")
	print ("}")
	print ("")

	page_bits = 12;
	print ()
	print ("static unsigned int")
	print ("joining_type (Uchar32 u)")
	print ("{")
	print ("  switch (u >> %d)" % page_bits)
	print ("  {")
	pages = set([u>>page_bits for u in [s for s,e in ranges]+[e for s,e in ranges]])
	for p in sorted(pages):
		print ("    case 0x%0Xu:" % p)
		for (start,end) in ranges:
			if p not in [start>>page_bits, end>>page_bits]: continue
			offset = "joining_offset_0x%04xu" % start
			print ("      if (uc_in_range(u, 0x%04Xu, 0x%04Xu)) return joining_table[u - 0x%04Xu + %s];" % (start, end, start, offset))
		print ("      break;")
		print ("")
	print ("    default:")
	print ("      break;")
	print ("  }")
	print ("  return X;")
	print ("}")
	print ()
	for value,short in short_value.items():
		print ("#undef %s" % (short))
	print ()

def print_shaping_table(f):

	shapes = {}
	ligatures = {}
	names = {}
	for line in f:

		fields = [x.strip () for x in line.split (';')]
		if fields[5][0:1] != '<':
			continue

		items = fields[5].split (' ')
		shape, items = items[0][1:-1], tuple (int (x, 16) for x in items[1:])

		if not shape in ['initial', 'medial', 'isolated', 'final']:
			continue

		c = int (fields[0], 16)
		if len (items) != 1:
			# We only care about lam-alef ligatures
			if len (items) != 2 or items[0] != 0x0644 or items[1] not in [0x0622, 0x0623, 0x0625, 0x0627]:
				continue

			# Save ligature
			names[c] = fields[1]
			if items not in ligatures:
				ligatures[items] = {}
			ligatures[items][shape] = c
			pass
		else:
			# Save shape
			if items[0] not in names:
				names[items[0]] = fields[1]
			else:
				names[items[0]] = os.path.commonprefix ([names[items[0]], fields[1]]).strip ()
			if items[0] not in shapes:
				shapes[items[0]] = {}
			shapes[items[0]][shape] = c

	print ()
	print ("static const Uint16 shaping_table[][4] =")
	print ("{")

	keys = shapes.keys ()
	min_u, max_u = min (keys), max (keys)
	for u in range (min_u, max_u + 1):
		s = [shapes[u][shape] if u in shapes and shape in shapes[u] else 0
		     for shape in  ['initial', 'medial', 'final', 'isolated']]
		value = ', '.join ("0x%04Xu" % c for c in s)
		print ("  {%s}, /* U+%04X %s */" % (value, u, names[u] if u in names else ""))

	print ("};")
	print ()
	print ("#define SHAPING_TABLE_FIRST	0x%04Xu" % min_u)
	print ("#define SHAPING_TABLE_LAST	0x%04Xu" % max_u)
	print ()

	ligas = {}
	for pair in ligatures.keys ():
		for shape in ligatures[pair]:
			c = ligatures[pair][shape]
			if shape == 'isolated':
				liga = (shapes[pair[0]]['initial'], shapes[pair[1]]['final'])
			elif shape == 'final':
				liga = (shapes[pair[0]]['medial'], shapes[pair[1]]['final'])
			else:
				raise Exception ("Unexpected shape", shape)
			if liga[0] not in ligas:
				ligas[liga[0]] = []
			ligas[liga[0]].append ((liga[1], c))
	max_i = max (len (ligas[l]) for l in ligas)
	print ()
	print ("static const struct ligature_set_t {")
	print (" Uint16 first;")
	print (" struct ligature_pairs_t {")
	print ("   Uint16 second;")
	print ("   Uint16 ligature;")
	print (" } ligatures[%d];" % max_i)
	print ("} ligature_table[] =")
	print ("{")
	for first in sorted (ligas.keys ()):

		print ("  { 0x%04Xu, {" % (first))
		for liga in ligas[first]:
			print ("    { 0x%04Xu, 0x%04Xu }, /* %s */" % (liga[0], liga[1], names[liga[1]]))
		print ("  }},")

	print ("};")
	print ()



print ("/* == Start of generated table == */")
print ("/*")
print (" * The following table is generated by running:")
print (" *")
print (" *   ./gen-arabic-table.py")
print (" *")
print (" * on ucd/ArabicShaping.txt ucd/UnicodeData.txt ucd/Blocks.txt files with these headers:")
print (" *")
for h in headers:
	for l in h:
		print (" * %s" % (l.strip()))
print (" */")
print ()
print ("#ifndef _MGFONT_UNICODE_ARABIC_SHAPING_TABLES_H")
print ("#define _MGFONT_UNICODE_ARABIC_SHAPING_TABLES_H")
print ()

read_blocks (files[2])
print_joining_table (files[0])
print_shaping_table (files[1])

print ()
print ("#endif /* _MGFONT_UNICODE_ARABIC_SHAPING_TABLES_H */")
print ()
print ("/* == End of generated table == */")
