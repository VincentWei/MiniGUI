#!/bin/sh

cat m4/*.m4 > acinclude.m4
aclocal
autoheader
automake --add-missing --foreign --copy
autoconf
