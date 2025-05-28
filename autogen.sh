#!/bin/sh

libtoolize
cat m4/*.m4 > aclocal.m4
aclocal
autoheader
automake --add-missing --foreign --copy
autoconf
