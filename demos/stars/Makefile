TOP=../..
EXES=stars
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/addons/game_engine -I$(TOP)/addons/v4pserial
stars: stars.o libv4p.a $(TOP)/addons/game_engine/libg4p.a $(TOP)/addons/v4pserial/libv4pserial.a
	$(CC) -o $@ stars.o \
	  -L$(TOP)/addons/game_engine -L$(TOP)/addons/v4pserial -lv4p -lv4pserial -lg4p \
	   $(CPPFLAGS)
