TOP=../..
EXES=palette
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/game_engine

palette: palette.o libv4p.a $(TOP)/extra/game_engine/libg4p.a
	$(CC) -o $@ palette.o -L$(TOP)/extra/game_engine -lv4p -lg4p $(CPPFLAGS)
