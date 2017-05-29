#!/bin/sh


CC=gcc

xvfb: xvfb.c hello.c xvfb.h
	$(CC) -g -o  xvfb xvfb.c hello.c `pkg-config --cflags --libs gtk+-2.0` -lminigui_ths -lpthread -ljpeg -lpng


