TOP=../..
EXES=asteroids
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/addons/game_engine -I$(TOP)/addons/v4pserial

asteroids: asteroids.o libv4p.a $(TOP)/addons/game_engine/libg4p.a $(TOP)/addons/v4pserial/libv4pserial.a
	$(CC) -o $@ asteroids.o -L$(TOP)/addons/game_engine -L$(TOP)/addons/v4pserial -lv4p -lg4p -lv4pserial -lSDL -lm $(CPPFLAGS)