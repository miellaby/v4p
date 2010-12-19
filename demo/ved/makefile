# Project big lines
TOP=../..
EXES = ved
include $(TOP)/rules.mak

ved: editor.o $(TOP)/libv4p.a $(TOP)/libgm.a
	$(CC) $(CPPFLAGS) -o $@ editor.o -L$(TOP) -lv4p -lgm 
