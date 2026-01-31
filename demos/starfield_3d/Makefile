TOP=../..
EXES=starfield_3d
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/addons/game_engine -I$(TOP)/addons/v4pserial

starfield_3d: starfield_3d.o libv4p.a $(TOP)/addons/game_engine/libg4p.a
	$(CC) -o $@ starfield_3d.o \
	  -L$(TOP)/addons/game_engine -L$(TOP)/addons/v4pserial -lv4p -lv4pserial -lg4p $(CPPFLAGS) -lm