# Project big lines
TOP=../..
EXES = ved
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/gamemachine

ved: editor.o $(TOP)/libv4p.a $(TOP)/extra/gamemachine/libgm.a
	$(CC) $(CPPFLAGS) -o $@ editor.o \
	  -L$(TOP)/extra/gamemachine -lv4p -lgm 
