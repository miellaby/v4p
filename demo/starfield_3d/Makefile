TOP=../..
EXES=starfield_3d
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/game_engine -I$(TOP)/extra/v4pserial

starfield_3d: starfield_3d.o libv4p.a $(TOP)/extra/game_engine/libg4p.a
	$(CC) -o $@ starfield_3d.o \
	  -L$(TOP)/extra/game_engine -L$(TOP)/extra/v4pserial -lv4p -lv4pserial -lg4p $(CPPFLAGS) -lm