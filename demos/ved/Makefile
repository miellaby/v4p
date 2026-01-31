# Project big lines
TOP=../..
EXES = ved
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/addons/game_engine

ved: editor.o $(TOP)/libv4p.a $(TOP)/addons/game_engine/libg4p.a
	$(CC) -o $@ editor.o \
	  -L$(TOP)/addons/game_engine -lv4p -lg4p $(CPPFLAGS)
