#!/bin/sh

cat m4/*.m4 > acinclude.m4
aclocal
automake --foreign --add-missing --copy
autoconf
